//
// Created by os on 6/3/22.
//
#include "../h/riscv.hpp"
#include "../h/tcb.hpp"

void userMain();

int main()
{
    Riscv::init();
    userMain();

    return 0;
}