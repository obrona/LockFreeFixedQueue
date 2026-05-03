#include <atomic>
#include <vector>
#include <semaphore>
#include <iostream>
using namespace std;

template <typename T>
struct LockFreeFixedQueue {
    const int len;
    counting_semaphore<999999999> filled, empty;
    atomic<int> begin, end;
    vector<T> store;

    LockFreeFixedQueue(int len): len{len}, filled{0}, empty{len}, begin{0}, end{0}, store(len, T()) {}

    int next(int i) {
        return (i + 1) % len;
    }

    void push(T obj) {
        empty.acquire();
        int i = begin.load();
        while (!begin.compare_exchange_strong(i, next(i)));
        store[i] = obj; 
        filled.release();
    }

    T pop() {
        T out;
        filled.acquire();
        int i = end.load();
        while (!end.compare_exchange_strong(i, next(i)));
        out = store[i];
        empty.release();
        return out;
    }
};