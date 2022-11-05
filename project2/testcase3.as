	lw	0	1	one
	lw	0	5	base
	lw	0	6	offset  // data hazard. Load Word's saving happens in the MEM stage. So, register 6 used in instruction line 5 needs to be forwarded.
	lw	0	7	base2
	add	5	6	5
	sw	0	5	2   // data hazard.
	nor	5	7	2   // data hazard.
	beq	0	2	done
	noop
	noop
done	halt
one	.fill	1
base	.fill	13
offset	.fill	2
base2	.fill	15
