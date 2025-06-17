all:
	gcc -g -o server server.c -pthread
	gcc -g -o servant servant.c
	gcc -g -o client client.c -pthread

clean:
	rm -rf *.o server
	rm -rf *.o servant
	rm -rf *.o client
