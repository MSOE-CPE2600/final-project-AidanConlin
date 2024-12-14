CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET_LOG = voter_log
TARGET_TERMINAL = voting_terminal
OBJ_LOG = voter_log.o
OBJ_TERMINAL = voting_terminal.o

all: $(TARGET_LOG) $(TARGET_TERMINAL)

$(TARGET_LOG): $(OBJ_LOG)
	$(CC) $(CFLAGS) -o $(TARGET_LOG) $(OBJ_LOG)

$(TARGET_TERMINAL): $(OBJ_TERMINAL)
	$(CC) $(CFLAGS) -o $(TARGET_TERMINAL) $(OBJ_TERMINAL)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_LOG) $(OBJ_TERMINAL) $(TARGET_LOG) $(TARGET_TERMINAL)
