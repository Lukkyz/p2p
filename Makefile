all:
	clang -std=c11 -Wall -L. -levent -levent_pthreads -levent_core -lssl -lcrypto utils.c hash.c message.c main.c -o main

clean:
	rm main 
