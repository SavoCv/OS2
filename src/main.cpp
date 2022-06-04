//
// Created by os on 6/3/22.
//
#include "../h/riscv.hpp"
#include "../h/tcb.hpp"

void userMain();

int main()
{
    TCB::init();
    Riscv::init();
    userMain();

    return 0;
}