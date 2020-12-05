//
//  audioMixer.hpp
//  Analyser
//
//  Created by SeongMuk Gang on 03/12/2020.
//  Copyright © 2020 GlobalMusic. All rights reserved.
//

#ifndef audioMixer_hpp
#define audioMixer_hpp

#include <iostream>

#include <essentia/algorithmfactory.h>
#include <essentia/streaming/algorithms/poolstorage.h>
#include <essentia/scheduler/network.h>

class AudioMixer {
private:
    int count;
    std::vector<std::string> audioFileNames;
    std::string outputFileName;
    
    essentia::Real sampleRate = 44100.0;
    int frameSize = 2048;
    int hopSize = 1024;
    
    std::vector<essentia::standard::Algorithm*> audioVec;
    essentia::standard::Algorithm* monoMixer;
    essentia::standard::Algorithm* monoWriter;
    
    std::vector<std::vector<essentia::Real>> audioBufferVec;
    std::vector<essentia::StereoSample> stereoBuffer;
    
    std::vector<essentia::Real> monoOutput;
    
public:
    AudioMixer(std::vector<std::string> inFileNames) {
        audioFileNames = inFileNames;
        count = static_cast<int>(audioFileNames.size());
        
        // Prepare audio buffer in audio vector
        for (int i=0; i<count; i++) {
            std::vector<essentia::Real> audioBuffer;
            audioBufferVec.push_back(audioBuffer);
        }
        
        essentia::init();
        
        createAlgorithms();
        connectAlgorithms();
        computeAlgorithms();
    }
    void createAlgorithms();
    void connectAlgorithms();
    void computeAlgorithms();
};
#endif /* audioMixer_hpp */
