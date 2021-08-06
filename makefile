all: build

build:
	gcc socket_helper.h socket_helper.c ssl_helper.h ssl_helper.c server.h server.c main.c -o server -L/usr/local/ssl/lib -lssl -lcrypto

clean:
	-rm server
	-rm *.gch
	clear