#ifndef SOUND_THREAD_H_INCLUDED
#define SOUND_THREAD_H_INCLUDED

#include <thread>
#include <boost/lockfree/spsc_queue.hpp>
#include <cstdint>

class SoundThread {
public:
    void start();
    uint64_t play_sound(const char* filename, bool looping = false, int scale_factor = 4);
    void stop_sound(uint64_t index);

private:
    int next_sound_id = 0;

    struct Message {
        uint64_t id;
        bool looping;
        const char* filename;
        int scale_factor;
    };

    std::thread internal_thread;
    boost::lockfree::spsc_queue<Message, boost::lockfree::capacity<1024> > spsc_queue;
};

#endif