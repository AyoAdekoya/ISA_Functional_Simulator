_start:
    li t0, 5
    li t1, 11
    li t2, 2
    sb t0, 32(t4)
    sb t1, 36(t5)
    sb t2, 40(t6)
    .word 0xfeedfeed
