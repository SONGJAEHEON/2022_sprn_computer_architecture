	lw	0	1	one
	lw	0	5	base
	lw	0	6	offset
	lw	0	7	base2
	add	5	6	5
	sw	0	5	2
	nor	5	7	2
	beq	0	2	done
	noop
	noop
done	halt
one	.fill	1
base	.fill	13
offset	.fill	2
base2	.fill	15
