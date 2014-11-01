output: symboltable.o hash.o lexer.o AST.o hash2.o parser.o assembler.o driver.o
	gcc -g -lm -o compiler symboltable.o hash.o lexer.o AST.o hash2.o parser.o assembler.o driver.o
symboltable.o: symboltable.c
	gcc -c -g symboltable.c
hash.o : hash.c
	gcc -c -g hash.c
lexer.o : lexer.c
	gcc -c -g lexer.c
AST.o : AST.c
	gcc -c -g AST.c
hash2.o : hash2.c
	gcc -c -g hash2.c
parser.o : parser.c
	gcc -c -g parser.c
assembler.o : assembler.c
	gcc -c -g assembler.c
driver.o : driver.c
	gcc -c -g driver.c
clean:
	rm *.o
	rm compiler
