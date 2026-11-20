# =============================================================================
#  Makefile  --  CodeQuest
#
#  Project : CodeQuest - learn, practise and master C / C++ (raylib)
#  Author  : Szymon Stepniak
#
#  Targets
#    make            build the app into build/CodeQuest.exe
#    make run        build and launch it
#    make debug      build with the console attached (TRACELOG visible)
#    make shots      regenerate the screenshots in shots/ (uses demo data)
#    make verify     recompute every Section A answer (tools/verify.c)
#    make clean      delete build output
#
#  RAYLIB_DIR points at the folder the raylib Windows installer creates. The
#  installer ships its own gcc (w64devkit), which is used by default so the
#  project builds on a machine with no other compiler set up.
# =============================================================================

RAYLIB_DIR ?= C:/raylib
RAYLIB_INC ?= $(RAYLIB_DIR)/w64devkit/x86_64-w64-mingw32/include
RAYLIB_LIB ?= $(RAYLIB_DIR)/w64devkit/x86_64-w64-mingw32/lib
# make predefines CC as "cc", so ?= would never take effect here. Only
# override it when the value is still make's own default - that way an
# explicitly supplied CC=... on the command line still wins.
ifeq ($(origin CC),default)
CC = $(RAYLIB_DIR)/w64devkit/bin/gcc
endif

TARGET  = build/CodeQuest.exe
SRC     = $(wildcard src/*.c)

CFLAGS  = -std=c99 -Wall -Wextra -O2 -Isrc -I$(RAYLIB_INC)
LDFLAGS = -L$(RAYLIB_LIB) -lraylib -lopengl32 -lgdi32 -lwinmm

# -mwindows hides the console window in the shipped build. Drop it while
# debugging so raylib's TRACELOG output is visible.
RELEASE_FLAGS = -mwindows

.PHONY: all run debug shots verify clean

all: $(TARGET)

$(TARGET): $(SRC) | build
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LDFLAGS) $(RELEASE_FLAGS)

# Same build with the console attached, for reading TRACELOG messages.
debug: $(SRC) | build
	$(CC) $(SRC) -o build/CodeQuest-debug.exe $(CFLAGS) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

shots: $(SRC) | build
	$(CC) $(SRC) -o build/CodeQuest-shot.exe $(CFLAGS) $(LDFLAGS)
	./build/CodeQuest-shot.exe --shot
	@mkdir -p shots
	@mv -f *.png shots/ 2>/dev/null || true

# Recomputes every Section A answer from a real implementation of the lab
# functions. Run this after adding or editing a question in content.c.
verify: | build
	$(CC) tools/verify.c -o build/verify.exe -std=c99 -O1
	./build/verify.exe

build:
	@mkdir -p build

clean:
	@rm -rf build
	@echo cleaned
