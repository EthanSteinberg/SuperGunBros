#include "soundthread.h"

#include <portaudio.h>
#include <sndfile.h>

#include <iostream>
#include <map>

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

        struct PlayData {
            SNDFILE* file;
            bool looping;
        };

        std::map<int, PlayData> playing_sounds;

        const int FRAMES_PER_LOOP = 4000;
        while (true) {
            // First check our queue.
            spsc_queue.consume_all([&playing_sounds](const Message& item) {
                if (item.filename == nullptr) {
                    if (playing_sounds.count(item.id) != 0) {
                        sf_close(playing_sounds[item.id].file);
                        playing_sounds.erase(item.id);
                    }
                } else {
                    SF_INFO info;

                    PlayData data;
                    data.looping = item.looping;
                    data.file = sf_open(item.filename, SFM_READ, &info);

                    playing_sounds[item.id] = data;
                }
            });

            // Now process our next sound write.
            int16_t buffer[FRAMES_PER_LOOP] = {};

            std::map<int, PlayData> next_playing_sounds;
            for (auto& pair: playing_sounds) {
                const PlayData& data = pair.second;
                int16_t current_buffer[FRAMES_PER_LOOP];
                int count = sf_read_short(data.file, current_buffer, FRAMES_PER_LOOP);
                for (int i = 0 ; i < count; i++) {
                    buffer[i] += current_buffer[i];
                }

                if (count == FRAMES_PER_LOOP) {
                    next_playing_sounds[pair.first] = data;
                } else {
                    if (data.looping) {
                        sf_seek(data.file, 0, SEEK_SET);

                        int count2 = sf_read_short(data.file, current_buffer, FRAMES_PER_LOOP - count);
                        if (FRAMES_PER_LOOP - count != count2) {
                            std::cout<<"We have a problem where the clip is shorter than FRAMES_PER_LOOP? Talk to Ethan."<<std::endl;
                            exit(-1);
                        }
                        for (int i = 0 ; i < count2; i++) {
                            buffer[count + i] += current_buffer[i];
                        }
                        next_playing_sounds[pair.first] = data;
                    } else {
                        sf_close(data.file);
                    }

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

uint64_t SoundThread::play_sound(const char* filename, bool looping) {
    uint64_t id = next_sound_id++;

    if (filename == nullptr) {
        std::cout<<"Trying to play null sound?"<<std::endl;
        exit(-1);
    }

    Message m;
    m.id = id;
    m.looping = looping;
    m.filename = filename;
    spsc_queue.push(m);

    return id;
}

void SoundThread::stop_sound(uint64_t id) {
    Message m;
    m.id = id;
    m.filename = nullptr;
    spsc_queue.push(m);
}