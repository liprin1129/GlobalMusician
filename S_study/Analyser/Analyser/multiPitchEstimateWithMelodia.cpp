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
    
    _fc = factory.create("FrameCutter",
                         "frameSize", 8000,
                         "hopSize", _hopSize);
    
    _w = factory.create("Windowing",
                        "type", "blackmanharris62");
    
    _el = factory.create("EqualLoudness",
                         //"sampleRate", _sampleRate);
                         "sampleRate", 8000);
    
    _multiPitchMelodiaEstimator  = factory.create("MultiPitchMelodia",
                                                 //"sampleRate", (int)_sampleRate/20,
                                                 "sampleRate", (int)8000/2,
                                                 "frameSize", (int)8000/2,
                                                 "hopSize", _hopSize);
    
//    _midiPitchEstimator = factory.create("PitchContourSegmentation",
//                                    "sampleRate", (int)_sampleRate,
//                                    "hopSize", _hopSize);
}

void MultiPitchEstimateWithMelodia::connectAlgorithms() {
    _audioLoader->output("audio").set(_inAudioBuffer);
    
    _fc->input("signal").set(_inAudioBuffer);
    _fc->output("frame").set(_frame);

    _w->input("frame").set(_frame);
    _w->output("frame").set(_windowedFrame);

    _el->input("signal").set(_windowedFrame);
//    _el->input("signal").set(_inAudioBuffer);
    _el->output("signal").set(_audioElBuffer);
    
    _multiPitchMelodiaEstimator->input("signal").set(_audioElBuffer);
    _multiPitchMelodiaEstimator->output("pitch").set(_multiPitchEstimate);
    
//    _midiPitchEstimator->input("pitch").set(_frameMultiPitchEstimate);
//    _midiPitchEstimator->input("signal").set(_inAudioBuffer);
//    _midiPitchEstimator->output("onset").set(_onset);
//    _midiPitchEstimator->output("duration").set(_duration);
//    _midiPitchEstimator->output("MIDIpitch").set(_midiPitchEstimate);
}


void MultiPitchEstimateWithMelodia::computeNetwork() {
    // Get audio buffer
    _audioLoader->compute();
    // This is only for loading an audio file. For real time processing, audio frame must be replace with input audio buffer.
    
    while (true) {
        _fc->compute();
        //std::cout << _frame[0] << std::endl;
        if (!_frame.size()) {
            break;
        }

        _w->compute();
        //_el->compute();
        std::cout << "equal loudness" << std::endl;
        std::cout << _audioElBuffer.size() << std::endl;
        
        _multiPitchMelodiaEstimator->compute();
        std::cout << "multi pitch estimate" << std::endl;
        std::cout << _multiPitchEstimate.size() << std::endl;
////        for (auto const& frame: _multiPitchEstimate) {
////            for (auto const& pitch: frame) {
////                _frameMultiPitchEstimate.push_back(pitch);
////                std::cout << pitch << std::endl;
////            }
////            _midiPitchEstimator->compute();
////        }
    }
    delete _audioLoader;
    delete _el;
    delete _multiPitchMelodiaEstimator;
    delete _midiPitchEstimator;
    
    essentia::shutdown();
}
