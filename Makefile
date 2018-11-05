brute: brute_exec
	./brute_exec

brute_exec: brute.c
	gcc -Wall -o brute_exec brute.c

clean:
	rm -f brute_exec