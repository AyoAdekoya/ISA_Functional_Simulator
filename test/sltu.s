_start:
    li a0, 0x100000000000fffe
    li a1, 0xffff
    sltu a2, a0, a1
    .word 0xfeedfeed
