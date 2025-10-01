_start:
    li sp, 0x200
    li a0, 0xff
    sb a0, 0(sp)
    lbu a1, 0(sp)
    .word 0xfeedfeed
