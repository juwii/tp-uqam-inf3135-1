canvascii: canvascii.c
	gcc -o canvascii -Wall -Wextra -std=c11 canvascii.c
html:
	pandoc -c github-pandoc.css -s README.md -o README.html
	pandoc -s sujet.md -c github-pandoc.css -o sujet.html
clean:
	rm *.html & rm canvascii
test:
	bats check.bats
