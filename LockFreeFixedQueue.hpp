#include <atomic>
#include <vector>
#include <semaphore>
#include <iostream>
using namespace std;

// one problem is that producer B can signal that reader A can read, but producer A is not done yet.
// producer A 'came' first.
// causing reader A to read rubbish.

// same problem with producer.
// consumer B signal that producer A can proceed, but consumer A is not done yet.

// producer can come when generation is 0, 4, 8...
// so 4k means ready to produce, 4k + 1, means got the ticket.

// consumer can come when generation is 2, 6, 10...
// so 2 + 4k means ready to comsume, 2 + 4k + 1 means got the ticket.

// need the concept of generation because of looping back.
// for example, producer 1, 2 come in, producer 1 is slow.
// then consumer 1, 2 comes in, 1 is slow, 2 is done.
// cannot let consumer 1 read value before producer 1 writes it.
// producer 3 comes in, gets the same slot as producer 1. We cannot let prod 3 override the value of prod 1.

// compiling with threadSanitizer makes the execution time very long.


template <typename T>
struct LockFreeFixedQueue {
    const int len;
    counting_semaphore<999999999> filled, empty;
    atomic<int> begin, end;
    vector<T> store;
    vector<atomic<unsigned int>> generation;

    LockFreeFixedQueue(int len): 
        len{len}, 
        filled{0}, 
        empty{len}, 
        begin{0}, 
        end{0}, 
        store(len),
        generation(len)
    {}

    int next(int i) {
        return (i + 1) % len;
    }

    bool can_produce(unsigned int g) {
        return g % 4 == 0;
    }

    bool can_consume(unsigned int g) {
        return g >= 2 && (g - 2) % 4 == 0;
    }

    void push(T obj) {
        empty.acquire();
        
        int i = begin.load();
        while (!begin.compare_exchange_strong(i, next(i)));
        
        while (1) {
            unsigned int g = generation[i].load();
            if (!can_produce(g)) continue;
            bool ans = generation[i].compare_exchange_strong(g, g + 1);
            if (ans) break;
        }
        store[i] = obj; 
        generation[i].fetch_add(1);
       
        
        filled.release();
    }

    T pop() {
        T out;
        
        filled.acquire();
        
        int i = end.load();
        while (!end.compare_exchange_strong(i, next(i)));
        
        while (1) {
            unsigned int g = generation[i].load();
            if (!can_consume(g)) continue;
            bool ans = generation[i].compare_exchange_strong(g, g + 1);
            if (ans) break;
        }
        out = store[i];
        generation[i].fetch_add(1);
        
        empty.release();
        return out;
    }
};