.extern _ZN5Riscv16processorContextE

.align 4
.global _ZN5Riscv18returnFromInerruptEv
.type _ZN5Riscv18returnFromInerruptEv, @function
_ZN5Riscv18returnFromInerruptEv:

    ld a0, _ZN5Riscv16processorContextE
    .irp index, 0,1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(a0)
    .endr

    ld a0, 10 * 8 (a0)

    sret
