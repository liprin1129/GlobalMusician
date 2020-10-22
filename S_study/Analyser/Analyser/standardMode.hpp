//
//  standardMode.hpp
//  Analyser
//
//  Created by SeongMuk Gang on 21/10/2020.
//  Copyright © 2020 GlobalMusic. All rights reserved.
//

#ifndef standardMode_hpp
#define standardMode_hpp

#include <iostream>
#include <essentia/algorithmfactory.h>
#include <essentia/streaming/algorithms/poolstorage.h>
#include <essentia/scheduler/network.h>

class StandardMode {
private:
    std::string audioFilename;
    std::string outputFilename;
    
    essentia::Pool pool;

    essentia::Real sampleRate = 44100.0;
    int frameSize = 2048;
    int hopSize = 1024;
    
    essentia::standard::Algorithm* audio;
    essentia::standard::Algorithm* fc;
    essentia::standard::Algorithm* w;
    essentia::standard::Algorithm* spec;
    essentia::standard::Algorithm* mfcc;

    std::vector<essentia::Real> audioBuffer;
    std::vector<essentia::Real> frame, windowedFrame;
    std::vector<essentia::Real> spectrum, mfccCoeffs, mfccBands;
    
    void createAlgorithms();
    void connectAlgorithms();
    void computeAlgorithms();
    
    public:
        StandardMode(std::string argv1, std::string argv2) {
            audioFilename = argv1;
            outputFilename = argv2;
            
            essentia::init();
            
            createAlgorithms();
            connectAlgorithms();
            computeAlgorithms();
        };
};
#endif /* standardMode_hpp */
