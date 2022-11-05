	lw	0	1	base
	lw	0	2	base2
	lw	0	3	neg     // data hazard. Register 3 in line 5 needs to be forwarded.
	lw	0	4	nine
loop	add 2	3	2
	beq	3	4	nxt2    // branch hazard.
	beq	1	2	nxt     // branch hazard.
	beq	0	0	loop    // branch hazard.
nxt	jalr	2	3
nxt2	sw	0	2	1
done	halt
base	.fill	5
base2	.fill	10
neg	.fill -1
nine	.fill	9
