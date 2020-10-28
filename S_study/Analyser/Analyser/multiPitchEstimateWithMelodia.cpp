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
    
    _monoLoader        = factory.create("MonoLoader",
                               "filename", _audioFilename,
                               "sampleRate", _sampleRate,
                               "downmix", "mix");
}

void MultiPitchEstimateWithMelodia::connectAlgorithms() {
    _audioLoader->output("audio").set(_inAudioBuffer);

}


void MultiPitchEstimateWithMelodia::computeNetwork() {

}
