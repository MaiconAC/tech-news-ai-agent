CC = gcc
CFLAGS = -Wall -Wextra -g -lcurl

TARGET = -o agent.bin main.c

agent.bin:
	$(CC) $(CFLAGS) $(TARGET)
	
clean:
	rm -rf agent.bin


