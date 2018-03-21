all:
	mkdir -p bin
	gcc src/lex.c -o bin/lex
clean:
	rm bin/lex
	rmdir bin
