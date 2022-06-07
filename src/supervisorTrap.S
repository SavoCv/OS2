.extern _ZN5Riscv20handleSupervisorTrapEv
.extern _ZN5Riscv8sysStackE
.extern _ZN5Riscv16processorContextE

.align 4
.global _ZN5Riscv14supervisorTrapEv
.type _ZN5Riscv14supervisorTrapEv, @function
_ZN5Riscv14supervisorTrapEv:
    # push all registers to user stack and switch to system stack

    #save a0 to stack, without moving sp
    sd a0, -8(sp)
      
    #save registers except a0 (x10)
    ld a0, _ZN5Riscv16processorContextE
    .irp index, 0,1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(a0)
    .endr
    
    #save register a0
    mv s3, a0
    ld a0, -8(sp)
    sd x10, 10 * 8 (s3) #x10 is a0
    
    #changing stack
    ld sp, _ZN5Riscv8sysStackE
    
    call _ZN5Riscv20handleSupervisorTrapEv

    ld a0, _ZN5Riscv16processorContextE
    .irp index, 0,1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(a0)
    .endr
    
    ld a0, 10 * 8 (a0)

    sret
