TARGET = gameoflife
SOURCE_FILES = main.c

CFLAGS=`sdl2-config --cflags`
LFLAGS=`sdl2-config --libs` -lSDL2_ttf

compile-run :
	make all
	./${TARGET}
	make clean

all :  main.c
	gcc ${SOURCE_FILES} $(CFLAGS) $(LFLAGS) -o $(TARGET)
	chmod +x $(TARGET)

clean:
	rm -rf .o
