.PHONY: clean case_add case_sub case_mul case_div case_all out

out: case_all
	./calc.c  < case_all > out

case_add:
	./casegen.c add 100 > case_add

case_sub:
	./casegen.c sub 100 > case_sub

case_mul:
	./casegen.c mul 100 > case_mul

case_div:
	./casegen.c div 100 > case_div

case_all: case_add case_sub case_mul case_div
	cat case_add case_sub case_mul case_div > case_all

clean:
	rm -f out calc.c casegen.c case_* *.o
