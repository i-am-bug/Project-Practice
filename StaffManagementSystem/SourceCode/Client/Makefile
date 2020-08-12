CC := gcc

ifeq ($(DEBUGS), -g)
	DEBUG := -g
else
	DEBUG :=
endif

TARGET ?= run
SRC_DIR = ./src
INC_DIR = ./inc

OBJSC := $(wildcard $(SRC_DIR)/*.c)
OBJSC += $(wildcard *.c)
OBJSO := $(patsubst %.c, %.o, $(OBJSC))

all:$(OBJSO)
	$(CC) $(DEBUG) $^ -o $(TARGET) -lm

$(SRC_DIR)/%.o:$(SRC_DIR)/%.c
	$(CC) -c -o $@ $< -I $(INC_DIR)

%.o:%.c
	$(CC) -c -o $@ $< -I $(INC_DIR)

clean:
	rm *.o $(SRC_DIR)/*.o $(TARGET)
