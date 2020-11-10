//
//  main.cpp
//  midiGeneration
//
//  Created by SeongMuk Gang on 10/11/2020.
//  Copyright © 2020 GlobalMusic. All rights reserved.
//

#include <iostream>
#include <fluidsynth.h>
#include <thread>
#include <chrono>

int main(int argc, const char * argv[]) {
    fluid_settings_t* settings = new_fluid_settings();
    fluid_settings_setstr(settings, "audio.file.name", "output.wav");
    fluid_settings_setstr(settings, "player.timing-source", "sample");
    fluid_settings_setint(settings, "synth.lock-memory", 0);
    
    fluid_synth_t* synth = new_fluid_synth(settings);
    fluid_audio_driver_t* adriver = new_fluid_audio_driver(settings, synth);
    
    // fluid_synth_sfload(synth, argv[1], 1);
    fluid_synth_sfload(synth, "DATA/MuseScore_General.sf2", 1);
    // fluid_synth_sfload(synth, "/Applications/MuseScore\ 3.app/Contents/Resources/sound/MuseScore_General.sf3", 1);
    
    // succesfully loaded soundfont...play something
    fluid_synth_noteon(synth, 0, 60, 100); // play middle C
    std::this_thread::sleep_for(std::chrono::seconds(1)); // sleep for 1 second
    
    delete_fluid_audio_driver(adriver);
    delete_fluid_synth(synth);
    delete_fluid_settings(settings);
    
    
    std::cout << "Hello, !\n";
    return 0;
}
