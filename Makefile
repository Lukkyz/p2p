all:
	gcc -Wall -L. -levent -levent_core -lssl -lcrypto hash.c message.c main.c -o main

clean:
	rm main 
