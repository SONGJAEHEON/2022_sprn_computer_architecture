NOP	noop
	lw	0	1	base
	lw	1	2	base2
	lw	0	7	neg
	lw	0	5	nine    // data hazard. Register 5 in line 6 needs to be forwarded.
	jalr	5	4
loop	add	1	7	1
	beq	0	1	done    // branch hazard.
	beq	0	0	loop    // branch hazard.
func	add	1	2	2
	sw	1	2	9
	jalr	4	3
done	halt
base	.fill	2
base2	.fill	0
base3	.fill	10
neg	.fill	-1
nine	.fill	9
