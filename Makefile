all:
	gcc -o hello hello.c
	
run:
	./hello

clean:
	rm -f hello
