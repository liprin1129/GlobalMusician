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
    int _frameSize = 8000;//2048;
    int _hopSize = 128;
    
    std::vector<essentia::Real> _inAudioBuffer;
    std::vector<essentia::Real> _audioElBuffer;
    std::vector<essentia::Real> _frame, _windowedFrame;
    std::vector<std::vector<essentia::Real>> _multiPitchEstimate;
    std::vector<essentia::Real> _onset;
    std::vector<essentia::Real> _duration;
    std::vector<essentia::Real> _midiPitchEstimate;
    
    std::vector<essentia::Real> _frameMultiPitchEstimate;
    
    essentia::standard::Algorithm* _audioLoader;
    essentia::standard::Algorithm* _fc;
    essentia::standard::Algorithm* _w;
    
    essentia::standard::Algorithm* _el;
    essentia::standard::Algorithm* _multiPitchMelodiaEstimator;
    essentia::standard::Algorithm* _midiPitchEstimator;
    
    //essentia::standard::Algorithm*
    
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
