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
    
    essentia::Pool _pool;

    essentia::Real _sampleRate = 44100.0;
    int _frameSize = 2048;
    int _hopSize = 128;
    
    std::vector<essentia::Real> _audioBuffer;
    std::vector<std::vector<essentia::Real>> _multiPitch;
    
    essentia::streaming::Algorithm* _audioLoader;
    essentia::streaming::Algorithm* _el;
    essentia::streaming::Algorithm* _multiPitchEstimateMelodia;
    
    void createAlgorithms();
    void connectAlgorithms();
    void runNetwork();
    
public:
    MultiPitchEstimateWithMelodia(std::string audioFilename): _audioFilename(audioFilename) {
        essentia::init();
        
        createAlgorithms();
        connectAlgorithms();
        runNetwork();
    }
};
#endif /* hpcpExtractor_hpp */
