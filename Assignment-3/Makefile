all:Q1.out Q2.out

Q1.out: Q1.c
	gcc Q1.c -o Q1.out -Wall -Werror -pedantic -pthread

Q2.out: Q2.c
	gcc Q2.c -o Q2.out -Wall -Werror -pedantic -pthread

clean:
	rm -rfv *.out *.o