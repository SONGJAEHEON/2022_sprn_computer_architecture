	lw	0	1	base
	lw	0	2	base2
	lw	0	3	neg
	lw	0	4	nine
loop	add 2	3	2
	beq	3	4	nxt2
	beq	1	2	nxt
	beq	0	0	loop
nxt	jalr	2	3
nxt2	sw	0	2	1
done	halt
base	.fill	5
base2	.fill	10
neg	.fill -1
nine	.fill	9
