#ifndef SOUND_THREAD_H_INCLUDED
#define SOUND_THREAD_H_INCLUDED

#include <thread>
#include <boost/lockfree/spsc_queue.hpp>

class SoundThread {
public:
    void start();
    void play_sound(const char* filename);

private:
    std::thread internal_thread;
    boost::lockfree::spsc_queue<const char*, boost::lockfree::capacity<1024> > spsc_queue;
};

#endif