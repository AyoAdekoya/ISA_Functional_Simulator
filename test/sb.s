_start:
    li sp, 0x200
    li a0, 0x7f
    sb a0, 0(sp)
    lb a1, 0(sp)
    .word 0xfeedfeed
