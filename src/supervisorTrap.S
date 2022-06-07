.extern _ZN5Riscv20handleSupervisorTrapEv
.extern _ZN5Riscv9sys_stackE
.extern _ZN5Riscv10user_stackE

.align 4
.global _ZN5Riscv14supervisorTrapEv
.type _ZN5Riscv14supervisorTrapEv, @function
_ZN5Riscv14supervisorTrapEv:
    # push all registers to user stack and switch to system stack

    addi sp, sp, -256
    .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr

    ld s3, _ZN5Riscv10user_stackE
    sd sp, 0(s3)

    ld s3, _ZN5Riscv9sys_stackE
    ld sp, 0(s3)

    call _ZN5Riscv20handleSupervisorTrapEv

    ld s3, _ZN5Riscv10user_stackE
    ld sp, 0(s3)

    # pop all registers from stack
    .irp index, 0,1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(sp)
    .endr
    addi sp, sp, 256

    sret
