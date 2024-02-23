.PHONY: clean case_add case_sub case_mul case_div case_all out

out: case_all
	./calc < case_all > out

case_add:
	./casegen add 100 > case_add

case_sub:
	./casegen sub 100 > case_sub

case_mul:
	./casegen mul 100 > case_mul

case_div:
	./casegen div 100 > case_div

case_all: case_add case_sub case_mul case_div
	cat case_add case_sub case_mul case_div > case_all

clean:
	rm -f out calc casegen case_* *.o
