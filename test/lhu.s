_start:
    li sp, 0x50
    li a0, 0xbbbbbaaaaaaddddd
    sh a0, 0(sp)
    lhu a1, 0(sp)
    .word 0xfeedfeed
