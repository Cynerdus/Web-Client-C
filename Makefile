CC = gcc -lm
CFLAGS = -Wall -g -lm

.PHONY: clean run_client

build: client

client:	client.c helpers.c macros.c commands.c requests.c buffer.c parson.c
	$(CC) -o $@ $^ $(CCFLAGS)

run:
	./client

clean:
	rm -f client