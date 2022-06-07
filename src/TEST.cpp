//
// Created by os on 5/18/22.
//

void test_allocator();

void test_thread();

namespace TA2{
    void test_allocator();
}

extern "C" void test_allocator_c_api();

enum Testing{Allocator, Allocator2, Allocator_c_api, Thread};

const Testing t = Thread;

void userMain() {
    switch (t) {
        case Allocator2: TA2::test_allocator(); break;
        case Thread: test_thread(); break;
        case Allocator: test_allocator(); break;
        case Allocator_c_api: test_allocator_c_api(); break;
        default: break;
    }
}