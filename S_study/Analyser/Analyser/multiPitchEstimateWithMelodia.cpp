//
//  hpcpExtractor.cpp
//  Analyser
//
//  Created by SeongMuk Gang on 21/10/2020.
//  Copyright © 2020 GlobalMusic. All rights reserved.
//

#include "multiPitchEstimateWithMelodia.hpp"

void MultiPitchEstimateWithMelodia::createAlgorithms() {
    essentia::standard::AlgorithmFactory& factory = essentia::standard::AlgorithmFactory::instance();
    
    _audioLoader        = factory.create("MonoLoader",
                               "filename", _audioFilename,
                               "sampleRate", _sampleRate,
                               "downmix", "mix");
    
    _el = factory.create("EqualLoudness",
                         "sampleRate", _sampleRate);
    
    _multiPitchMelodiaEstimator  = factory.create("MultiPitchMelodia",
                                                 "sampleRate", (int)_sampleRate,
                                                 "frameSize", _frameSize,
                                                 "hopSize", _hopSize);
    
    _midiPitchEstimator = factory.create("PitchContourSegmentation",
                                    "sampleRate", (int)_sampleRate,
                                    "hopSize", _hopSize);
}

void MultiPitchEstimateWithMelodia::connectAlgorithms() {
    _audioLoader->output("audio").set(_inAudioBuffer);
    _el->input("signal").set(_inAudioBuffer);
    _el->output("signal").set(_audioElBuffer);
    _multiPitchMelodiaEstimator->input("signal").set(_audioElBuffer);
    _multiPitchMelodiaEstimator->output("pitch").set(_multiPitchEstimate);
    
    _midiPitchEstimator->input("pitch").set(_frameMultiPitchEstimate);
    _midiPitchEstimator->input("signal").set(_inAudioBuffer);
    _midiPitchEstimator->output("onset").set(_onset);
    _midiPitchEstimator->output("duration").set(_duration);
    _midiPitchEstimator->output("MIDIpitch").set(_midiPitchEstimate);
}


void MultiPitchEstimateWithMelodia::computeNetwork() {
    // Get audio buffer
    _audioLoader->compute();
    
    // Process
    _el->compute();
    _multiPitchMelodiaEstimator->compute();
    
//    std::cout << "time   pitch [Hz]" << std::endl;
//    for (int frameCount=0; frameCount < (int)_multiPitchEstimate.size(); frameCount++) {
//        for (int pitchCount=0; pitchCount < (int)_multiPitchEstimate[frameCount].size(); pitchCount++) {
//            std::fprintf(stdout, "%f S \t %f [Hz]\n", float(frameCount)*float(_hopSize)/float(_sampleRate), _multiPitchEstimate[frameCount][pitchCount]);
//        }
//    }
    for (auto const& frame: _multiPitchEstimate) {
        for (auto const& pitch: frame) {
            _frameMultiPitchEstimate.push_back(pitch);
        }
        _midiPitchEstimator->compute();
    }
    
    for (int p=0; p < (int)_midiPitchEstimate.size(); p++) {
        std::fprintf(stdout, "%f S \t -> %f S \t : %f\n", _onset[p], _duration[p], _midiPitchEstimate[p]);
    }
    
    delete _audioLoader;
    delete _el;
    delete _multiPitchMelodiaEstimator;
    delete _midiPitchEstimator;
    
    essentia::shutdown();
}
