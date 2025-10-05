_start:
    li sp, 0x50
    li a0, 0xbbbbb777777ddddd
    sd a0, 0(sp)
    lw a1, 0(sp)
    .word 0xfeedfeed
