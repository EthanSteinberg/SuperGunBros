#include "soundthread.h"

#include <portaudio.h>
#include <sndfile.h>

#include <iostream>
#include <vector>

inline void pa_check(PaError error) {
    if (error != paNoError) {
        std::cout<<"audio error: " << Pa_GetErrorText(error)<<std::endl;
        exit(-1);
    }
}

void SoundThread::start() {
    internal_thread = std::move(std::thread([&]() {
        pa_check(Pa_Initialize());
        PaStream* stream;
        pa_check(Pa_OpenDefaultStream(
                  &stream,
                  0,
                  1,
                  paInt16,
                  44100,
                  paFramesPerBufferUnspecified,
                  nullptr,
                  nullptr));
        pa_check(Pa_StartStream(stream));

        std::vector<SNDFILE*> playing_sounds;

        const int FRAMES_PER_LOOP = 4000;
        while (true) {
            // First check our queue.
            spsc_queue.consume_all([&playing_sounds](const char* item) {
                SF_INFO info;
                playing_sounds.push_back(sf_open(item, SFM_READ, &info));
            });

            // Now process our next sound write.
            int16_t buffer[FRAMES_PER_LOOP] = {};

            std::vector<SNDFILE*> next_playing_sounds;
            for (SNDFILE* sound: playing_sounds) {
                int16_t current_buffer[FRAMES_PER_LOOP];
                int count = sf_read_short(sound, current_buffer, FRAMES_PER_LOOP);
                for (int i = 0 ; i < count; i++) {
                    buffer[i] += current_buffer[i];
                }

                if (count != 0) {
                    next_playing_sounds.push_back(sound);
                } else {
                    sf_close(sound);
                }
            }

            playing_sounds = next_playing_sounds;

            pa_check(Pa_WriteStream(stream, buffer, FRAMES_PER_LOOP));
        }

        // Cleanup, not used for now

        pa_check(Pa_StopStream(stream));
        pa_check(Pa_CloseStream(stream));
        Pa_Terminate();
    }));


}

void SoundThread::play_sound(const char* filename) {
    spsc_queue.push(filename);
}