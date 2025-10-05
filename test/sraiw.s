_start:
    li a0, 0x7fffffff
    li a1, 0xffffffff
    sraiw a2, a0, 1
    sraiw a3, a0, 2
    .word 0xfeedfeed
