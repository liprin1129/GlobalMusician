//
//  hpcpExtractor.hpp
//  Analyser
//
//  Created by SeongMuk Gang on 21/10/2020.
//  Copyright © 2020 GlobalMusic. All rights reserved.
//

#ifndef hpcpExtractor_hpp
#define hpcpExtractor_hpp

#include <iostream>
#include <essentia/algorithmfactory.h>
#include <essentia/streaming/algorithms/poolstorage.h>
#include <essentia/scheduler/network.h>

class MultiPitchEstimateWithMelodia {
private:
    std::string _audioFilename;
    std::vector<std::vector<essentia::Real>>  _pitch;

    // fundamental variables
    essentia::Real _sampleRate          = 44100.;
    int _frameSize                      = 2048;
    int _hopSize                        = 128;
    int _zeroPaddingFactor              = 4;
    essentia::Real _referenceFrequency  = 55.0;
    essentia::Real _binResolution       = 10.0;
    int _numberHarmonics                = 10;

    essentia::Real magnitudeThreshold   = 40;
    essentia::Real magnitudeCompression = 1.0;
    essentia::Real harmonicWeight       = 0.8;
    essentia::Real minFrequency         = 80.0;
    essentia::Real maxFrequency         = 1760.0;
    
    // Derived variables
    std::min(_numberHarmonics, floor(_sampleRate / maxFrequency));
    int _numberHarmonicsMax             = std::min(_numberHarmonics, floor(_sampleRate / maxFrequency));
    essentia::Real _centToHertzBase     = pow(2, _binResolution / 1200.0);
    int _binsInSemitone                 = floor(100.0 / _binResolution);
    int _binsInOctave;
    essentia::Real _referenceTerm;
    std::vector<essentia::Real> _centSpectrum;
    int _numberBins;
    
    // Pre-processing
    essentia::standard::Algorithm* _monoLoader;
    essentia::standard::Algorithm* _frameCutter;
    essentia::standard::Algorithm* _windowing;

    // Spectral peaks and whitening
    essentia::standard::Algorithm* _spectrum;
    essentia::standard::Algorithm* _spectralPeaks;
    essentia::standard::Algorithm* _spectralWhitening;

    // Pitch salience function
    essentia::standard::Algorithm* _pitchSalienceFunction;
    essentia::standard::Algorithm* _pitchSalienceFunctionPeaks;
    
    void createAlgorithms();
    void connectAlgorithms();
    void computeNetwork();
    
public:
    MultiPitchEstimateWithMelodia(std::string audioFilename): _audioFilename(audioFilename) {
        essentia::init();
        
        createAlgorithms();
        connectAlgorithms();
        computeNetwork();
    }
};
#endif /* hpcpExtractor_hpp */
