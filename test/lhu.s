_start:
    li sp, 0x200
    li a0, 0xffff
    sh a0, 0(sp)
    lhu a1, 0(sp)
    .word 0xfeedfeed
