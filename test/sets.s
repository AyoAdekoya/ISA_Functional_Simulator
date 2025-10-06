# Thanks Alexei Korolev
li t1, 0x1
slli t1, t1, 63    # t1 should be 0x8000000000
mv s0, t1
slt s2, t1, zero   # should be 0x1
sltu s3, t1, zero  # should be 0x0
slti s4, t1, 0x0   # should be 0x1
sltiu s5, t1, 0x0  # should be 0x0

andi t1, t1, 0x0   # set t1 to zero
li t2, 1
sub t1, t1, t2     # 0xFFFFFFFFFFFFFFFF

slt a0, t1, zero   # should be 0x1
sltu a1, t1, zero  # should be 0x0
slti a2, t1, 0x0   # should be 0x1
sltiu a3, t1, 0x0  # should be 0x0

.word 0xfeedfeed