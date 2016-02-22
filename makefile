CC := gcc -std=c99
LIBS_MAIN := main_app_controller.o utility_lib.o gameplay_lib.o
LIBS_SERVER := utility_lib.o gameplay_lib.o
CFLAGS := -c
GTK_FLAGS_PRE := `pkg-config --cflags gtk+-3.0`
GTK_FLAGS_POST := `pkg-config --libs gtk+-3.0`
#BUILD_DIR_MAIN := $(addprefix build/, $(LIBS_MAIN))
#BUILD_DIR_SERVER := $(addprefix build/, $(LIBS_SERVER))

vpath %.c src
.PHONY: clean all

all : main_app main_server

# Compile final program
main_app: main_app.o $(LIBS_MAIN)
	$(CC) $(GTK_FLAGS_PRE) $^ -o $@ $(GTK_FLAGS_POST)

# Compile final program
main_server: main_server.o $(LIBS_SERVER)
	$(CC) $(GTK_FLAGS_PRE) $^ -o $@ $(GTK_FLAGS_POST)

# Compile single object file
%.o : %.c
	$(CC) $(GTK_FLAGS_PRE) $(CFLAGS) $< -o $@ $(GTK_FLAGS_POST)

#Clean
clean:
	rm -rf *.o
	rm -rf main_app
	rm -rf main_server
