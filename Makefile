TARGETS = chat-client chat-server

CC = gcc
CFLAGS = -Wall -Werror -O3

default: $(TARGETS)

%: %.c
	$(CC) $(CLFAGS) -o $@ $<

clean:
	rm -f $(TARGETS)