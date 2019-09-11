sieve:
	cc sieve3.c -o sieve -lm -lpthread

clean:
	rm -rf *.o sieve

queen:
	mpicc queen.c -o queenpb

cleanq:
	rm -rf *.o queenpb
