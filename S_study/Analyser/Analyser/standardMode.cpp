//
//  standardMode.cpp
//  Analyser
//
//  Created by SeongMuk Gang on 21/10/2020.
//  Copyright © 2020 GlobalMusic. All rights reserved.
//

#include "standardMode.hpp"

void StandardMode::createAlgorithms() {
    essentia::standard::AlgorithmFactory& factory = essentia::standard::AlgorithmFactory::instance();
    
    audio    = factory.create("MonoLoader",
                              "filename", audioFilename,
                              "sampleRate", sampleRate);
    
    fc       = factory.create("FrameCutter",
                              "frameSize", frameSize,
                              "hopSize", hopSize);
    
    w        = factory.create("Windowing",
                              "type", "blackmanharris62");
    
    spec     = factory.create("Spectrum");
    mfcc     = factory.create("MFCC");
}

void StandardMode::connectAlgorithms() {
    // Audio -> FrameCutter
//    std::vector<essentia::Real> audioBuffer;
    
    audio->output("audio").set(audioBuffer);
    fc->input("signal").set(audioBuffer);
    
    // FrameCutter -> Windowing -> Spectrum
//    std::vector<essentia::Real> frame, windowedFrame;
    
    fc->output("frame").set(frame);
    w->input("frame").set(frame);
    
    w->output("frame").set(windowedFrame);
    spec->input("frame").set(windowedFrame);
    
    // Spectrum -> MFCC
//    std::vector<essentia::Real> spectrum, mfccCoeffs, mfccBands;
    
    spec->output("spectrum").set(spectrum);
    mfcc->input("spectrum").set(spectrum);
    
    mfcc->output("bands").set(mfccBands);
    mfcc->output("mfcc").set(mfccCoeffs);
}

void StandardMode::computeAlgorithms() {
    audio->compute();
    
    while (true) {
        fc->compute();
        
        if (!frame.size()) {
            break;
        }
        
        w->compute();
        spec->compute();
        mfcc->compute();
        
        // pool.add("lowlevel.mfcc", mfccCoeffs);
        pool.set("lowlevel.mfcc", mfccCoeffs);
        
        for (auto i: pool.value<std::vector<essentia::Real>>("lowlevel.mfcc")) {
            std::cout << i << std::endl;
        }
        
        break;
    }
    /*
    essentia::Pool aggrPool;
    const char* stats[] = { "mean", "var", "min", "max" };
    
    essentia::standard::Algorithm* aggr = essentia::standard::AlgorithmFactory::create("PoolAggregator",
                                                                                       "defaultStats",
                                                                                       essentia::arrayToVector<std::string>(stats));
    
    aggr->input("input").set(pool);
    aggr->output("output").set(aggrPool);
    aggr->compute();
    
    // ** WRITE RESULTS TO FILE ** //
    std::cout << "-------- writing results to file " << outputFilename << " ---------" << std::endl;
    
    essentia::standard::Algorithm* output = essentia::standard::AlgorithmFactory::create("YamlOutput",
                                                                                         "filename",
                                                                                         outputFilename);
    
    output->input("pool").set(aggrPool);
    output->compute();
    */
    delete audio;
    delete fc;
    delete w;
    delete spec;
    delete mfcc;
    //delete aggr;
    //delete output;
    
    essentia::shutdown();
}
