all:
	gcc -Wall -L. -levent -levent_core -lssl -lcrypto message.c main.c -o main

clean:
	rm main 
