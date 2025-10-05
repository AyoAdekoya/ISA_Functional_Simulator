# Thank you Yusuf Abdelnur
# Test code for immediate shifts and whether the sext is correctly
# done for word instructions and arith vs logic. To see shift right 
# is being done for last two instructions, change immediate i first line
# from -1 to -256.

addiw t0, zero, -1       # $t0 = 0xffffffffffffffff (sign-extended -1)

# $t1 = 0xfffffffffffffff0 (32-bits read, shifted left 4 bits (1 byte), then sign-extend)
slliw t1, t0, 4    
    
# $t2 = 0xffffffff00000000 (64-bits read, shifted left 32, (8 bytes))
slli  t2, t0, 32         

# $t3 = 0x000000000fffffff (7 f's, 32-bit right shift, zero-extended with 0s because its logical)
srliw t3, t0, 4          
# $t4 = 0x0fffffffffffffff (64-bit right shift of 4 bits (1 byte), no sign extension because its logical)
srli  t4, t0, 4          

# $t5 = 0xffffffffffffffff (32-bit shift right of 4 bits (1 byte), because its arithmetic, sign-extends
# keeping t0 -1.)
sraiw t5, t0, 4
# $t6 = 0xffffffffffffffff (64-bit shift right of 4 bits (1 byte), stays -1 bcz arithmetic)
srai  t6, t0, 16

.word 0xfeedfeed
