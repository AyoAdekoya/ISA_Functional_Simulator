_start:
    li t0, 5
    li t1, 11
    li t2, 2
    sb t0, 0x50(sp)
    sb t1, 0x64(sp)
    sb t2, 0x78(sp)
    .word 0xfeedfeed
