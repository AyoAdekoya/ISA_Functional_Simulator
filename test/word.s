_start:
    li t0, 5      # t0 = 5
    sub t1, t2, t0    # t1 = 0xfffffffffffffffb
    andi t2, t1, -10    # t2 = 0xfffffffffffffff2
    li t3, 31     # t3 = 31
    sllw t6, t2, 31    # t6 = 0x0000000000000000
    ori t3, t6, 0x01b   # $t3 = 0x000000000000001b
    addw t4, t6, t3    # t4 = 0x000000000000001b
    Subw t5, t4, t3    # t5 = 0x000000000000000

 .word 0xfeedfeed