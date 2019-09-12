sieve:
	cc sieve3.c -o sieve -lm -lpthread

clean:
	rm -rf *.o sieve
