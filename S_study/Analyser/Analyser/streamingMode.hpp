//
//  streamingMode.hpp
//  Analyser
//
//  Created by SeongMuk Gang on 20/10/2020.
//  Copyright © 2020 GlobalMusic. All rights reserved.
//

#ifndef streamingMode_hpp
#define streamingMode_hpp

#include <iostream>
#include <essentia/algorithmfactory.h>
#include <essentia/streaming/algorithms/poolstorage.h>
#include <essentia/scheduler/network.h>

class StreamingMode {
private:
    std::string audioFilename;
    std::string outputFilename;
    
    essentia::Pool pool;

    essentia::Real sampleRate = 44100.0;
    int frameSize = 2048;
    int hopSize = 1024;
    
    essentia::streaming::Algorithm* audio;
    essentia::streaming::Algorithm* fc;
    essentia::streaming::Algorithm* w;
    essentia::streaming::Algorithm* spec;
    essentia::streaming::Algorithm* mfcc;

    void createAlgorithms();
    void connectAlgorithms();
    void runNetwork();
    
public:
    StreamingMode(std::string argv1, std::string argv2) {
        audioFilename = argv1;
        outputFilename = argv2;
        
//        std::cout << argv1 << ", " << argv2 << std::endl;
        
        essentia::init();
        
        createAlgorithms();
        // ** CONNECTING THE ALGORITHMS ** //
        connectAlgorithms();
        runNetwork();
    };
};
#endif /* streamingMode_hpp */
