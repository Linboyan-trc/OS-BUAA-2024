mkdir test
cp -r ./code test/
cat ./code/14.c

cd test/code

i=0
while (($i < 16 ))
do
	gcc -o $i.o $i.c
	let i=i+1
done

cd ..
gcc -o hello code/*.o

chmod +x hello
./hello 2> err.txt

mv err.txt ../

cd ..
chmod 655 err.txt
