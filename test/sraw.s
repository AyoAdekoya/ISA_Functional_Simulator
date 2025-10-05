_start:
    li a0, 0x7fffffff
    li a1, 0xffffffff
    li a2, 0x1
    li a3, 0x2
    sraw a4, a0, a2
    sraw a5, a0, a3
    .word 0xfeedfeed