_start:
    li sp, 0x200
    li a0, 0x1234
    sh a0, 0(sp)
    lh a1, 0(sp)
    .word 0xfeedfeed
