# The c compiler
CC = gcc

# The filename
FILENAME = minishell

# The build target executable:
BINARY = minishell

all: $(BINARY)

$(BINARY): $(BINARY).c
	$(CC)  -o $(BINARY) $(FILENAME).c

clean: 
	rm $(BINARY)
