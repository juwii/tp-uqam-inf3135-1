canvascii: canvascii.c
	gcc -o canvascii -std=c11 canvascii.c

test:
	bats check.bats
