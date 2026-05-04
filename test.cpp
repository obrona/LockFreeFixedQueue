#include "LockFreeFixedQueue.hpp"
#include <iostream>
#include <print>
#include <thread>
#include <numeric>
using namespace std;

void push(LockFreeFixedQueue<int>& lfq) {
    for (int i = 0; i < 10; i++) {
        println("push {}", i);
        lfq.push(i);
    }
}

void pop(LockFreeFixedQueue<int>& lfq) {
    for (int i = 0; i < 10; i++) {
        println("pop {}", lfq.pop());
    }
}

int main() {
    int N = 10, M = 10000;
    LockFreeFixedQueue<int> lfq(10);
    vector<long long> store(N, 0);
    
    {
        vector<jthread> producers, consumers;
        
        for (int p = 0; p < N; p++) {
            producers.emplace_back(
                [&lfq, M] () {
                    for (int i = 0; i < M; i++) lfq.push(i);
                }
            );
        }

        for (int c = 0; c < N; c++) {
            consumers.emplace_back(
                [&lfq, &store, M, c] () {
                    for (int i = 0; i < M; i++) store[c] += lfq.pop();
                }
            );
        }

    }

    long long sum = accumulate(store.begin(), store.end(), 0, [] (int acc, int x) { return acc + x; });
    println("{}", sum);
}