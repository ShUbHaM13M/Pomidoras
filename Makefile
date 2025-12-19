CC = gcc
# -Wno-int-conversion
CFLAGS = -std=c11 -g -ggdb -Iinclude -I$(RAYLIB_INCLUDE)

SRCS = ./src/main.c

LDFLAGS = -L$(RAYLIB_LIB) -lraylib -lm -lpthread -ldl -lrt -lX11

RAYLIB_DIR = raylib
RAYLIB_INCLUDE = $(RAYLIB_DIR)/src
RAYLIB_LIB = $(RAYLIB_DIR)/raylib

OBJS = $(SRCS:.c=.o)
TARGET = pomidoras

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

fetch_raylib:
	git clone --depth 1 --branch 5.5 https://github.com/raysan5/raylib.git $(RAYLIB_DIR)

build_raylib:
	cd $(RAYLIB_DIR) && make

build_windows:
	x86_64-w64-mingw32-gcc -O2 -std=c11 -I./raylib-mingw/include/ src/main.c ./raylib-mingw/lib/libraylib.a -lopengl32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -luuid -lws2_32 -static -static-libgcc -static-libstdc++ -o pomidoras.exe

.PHONY: all clean fetch_raylib
