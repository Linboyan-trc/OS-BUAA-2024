out: fibo.o main.o
    gcc -o $@ $^

fibo.o: code/fibo.c ../include/fibo.h
    gcc -c -o $@ $< -I../include

main.o: code/main.c
    gcc -c -o $@ $<

clean:
    rm -f fibo.o main.o out
