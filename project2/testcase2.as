	lw	0	1	base 
	lw	1	2	17
	lw	0	3	base2
	lw	0	4	neg2
	lw	0	5	neg1
loop	noop
	add	2	4	2
	lw	0	3	base2   // data hazard. Load Word's saving happens in the MEM stage. So, register 3 used in instruction line 10 needs to be forwarded.
loop2	noop
	add 3	5	3   // data hazard. The calculation result is stored at register 3. But it is used to check branch condition right after the instruction.
	beq	0	3	contin  // branch hazard. The following instruction can be discarded according to the comparison result.
	beq	0	0	loop2   // branch hazard. The following instruction can be discarded according to the comparison result.
contin	beq	0	2	done    // branch hazard. The following instruction can be discarded according to the comparison result.
	beq	0	0	loop    // branch hazard. The following instruction can be discarded according to the comparison result.
done	halt
base	.fill 3
base2	.fill 2
neg2	.fill -2
neg1	.fill -1
dest	.fill 2
eleven	.fill 10
