# t5 should be 0 or 0x0 at the end of this code.
start:
    addi t0, zero, 0
    addi t1, zero, 1
    addi t2, zero, 1
    addi t5, zero, 0   
    bltu t0, t1, branch1
    addi t5, t5, 16
branch1:
    bltu t0, t2, branch2
    addi t5, t5, 8    
branch2:
    addi t0, zero, 0
    addi t1, zero, 1
    bltu t0, t1, branch3
    addi t5, t5, 4
branch3:
    addi t0, zero, 4
    addi t1, zero, -1
    bltu t0, t1, branch4
    addi t5, t5, 2
branch4:

    .word 0xfeedfeed

# Thank you Cecily Gross