//
//  multiPitchEstimationStreaming.hpp
//  Analyser
//
//  Created by SeongMuk Gang on 26/10/2020.
//  Copyright © 2020 GlobalMusic. All rights reserved.
//

#ifndef multiPitchEstimationStreaming_hpp
#define multiPitchEstimationStreaming_hpp

#include <iostream>
#include <essentia/algorithmfactory.h>
#include <essentia/streaming/algorithms/poolstorage.h>
#include <essentia/scheduler/network.h>

class MultiPitchEstimationStreaming {
    std::string _audioFileName;
    
    essentia::Real _sampleRate = 44100.0;
    int _frameSize = 2048;
    int _hopSize = 128;
    
    std::vector<essentia::Real> _inAudioBuffer;
    std::vector<essentia::Real> _audioElBuffer;
    std::vector<std::vector<essentia::Real>> _multiPitchEstimate;
    std::vector<essentia::Real> _onset;
    std::vector<essentia::Real> _duration;
    std::vector<essentia::Real> _midiPitchEstimate;
};

#endif /* multiPitchEstimationStreaming_hpp */
