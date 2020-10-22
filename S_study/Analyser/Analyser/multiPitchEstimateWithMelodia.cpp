//
//  hpcpExtractor.cpp
//  Analyser
//
//  Created by SeongMuk Gang on 21/10/2020.
//  Copyright © 2020 GlobalMusic. All rights reserved.
//

#include "multiPitchEstimateWithMelodia.hpp"

void MultiPitchEstimateWithMelodia::createAlgorithms() {
    essentia::streaming::AlgorithmFactory& factory = essentia::streaming::AlgorithmFactory::instance();
    
    _audioLoader        = factory.create("MonoLoader",
                               "filename", _audioFilename,
                               "sampleRate", _sampleRate,
                               "downmix", "mix");
    
    _el = factory.create("EqualLoudness",
                         "sampleRate", _sampleRate);
    
    _multiPitchEstimateMelodia  = factory.create("MultiPitchMelodia",
                                                 "sampleRate", _sampleRate,
                                                 "frameSize", _frameSize,
                                                 "hopSize", _hopSize);
}

void MultiPitchEstimateWithMelodia::connectAlgorithms() {
    _audioLoader->output("audio") >> _el->input("signal");
    _el->output("signal") >> _multiPitchEstimateMelodia->input("signal");
    _multiPitchEstimateMelodia->output("pitch") >> essentia::streaming::PoolConnector(_pool, "multipitch.melodia");
}


void MultiPitchEstimateWithMelodia::runNetwork() {
    essentia::scheduler::Network n(_audioLoader);
    n.run();
    
    for(auto const& frame: _pool.value<std::vector<std::vector<essentia::Real>>>("multipitch.melodia")) {
        for(auto const& pitches: frame) {
            std::cout << pitches << std::endl;
        }
    }
    
    n.clear();
}
