all:Q1.out Q2.out Q3.out

Q1.out: Q1.c
	gcc Q1.c -o Q1.out -Wall -Werror -pthread

Q2.out: Q2.c
	gcc Q2.c -o Q2.out -Wall -Werror -pthread

Q3.out: Q3.c
	gcc Q3.c -o Q3.out -Wall -Werror -pthread

clean:
	rm -rfv *.out *.o
