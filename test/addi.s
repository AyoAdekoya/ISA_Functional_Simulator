# addi t0, zero, 18446744073709551615
# addi t1, t0, 2047
# .word 0xfeedfeed

li   t1, 0xFFFFFFFFFFFFFFFF
li   t2, 0xFFFFFFFFFFFFFFFF
#addi t3, t0, 1
add  t3, t1, t2     

# x3 = 0x0000000000000000
.word 0xfeedfeed    