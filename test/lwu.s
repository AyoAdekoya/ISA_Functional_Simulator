_start:
    li sp, 0x200
    li a0, 0xffffffff
    sw a0, 0(sp)
    lwu a1, 0(sp)
    .word 0xfeedfeed
