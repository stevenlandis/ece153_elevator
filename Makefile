state:
	gcc -o state_exec state.c qepn.c qhsmElevator.c
	./state_exec

brute: brute_exec
	./brute_exec

brute_exec: brute.c
	gcc -Wall -o brute_exec brute.c

clean:
	rm -f brute_exec