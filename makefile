#COMPILER_FLAGS specifies the additional compilation options we're using
COMPILER_FLAGS = -Wall
LINKER_FLAGS = -lSDL2
TARGET = gameoflife
SOURCE_FILES = main.c

compile-run : gameoflife
	make all
	./${TARGET}
	make clean

all :  main.c
	gcc ${SOURCE_FILES} $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(TARGET)
	chmod +x gameoflife

clean:
	rm -rf .o