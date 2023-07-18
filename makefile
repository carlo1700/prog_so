CC = gcc
CFLAGS = -Wall -Wextra -g

# Lista dei file sorgente
SRCS = main.c FileSystemFAT.c utils.c prints.c

# Lista dei file oggetto generati dalla compilazione
OBJS = $(SRCS:.c=.o)

# Nome dell'eseguibile
TARGET = myprogram

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Regola generica per compilare i file sorgente in file oggetto
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
