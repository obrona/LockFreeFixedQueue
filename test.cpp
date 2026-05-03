#include "LockFreeFixedQueue.hpp"
#include <iostream>
#include <print>
#include <thread>
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
    LockFreeFixedQueue<int> lfq(10);
    long long s1 = 0, s2 = 0;
    int N = 1e3;

    {
        jthread p1([&lfq, N] () {
            for (int i = 0; i < N; i++) lfq.push(i);
        });

        jthread p2([&lfq, N] () {
            for (int i = 0; i < N; i++) lfq.push(i);
        });

        jthread c1([&lfq, &s1, N] () {
            for (int i = 0; i < N; i++) s1 += lfq.pop();
            //println("{}", s1);
        });

        jthread c2([&lfq, &s2, N] () {
            for (int i = 0; i < N; i++) s2 += lfq.pop();
            //println("{}", s2); 
        });
    }

    println("{}", s1 + s2);
}