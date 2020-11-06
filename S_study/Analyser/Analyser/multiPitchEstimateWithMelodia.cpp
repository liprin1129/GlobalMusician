//
//  hpcpExtractor.cpp
//  Analyser
//
//  Created by SeongMuk Gang on 21/10/2020.
//  Copyright © 2020 GlobalMusic. All rights reserved.
//

#include "multiPitchEstimateWithMelodia.hpp"

void MultiPitchEstimateWithKlapuri::createAlgorithms() {
    essentia::standard::AlgorithmFactory& factory = essentia::standard::AlgorithmFactory::instance();
    
    _monoLoader = factory.create("MonoLoader", "filename", _audioFilename,
                           "sampleRate", _sampleRate,
                           "downmix", "mix");
    
    // Pre-processing
    _frameCutter = factory.create("FrameCutter", "frameSize", _frameSize,
                            "hopSize", _hopSize,
                            "startFromZero", false);

    _windowing = factory.create("Windowing", "size", _frameSize,
                          "zeroPadding", (_zeroPaddingFactor-1) * _frameSize,
                          "type", _windowType);
    // Spectral peaks
    _spectrum = factory.create("Spectrum", "size", _frameSize * _zeroPaddingFactor);


    _spectralPeaks = factory.create("SpectralPeaks",
                "minFrequency", 1,  // to avoid zero frequencies
                "maxFrequency", 20000,
                "maxPeaks", _maxSpectralPeaks,
                "sampleRate", _sampleRate,
                "magnitudeThreshold", 0,
                "orderBy", "magnitude");
    
    // Spectral whitening
    _spectralWhitening = factory.create("SpectralWhitening", "sampleRate", _sampleRate);
    
    // Pitch salience contours
    _pitchSalienceFunction = factory.create("PitchSalienceFunction", "binResolution", _binResolution,
                    "referenceFrequency", _referenceFrequency,
                    "magnitudeThreshold", magnitudeThreshold,
                    "magnitudeCompression", magnitudeCompression,
                    "numberHarmonics", _numberHarmonics,
                    "harmonicWeight", harmonicWeight);

    // pitch salience function peaks are considered F0 cadidates -> limit to considered frequency range
    _pitchSalienceFunctionPeaks = factory.create("PitchSalienceFunctionPeaks", "binResolution", _binResolution,
                       "referenceFrequency", _referenceFrequency,
                       "minFrequency", minFrequency,
                       "maxFrequency", maxFrequency);
}

void MultiPitchEstimateWithKlapuri::connectAlgorithms() {
    // Pre-processing
    std::vector<essentia::Real> audioBuffer;
    _monoLoader->output("audio").set(audioBuffer);
    
    std::vector<essentia::Real> frame;
    _frameCutter->input("signal").set(audioBuffer);
    _frameCutter->output("frame").set(frame);

    std::vector<essentia::Real> frameWindowed;
    _windowing->input("frame").set(frame);
    _windowing->output("frame").set(frameWindowed);

    // Spectral peaks
    std::vector<essentia::Real> frameSpectrum;
    _spectrum->input("frame").set(frameWindowed);
    _spectrum->output("spectrum").set(frameSpectrum);

    std::vector<essentia::Real> frameFrequencies;
    std::vector<essentia::Real> frameMagnitudes;
    _spectralPeaks->input("spectrum").set(frameSpectrum);
    _spectralPeaks->output("frequencies").set(frameFrequencies);
    _spectralPeaks->output("magnitudes").set(frameMagnitudes);

    // Spectral whitening
    std::vector<essentia::Real> frameWhiteMagnitudes;
    _spectralWhitening->input("spectrum").set(frameSpectrum);
    _spectralWhitening->input("frequencies").set(frameFrequencies);
    _spectralWhitening->input("magnitudes").set(frameMagnitudes);
    _spectralWhitening->output("magnitudes").set(frameWhiteMagnitudes);
    
    // Pitch salience contours
    std::vector<essentia::Real> frameSalience;
    _pitchSalienceFunction->input("frequencies").set(frameFrequencies);
    _pitchSalienceFunction->input("magnitudes").set(frameMagnitudes);
    _pitchSalienceFunction->output("salienceFunction").set(frameSalience);

    std::vector<essentia::Real> frameSalienceBins;
    std::vector<essentia::Real> frameSalienceValues;
    _pitchSalienceFunctionPeaks->input("salienceFunction").set(frameSalience);
    _pitchSalienceFunctionPeaks->output("salienceBins").set(frameSalienceBins);
    _pitchSalienceFunctionPeaks->output("salienceValues").set(frameSalienceValues);

    std::vector<essentia::Real> nearestBinWeights;
    nearestBinWeights.resize(_binsInSemitone + 1);
    for (int b=0; b <= _binsInSemitone; b++) {
      nearestBinWeights[b] = pow(cos((essentia::Real(b)/_binsInSemitone)* M_PI/2), 2);
    }
    
    _monoLoader->compute();
    
    while (true) {
      // get a frame
      _frameCutter->compute();

      if (!frame.size()) {
        break;
      }

      _windowing->compute();

      // calculate spectrum
      _spectrum->compute();

      // calculate spectral peaks
      _spectralPeaks->compute();
      
      // whiten the spectrum
      _spectralWhitening->compute();

      // calculate salience function
      _pitchSalienceFunction->compute();

      // calculate peaks of salience function
      _pitchSalienceFunctionPeaks->compute();
        
      // no peaks in this frame
      if (!frameSalienceBins.size()) {
        continue;
      }

      // Joint F0 estimation (pitch salience function peaks as candidates)

      // compute the cent-scaled spectrum
      fill(_centSpectrum.begin(), _centSpectrum.end(), (essentia::Real) 0.0);
      for (int i=0; i<(int)frameSpectrum.size(); i++) {
        essentia::Real f = (essentia::Real(i) / essentia::Real(frameSpectrum.size())) * (_sampleRate/2);
        int k = frequencyToCentBin(f);
        if (k>0 && k<_numberBins) {
          _centSpectrum[k] += frameSpectrum[i];
        }
      }
    
      // get indices corresponding to harmonics of each found peak
      std::vector<std::vector<int> > kPeaks;
      for (int i=0; i<(int)frameSalienceBins.size(); i++) {
        std::vector<int> k;
        essentia::Real f = _referenceFrequency * pow(_centToHertzBase, frameSalienceBins[i]);
        for (int m=0; m<_numberHarmonicsMax; m++) {
          // find the exact peak for each harmonic
          int kBin = frequencyToCentBin(f*(m+1));
          int kBinMin = std::max(0, int(kBin-_binsInSemitone));
          int kBinMax = std::min(_numberBins-1, int(kBin+_binsInSemitone));
          std::vector<essentia::Real> specSegment;
          for (int ii=kBinMin; ii<=kBinMax; ii++) {
            specSegment.push_back(_centSpectrum[ii]);
          }
          kBin = kBinMin + essentia::argmax(specSegment)-1;
          k.push_back(kBin);
        }
        kPeaks.push_back(k);
      }
      
      // candidate Spectra
      std::vector<std::vector<essentia::Real> > Z;
      for (int i=0; i<(int)frameSalienceBins.size(); i++) {
        std::vector<essentia::Real> z(_numberBins, 0.);
        for (int h=0; h<_numberHarmonicsMax; h++) {
          int hBin = kPeaks[i][h];
          for(int b = std::max(0, hBin-_binsInSemitone); b <= std::min(_numberBins-1, hBin+_binsInSemitone); b++) {
            z[b] += nearestBinWeights[abs(b-hBin)] * getWeight(hBin, h) * 0.25; // 0.25 is cancellation parameter
          }
        }
        Z.push_back(z);
      }

      // TODO: segfault somewhere here
      // inhibition function
      int numCandidates = (int)frameSalienceBins.size();
      std::vector<std::vector<essentia::Real> > inhibition;

      for (int i=0; i<numCandidates; i++) {
        std::vector<essentia::Real> inh(numCandidates, 0.);
        for (int j=0; j<numCandidates; j++) {
          for (int m=0; m<_numberHarmonicsMax; m++) {
            inh[j] += getWeight(kPeaks[i][m], m) * _centSpectrum[kPeaks[i][m]] * Z[j][kPeaks[i][m]];
          }
        }
        inhibition.push_back(inh);
      }

      // polyphony estimation initialization
      std::vector<int> finalSelection;
      int p = 1;
      essentia::Real gamma = 0.73;
      essentia::Real S = frameSalienceValues[essentia::argmax(frameSalienceValues)] / pow(p,gamma);
      finalSelection.push_back(essentia::argmax(frameSalienceValues));
      
      // goodness function
      std::vector<std::vector<essentia::Real> > G;
      for (int i=0; i<numCandidates; i++) {
        std::vector<essentia::Real> g;
        for (int j=0; j<numCandidates; j++) {
          if(i==j) {
            g.push_back(0.0);
          } else {
            essentia::Real g_val = frameSalienceValues[i] + frameSalienceValues[j] - (inhibition[i][j] + inhibition[j][i]);
            g.push_back(g_val);
          }
        }
        G.push_back(g);
      }
    
      std::vector<std::vector<int> > selCandInd;
      std::vector<essentia::Real> selCandVal;
      std::vector<essentia::Real> localF0;
      
      while (true) {
        // find numCandidates largest values
        essentia::Real maxVal=-1;
        int maxInd_i=0;
        int maxInd_j=0;
    
        for (int I=0; I < numCandidates; I++) {
          std::vector<int> localInd;
          for (int i=0; i < numCandidates; i++) {
            for (int j=0; j < numCandidates; j++) {
              if (G[i][j] > maxVal) {
                maxVal = G[i][j];
                maxInd_i = i;
                maxInd_j = j;
              }
            }
          }

          localInd.push_back(maxInd_i);
          localInd.push_back(maxInd_j);
          selCandInd.push_back(localInd);
          selCandVal.push_back(G[maxInd_i][maxInd_j]);
          G[maxInd_i][maxInd_j] -= 1;
          maxVal -= 1;
          maxInd_i = 0;
          maxInd_j = 0;
        }
    
        // re-estimate polyphony
        p++;
        essentia::Real Snew = selCandVal[essentia::argmax(selCandVal)] / pow(p,gamma);
        if (Snew > S) {
          finalSelection.clear();
          for (int i=0; i<(int)selCandInd[0].size(); i++) {
            finalSelection.push_back(selCandInd[0][i]);
          }
          // re-calculate goddess function
          for (int i=0; i<numCandidates; i++) {
            for (int j=0; j<numCandidates; j++) {
              G[i][j] += frameSalienceValues[j];
              for (int ii=0; ii<(int)selCandInd[i].size(); ii++) {
                G[i][j] -= (inhibition[selCandInd[i][ii]][j] + inhibition[j][selCandInd[i][ii]]);
              }
            }
          }
          S = Snew;
        }
        else {
          // add estimated f0 to frame
          for (int i=0; i<(int)finalSelection.size(); i++) {
            essentia::Real freq = _referenceFrequency * pow(_centToHertzBase, frameSalienceBins[finalSelection[i]]);
            localF0.push_back(freq);
          }
          break;
        }
      }
        for (auto const& lf0: localF0) {
                std::cout << lf0 << std::endl;
        }
    }
}


void MultiPitchEstimateWithKlapuri::computeNetwork() {

}

int MultiPitchEstimateWithKlapuri::frequencyToCentBin(essentia::Real frequency) {
    // +0.5 term is used instead of +1 (as in [1]) to center 0th bin to 55Hz
    // formula: floor(1200 * log2(frequency / _referenceFrequency) / _binResolution + 0.5)
    //  --> 1200 * (log2(frequency) - log2(_referenceFrequency)) / _binResolution + 0.5
    //  --> 1200 * log2(frequency) / _binResolution + (0.5 - 1200 * log2(_referenceFrequency) / _binResolution)
    return floor(_binsInOctave * log2(frequency) + _referenceTerm);
}

essentia::Real MultiPitchEstimateWithKlapuri::getWeight(int centBin, int harmonicNumber) {
  essentia::Real f = _referenceFrequency * pow(_centToHertzBase, centBin);
  essentia::Real alpha = 27.0;
  essentia::Real beta = 320.0;
  essentia::Real w = (f+alpha) / (harmonicNumber*f+beta);
  return w;
}
