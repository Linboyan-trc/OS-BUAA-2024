mkdir test
cp -r ./code test/
cat ./code/14.c

cd test/code

gcc -c 0.c
gcc -c 1.c
gcc -c 2.c
gcc -c 3.c
gcc -c 4.c
gcc -c 5.c
gcc -c 6.c
gcc -c 7.c
gcc -c 8.c
gcc -c 9.c
gcc -c 10.c
gcc -c 11.c
gcc -c 12.c
gcc -c 13.c
gcc -c 14.c
gcc -c 15.c

cd ..
gcc -o hello code/*.o

chmod +x hello
./hello 2> err.txt

mv err.txt ../

cd ..
chmod 655 err.txt

case $# in
0)
	awk 'NR==2 {print}' err.txt >&2
;;
1)
	temp1=$(($1+1))
	awk "NR==$temp1 {print}" err.txt >&2
;;
2)
	temp2=$(($1+$2))
	awk "NR==$temp2 {print}" err.txt >&2
;;
esac
