# imm values between -2048 and 2047
_start:
    li a0, 0xfffe 
    slti a1, a0, 2047 # gets sign extended to 64 before compared
    .word 0xfeedfeed
