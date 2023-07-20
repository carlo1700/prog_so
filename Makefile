CC = gcc
CFLAGS = -Wall -Wextra -g
BUILD = build

SRCS := $(wildcard source/*.c)
OBJS := $(patsubst source/%.c, $(BUILD)/%.o, $(SRCS))
TARGETS := $(patsubst testing/%.c, $(BUILD)/%, $(wildcard testing/*.c))

all: $(TARGETS)

$(BUILD)/%.o: source/%.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%: testing/%.c $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(BUILD) $(LIB)

re: clean all

.PHONY: all clean re