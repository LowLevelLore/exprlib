TARGET := expr
LIB    := libexpr.a

SRC_DIR   := src
INCL_DIR  := include
BUILD_DIR := build
LIB_DIR   := $(BUILD_DIR)/lib
BIN_DIR   := $(BUILD_DIR)/bin

CC := gcc
AR := ar

CFLAGS := -std=gnu23 -Wall -Wextra -Werror -pedantic -g -I$(INCL_DIR) -Wno-unused-variable
LIBS   := -lm

all: $(BIN_DIR)/$(TARGET)

$(LIB_DIR)/$(LIB): $(SRC_DIR)/exprlib.c | $(LIB_DIR)
	$(CC) $(CFLAGS) -c $< -o $(LIB_DIR)/exprlib.o
	$(AR) rcs $@ $(LIB_DIR)/exprlib.o

$(BIN_DIR)/$(TARGET): $(SRC_DIR)/main.c $(LIB_DIR)/$(LIB) | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -L$(LIB_DIR) -lexpr -o $@ $(LIBS)

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
