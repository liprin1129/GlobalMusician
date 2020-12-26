//
//  audioMixer.cpp
//  Analyser
//
//  Created by SeongMuk Gang on 03/12/2020.
//  Copyright © 2020 GlobalMusic. All rights reserved.
//

#include "audioMixer.hpp"

void AudioMixer::createAlgorithms() {
    essentia::standard::AlgorithmFactory& factory = essentia::standard::AlgorithmFactory::instance();
    
    for (auto audioFileName: audioFileNames) {
        audioVec.push_back(factory.create("MonoLoader",
                                          "filename", audioFileName,
                                          "sampleRate", sampleRate));
    }
    
    monoMixer = factory.create("MonoMixer",
                               "type", "mix");
    
    monoWriter = factory.create("MonoWriter",
                                "filename", outputFileName);
}

void AudioMixer::connectAlgorithms() {
    for (int i=0; i<count; i++) {
        audioVec[i]->output("audio").set(audioBufferVec[i]);
    }
    
    monoMixer->input("audio").set(stereoBuffer);
    monoMixer->input("numberChannels").set(2);
    monoMixer->output("audio").set(monoOutput);
    
    monoWriter->input("audio").set(monoOutput);
}

void AudioMixer::computeAlgorithms() {
    
    for (int i=0; i<count; i++) {
        audioVec[i] -> compute();
//        std::cout << audioBufferVec[i].front() << std::endl;
        
//        delete audioVec[i];
    }
    
    for (int i=0; i<audioBufferVec[0].size(); i++) {
//        std::tuple<essentia::Real, essentia::Real> tp = std::make_tuple(audioBufferVec[0][i], audioBufferVec[1][i]);
        
        essentia::StereoSample ss;
        ss.first = audioBufferVec[0][i];
        ss.second = audioBufferVec[1][i];
        
        stereoBuffer.push_back(ss);
    }
//    std::cout << "ok" << std::endl;
    monoMixer->compute();
//    std::cout << "ok2" << std::endl;
//    for (auto i: monoOutput) {
//        std::cout << i << std::endl;
//    }
    monoWriter->compute();
//    std::cout << "ok3" << std::endl;
    for (int i=0; i<count; i++) {
        delete audioVec[i];
    }
    delete monoMixer;
    essentia::shutdown();
}
