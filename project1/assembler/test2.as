	lw	0	1	base
	lw	1	2	17
	lw	0	3	base2
	lw	0	4	neg2
	lw	0	5	neg1
loop	noop
	add	2	4	2
	lw	0	3	base2
loop2	noop
	add 3	5	3
	beq	0	3	contin
	beq	0	0	loop2
contin	beq	0	2	done
	beq	0	0	loop
done	halt
base	.fill 3
base2	.fill 2
neg2	.fill -2
neg1	.fill -1
dest	.fill 2
eleven	.fill 10
