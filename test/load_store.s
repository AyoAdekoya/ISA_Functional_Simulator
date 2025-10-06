# Thanks Lucy Wang
li t0, 0xf2345678
li t1, 0xb4

sb t0, 0(t1) # Expected: M_1B[180] = 0x78. (78 00 00 00)
sh t0, 4(t1) # Expected: M_2B[184] = 0x5678. (78 56 00 00)
sw t0, 8(t1) # Expected: M_4B[188] = 0xf2345678. (78 56 34 f2)
sd t0, 12(t1) # Expected: M_8B[192] = 0x00000000f2345678. (78 56 34 f2 00 00 00 00)            

lb t2, 0(t1) # Expected: t2 = 0x78.
lh t3, 4(t1) # Expected: t3 = 0x5678.
lw t4, 8(t1) # Expected: t4 = 0xfffffffff2345678.
ld t5, 12(t1) # Expected: t5 = 0x00000000f2345678.

li s1, -1
li s2, 0xdc

sb s1, 0(s2) # Expected: M_1B[220] = 0xff. (ff 00 00 00)
sh s1, 4(s2) # Expected: M_2B[224] = 0xffff. (ff ff 00 00)
sw s1, 8(s2) # Expected: M_4B[228] = 0xffffffff. (ff ff ff ff)

lbu s3, 0(s2) # Expected: s3 = 0xff.
lhu s4, 4(s2) # Expected: s4 = 0xffff.
lwu s5, 8(s2) # Expected: s5 = 0xffffffff.

.word 0xfeedfeed