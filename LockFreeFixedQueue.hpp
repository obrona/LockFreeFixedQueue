#include <atomic>
#include <vector>
#include <semaphore>
#include <iostream>
using namespace std;

template <typename T>
struct LockFreeFixedQueue {
    const int len;
    atomic<int> readers, writers;
    atomic<unsigned int> begin, end;
    vector<T> store;

    LockFreeFixedQueue(int pow_2): len{1 << pow_2}, readers{0}, writers{len}, begin{0}, end{0}, store(len, T()) {}

    void push(T obj) {
        // a spinlock sucks, but if we use wait, the notification can be lost.
        // also problem with wait is that 2 threads can read the same value.
        while (writers.fetch_sub(1) <= 0) writers++;

        int idx = begin.fetch_add(1) % len;
        store[idx] = obj;

        readers++;
    }

    T pop() {
        T obj;

        while (readers.fetch_sub(1) <= 0) readers++;

        int idx = end.fetch_add(1) % len;
        obj = store[idx];

        writers++;

        return obj;
    }
};