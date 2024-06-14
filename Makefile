CC = gcc
WARN = -Wall
ALLEGRO = -lallegro -lallegro_main -lallegro_ttf -lallegro_image -lallegro_color -lallegro_memfile -lallegro_font -lallegro_primitives
CCFLAGS = $(WARN) $(ALLEGRO)
TARGET = a4
SRCS = a4.c SFlib.c joystick.c
OBJS = a4.o SFlib.o joystick.o


$(TARGET): $(OBJS) 
	$(CC) $(OBJS) -o $(TARGET) $(CCFLAGS)

a4.o: a4.c SFlib.o joystick.o
	$(CC) -c a4.c -o a4.o $(CCFLAGS)

SFlib.o: SFlib.c SFlib.h 
	$(CC) -c SFlib.c -o SFlib.o $(CCFLAGS)

joystick.o: joystick.c joystick.h
	$(CC) -c $(WARN) joystick.c -o joystick.o 

clean: 
	rm -f *.o

purge: 
	rm $(TARGET)
