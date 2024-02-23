.PHONY: clean case_add case_sub case_mul case_div case_all out

out: case_all
	gcc -o calc calc.c
	./calc < case_all > out

case_add: casegen.c
	gcc -o casegen casegen.c
	./casegen add 100 > case_add

case_sub: casegen.c
	gcc -o casegen casegen.c
	./casegen sub 100 > case_sub

case_mul: casegen.c
	gcc -o casegen casegen.c
	./casegen mul 100 > case_mul

case_div: casegen.c
	gcc -o casegen casegen.c
	./casegen div 100 > case_div

case_all: case_add case_sub case_mul case_div
	cat case_add case_sub case_mul case_div > case_all

clean:
	rm -f out calc casegen case_* *.o
