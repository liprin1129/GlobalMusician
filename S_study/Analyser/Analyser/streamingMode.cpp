//
//  streamingMode.cpp
//  Analyser
//
//  Created by SeongMuk Gang on 20/10/2020.
//  Copyright © 2020 GlobalMusic. All rights reserved.
//

#include "streamingMode.hpp"

void StreamingMode::createAlgorithms() {
    essentia::streaming::AlgorithmFactory& factory = essentia::streaming::AlgorithmFactory::instance();
     
    audio = factory.create("MonoLoader",
                           "filename", audioFilename,
                           "sampleRate", sampleRate);
    
    fc = factory.create("FrameCutter",
                        "frameSize", frameSize,
                        "hopSize", hopSize,
                        "silentFrames", "noise");
    
    w = factory.create("Windowing",
                       "type", "blackmanharris62");
    spec = factory.create("Spectrum");
    mfcc = factory.create("MFCC");
}

void StreamingMode::connectAlgorithms() {
    std::cout << "-------- connecting algos --------" << std::endl;

    audio->output("audio")      >> fc->input("signal");
    fc->output("frame")         >> w->input("frame");
    w->output("frame")          >> spec->input("frame");
    spec->output("spectrum")    >> mfcc->input("spectrum");

    mfcc->output("bands")       >> essentia::streaming::NOWHERE;
    mfcc->output("mfcc")        >> essentia::streaming::PoolConnector(pool, "lowlevel.mfcc");
}

void StreamingMode::runNetwork() {
    // ** STARTING THE ALGORITHMS  ** //
    std::cout << "-------- start processing " << audioFilename << " --------" << std::endl;

    essentia::scheduler::Network n(audio);
    n.run();
    
    for (auto const &i: pool.value<std::vector<std::vector<essentia::Real>>>("lowlevel.mfcc")) {
        for (auto const &j: i) {
            std::cout << j << std::endl;
        }
    }
    /*
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
     */
}
