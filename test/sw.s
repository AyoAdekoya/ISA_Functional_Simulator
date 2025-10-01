_start:
    li sp, 0x200
    li a0, 0xdeadbeef
    sw a0, 0(sp)
    lw a1, 0(sp)
    .word 0xfeedfeed
