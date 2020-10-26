//
//  main.cpp
//  Analyser
//
//  Created by SeongMuk Gang on 20/10/2020.
//  Copyright © 2020 GlobalMusic. All rights reserved.
//
/*
#include <iostream>
#include <essentia/algorithmfactory.h>
#include <essentia/streaming/algorithms/poolstorage.h>
#include <essentia/scheduler/network.h>

int main(int argc, const char * argv[]) {
    std::string audioFilename = argv[1];
    std::string outputFilename = argv[2];
    
    essentia::init();
    
    essentia::Pool pool;
    
    essentia::Real sampleRate = 44100.0;
    int frameSize = 2048;
    int hopSize = 1024;
    
    essentia::streaming::AlgorithmFactory& factory = essentia::streaming::AlgorithmFactory::instance();
    essentia::streaming::Algorithm* audio   = factory.create("MonoLoader",
                                                           "filename", audioFilename,
                                                           "sampleRate", sampleRate);
    
    essentia::streaming::Algorithm* fc      = factory.create("FrameCutter",
                                                             "frameSize", frameSize,
                                                             "hopSize", hopSize,
                                                             "silentFrames", "noise");
    
    essentia::streaming::Algorithm* w       = factory.create("Windowing",
                                                             "type", "blackmanharris62");
    
    essentia::streaming::Algorithm* spec    = factory.create("Spectrum");
    essentia::streaming::Algorithm* mfcc    = factory.create("MFCC");
    
    // ** CONNECTING THE ALGORITHMS ** //
    std::cout << "-------- connecting algos --------" << std::endl;
    
    audio->output("audio")      >> fc->input("signal");
    fc->output("frame")         >> w->input("frame");
    w->output("frame")          >> spec->input("frame");
    spec->output("spectrum")    >> mfcc->input("spectrum");
    
    mfcc->output("bands")       >> essentia::streaming::NOWHERE;
    mfcc->output("mfcc")        >> essentia::streaming::PoolConnector(pool, "lowlevel.mfcc");
    
    // ** STARTING THE ALGORITHMS  ** //
    std::cout << "-------- start processing " << audioFilename << " --------" << std::endl;
    
    essentia::scheduler::Network n(audio);
    n.run();
    
    // aggregate the results
    essentia::Pool aggrPool; // the pool with the aggregated MFCC values
    const char* stats[] = { "mean", "var", "min", "max", "cov", "icov" };

    essentia::standard::Algorithm* aggr = essentia::standard::AlgorithmFactory::create("PoolAggregator",
                                                                   "defaultStats", essentia::arrayToVector<std::string>(stats));

    aggr->input("input").set(pool);
    aggr->output("output").set(aggrPool);
    aggr->compute();

    aggrPool.merge("lowlevel.mfcc.frames", pool.value<std::vector<std::vector<essentia::Real> > >("lowlevel.mfcc"));

    // write results to file
    std::cout << "-------- writing results to file " << outputFilename << " --------" << std::endl;

    essentia::standard::Algorithm* output = essentia::standard::AlgorithmFactory::create("YamlOutput",
                                                                     "filename", outputFilename);
    output->input("pool").set(aggrPool);
    output->compute();

    // NB: we could just wait for the network to go out of scope, but then this would happen
    //     after the call to essentia::shutdown() where the FFTW structs would already have
    //     been freed, so let's just delete everything explicitly now
    n.clear();

    delete aggr;
    delete output;
    essentia::shutdown();
    
    return 0;
}
*/

#include <iostream>
//#include "streamingMode.hpp"
#include "standardMode.hpp"
#include "multiPitchEstimateWithMelodia.hpp"

int main(int argc, const char * argv[]) {
    std::string a1(argv[1]);
//    std::string a2(argv[2]);
    
//    std::cout << a1 << ", " << a2 << std::endl;
//    StreamingMode strm(a1, a2);
//    StandardMode stdm(a1, a2);
    MultiPitchEstimateWithMelodia mpe(a1);
    return 0;
}
