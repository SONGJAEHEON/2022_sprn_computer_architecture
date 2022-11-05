    lw 0 1 data1    // r1 = 10
    lw 0 2 data2    // r2 = 20
    lw 0 3 data3    // r3 = 30
    nor 1 2 4   // r4 = (1010 | ~10100) | (~1010 | 10100)
    add 4 1 5   // r5 = r4 + r1
    beq 3 4 END // if (r3 == r4) move to END
    add 5 1 6   // branch hazard. Because the result of the comparison of registers r3, r4 is decided later than the Instruction Fetch of line 7 instruction. According to the calculation, it can be discarded.
END halt
data1 .fill 10
data2 .fill 20
data3 .fill 30
