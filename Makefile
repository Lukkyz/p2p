all:
	gcc -Wall -L. -levent -levent_core main.c -o main

clean:
	rm main 
