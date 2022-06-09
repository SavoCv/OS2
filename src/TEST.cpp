//
// Created by os on 5/18/22.
//

void test_allocator();

void test_thread();

void test_thread_and_sleep();

namespace TA2{
    void test_allocator();
}

extern "C" void test_allocator_c_api();

void test_input_output();

enum Testing{Allocator, Allocator2, Allocator_c_api, Thread, ThreadAndSleep, IO};

const Testing t = IO;

void userMain() {
    switch (t) {
        case Allocator2: TA2::test_allocator(); break;
        case Thread: test_thread(); break;
        case Allocator: test_allocator(); break;
        case Allocator_c_api: test_allocator_c_api(); break;
        case ThreadAndSleep: test_thread_and_sleep(); break;
        case IO: test_input_output(); break;
        default: break;
    }
}