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
    LockFreeFixedQueue<int> lfq(5);
    int s1 = 0, s2 = 0;

    {
        jthread p1([&lfq] () {
            for (int i = 0; i < 1000; i++) lfq.push(i);
        });

        jthread p2([&lfq] () {
            for (int i = 0; i < 1000; i++) lfq.push(i);
        });

        jthread c1([&lfq, &s1] () {
            for (int i = 0; i < 1000; i++) s1 += lfq.pop();
            println("{}", s1);
        });

        jthread c2([&lfq, &s2] () {
            for (int i = 0; i < 1000; i++) s2 += lfq.pop();
            println("{}", s2); 
        });
    }

    println("{}", s1 + s2);
}