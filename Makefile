CC = gcc
RC = windres # Resource compiler
CFLAGS = -Wall -Wextra -Iinclude
SRC_DIR = src
OBJ_DIR = obj
BIN = QuizWhiz.exe
PIN_FILE = pin.bin
ICON = myicon.ico
RES_FILE = resources.res
RC_FILE = resources.rc

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

.PHONY: all clean run

all: $(BIN)

$(BIN): $(OBJS) $(RES_FILE)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(RES_FILE): $(RC_FILE)
	$(RC) $< -O coff -o $@

clean:
	@if exist $(BIN) del /Q /F $(BIN)
	@if exist $(OBJ_DIR) rmdir /S /Q $(OBJ_DIR)
	@if exist student_data.txt del /Q /F student_data.txt
	@if exist student_progress.dat del /Q /F student_progress.dat
	@if exist $(PIN_FILE) del /Q /F $(PIN_FILE)
	@if exist $(RES_FILE) del /Q /F $(RES_FILE)

run: $(BIN)
	./$(BIN)
