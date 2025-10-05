_start:
    li sp, 0x50
    li a0, 0xbbbbbaaaaaaddddd
    sd a0, 0(sp)
    lwu a1, 0(sp)
    .word 0xfeedfeed