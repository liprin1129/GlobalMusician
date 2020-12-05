//
//  main.cpp
//  midiGeneration
//
//  Created by SeongMuk Gang on 10/11/2020.
//  Copyright © 2020 GlobalMusic. All rights reserved.
//
#include <cstdio>
#include <iostream>
#include <fluidsynth.h>
#include <thread>
#include <chrono>
#include <memory>

int main(int argc, const char * argv[]) {
    fluid_settings_t* settings = new_fluid_settings();
    fluid_settings_setstr(settings, "audio.file.name", "output.wav");
    fluid_settings_setstr(settings, "player.timing-source", "sample");
    fluid_settings_setint(settings, "synth.lock-memory", 0);
    fluid_settings_setnum(settings, "synth.sample-rate", 44100);
    fluid_settings_setstr(settings, "audio.file.format", "s16");
    
    fluid_synth_t* synth = new_fluid_synth(settings);
    fluid_synth_sfload(synth, "DATA/MuseScore_General.sf2", 1);
//    fluid_audio_driver_t* adriver = new_fluid_audio_driver(settings, synth);
    // fluid_synth_sfload(synth, argv[1], 1);
    // fluid_synth_sfload(synth, "/Applications/MuseScore\ 3.app/Contents/Resources/sound/MuseScore_General.sf3", 1);
    
    // Create sequncer
    fluid_sequencer_t* sequencer = new_fluid_sequencer2(0);
    fluid_seq_id_t synthSeqId = fluid_sequencer_register_fluidsynth(sequencer, synth);
    fluid_file_renderer_t *renderder = new_fluid_file_renderer(synth);
    
    // Select an instrument
    fluid_event_t *event = new_fluid_event();
    fluid_event_set_source(event, -1);
    fluid_event_set_dest(event, synthSeqId);
    fluid_event_program_change(event, 0, 0);
    fluid_sequencer_send_at(sequencer, event, 0, true);
    
    // Play the sound
    event = new_fluid_event();
    fluid_event_set_source(event, -1);
    fluid_event_set_dest(event, synthSeqId);
    fluid_event_note(event, 0, 50, 127, 1000);
    fluid_sequencer_send_at(sequencer, event, 0, true);
    
    // Stop the sound
    event = new_fluid_event();
    fluid_event_set_source(event, -1);
    fluid_event_set_dest(event, synthSeqId);
    fluid_event_all_sounds_off(event, 0);
    fluid_sequencer_send_at(sequencer, event, 1000, true);
    
    // Generate the sound to a file
    while (fluid_file_renderer_process_block(renderder) == FLUID_OK) {
        std::cout << "Saved to the file\n";
        if (fluid_sequencer_get_tick(sequencer)>1500) break;
    }
//    fluid_synth_noteon(synth, 0, 60, 100); // play middle C
//    std::this_thread::sleep_for(std::chrono::seconds(1)); // sleep for 1 second
    
    delete_fluid_settings(settings);
//    delete_fluid_audio_driver(adriver);
    delete_fluid_synth(synth);
    delete_fluid_sequencer(sequencer);
    delete_fluid_file_renderer(renderder);
    delete_fluid_event(event);
    
    return 0;
}

/*
#include <iostream>
#include <cstdio>
#include <fluidsynth.h>
#include <memory>
 
using namespace std;
 
 
template<typename _T>
using managed_ptr = unique_ptr<_T, void(*)(_T*)>;
 
int main()
{
    auto settings = managed_ptr<fluid_settings_t>{
            new_fluid_settings(), delete_fluid_settings };
// 출력 파일의 이름입니다.
    fluid_settings_setstr(settings.get(),
            "audio.file.name", "output.wav");
    fluid_settings_setstr(settings.get(),
            "player.timing-source", "sample");
    fluid_settings_setint(settings.get(),
            "synth.lock-memory", 0);
 
    auto synth = managed_ptr<fluid_synth_t>{
            new_fluid_synth(settings.get()), delete_fluid_synth };
// soundfont 파일을 로드합니다.
    fluid_synth_sfload(synth.get(), "DATA/MuseScore_General.sf2", 1);
 
    auto sequencer = managed_ptr<fluid_sequencer_t>{
            new_fluid_sequencer2(0), delete_fluid_sequencer };
    fluid_seq_id_t synthSeqID = fluid_sequencer_register_fluidsynth(
            sequencer.get(), synth.get());
 
    auto renderer = managed_ptr<fluid_file_renderer_t>{
            new_fluid_file_renderer(synth.get()), delete_fluid_file_renderer };
 
// 총 128가지 악기에 대해, 38~87까지 50개 음의 소리를 생성합니다.
//    for (size_t i = 0; i < 128; ++i)
//    {
//        for (size_t j = 0; j < 50; ++j)
//        {
//// 소리의 시작점
//            size_t point = i * 50 * 2000 + j * 2000;
// 악기를 선택합니다.
    int i = 0;
    int j = 0;
    size_t point = i * 50 * 2000 + j * 2000;
    
            auto evt = managed_ptr<fluid_event_t>{ new_fluid_event(), delete_fluid_event };
            fluid_event_set_source(evt.get(), -1);
            fluid_event_set_dest(evt.get(), synthSeqID);
            fluid_event_program_change(evt.get(), 0, i);
            fluid_sequencer_send_at(sequencer.get(), evt.get(), 0 + point, true);
 
// 127의 세기로 38+j번째 음을 1000ms동안 재생합니다.
            evt = managed_ptr<fluid_event_t>{ new_fluid_event(), delete_fluid_event };
            fluid_event_set_source(evt.get(), -1);
            fluid_event_set_dest(evt.get(), synthSeqID);
            fluid_event_note(evt.get(), 0, 38 + j, 127, 1000);
            fluid_sequencer_send_at(sequencer.get(), evt.get(), 1 + point, true);
 
// 음 재생이 끝난 뒤에도 잔향(ASDR에서 release 부분)이 있을수 있으니
// 1999ms 후에는 모든 소리를 꺼줍니다.
            evt = managed_ptr<fluid_event_t>{ new_fluid_event(), delete_fluid_event };
            fluid_event_set_source(evt.get(), -1);
            fluid_event_set_dest(evt.get(), synthSeqID);
            fluid_event_all_sounds_off(evt.get(), 0);
            fluid_sequencer_send_at(sequencer.get(), evt.get(), 1999 + point, true);
//        }
//    }
 
//// 타악기 소리 생성부분
//    for (size_t j = 0; j < 46; ++j)
//    {
//// 소리의 시작점
//        size_t point = 128 * 50 * 2000 + j * 2000;
//// 9번채널은 타악기 전용.
//// 127의 세기로 1000ms 동안 34 + j 타악기 음을 재생
//        auto evt = managed_ptr<fluid_event_t>{ new_fluid_event(), delete_fluid_event };
//        fluid_event_set_source(evt.get(), -1);
//        fluid_event_set_dest(evt.get(), synthSeqID);
//        fluid_event_note(evt.get(), 9, j + 34, 127, 1000);
//        fluid_sequencer_send_at(sequencer.get(), evt.get(), 1 + point, true);
//
//// 마찬가지로 1999ms 후에 모든 소리 꺼주기
//        evt = managed_ptr<fluid_event_t>{ new_fluid_event(), delete_fluid_event };
//        fluid_event_set_source(evt.get(), -1);
//        fluid_event_set_dest(evt.get(), synthSeqID);
//        fluid_event_all_sounds_off(evt.get(), 9);
//        fluid_sequencer_send_at(sequencer.get(), evt.get(), 1999 + point, true);
//    }
 
// 전체 시퀀서를 돌면서 실제 소리를 생성합니다.
    while (fluid_file_renderer_process_block(renderer.get()) == FLUID_OK)
    {
        if (fluid_sequencer_get_tick(sequencer.get()) > 2000 * 46) break;
        std::cout << "while\n";
    }
    return 0;
}
*/
