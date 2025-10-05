_start:
    li sp, 0x0
    li a0, 0x12345678
    sh a0, 0x50(sp)
    lw a1, 0(sp)
    .word 0xfeedfeed
