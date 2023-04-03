BIN_NAME=mandelbrot
FLAGS=-O2
LD_FLAGS=-lglfw3 -lGL -lm -static-libgcc
INCLUDE=-Iglad/include

output: mandelbrot.c 
	cc mandelbrot.c glad/src/glad.c -o $(BIN_NAME) $(FLAGS) $(LD_FLAGS) $(INCLUDE) 

clean:
	rm -rf $(BIN_NAME)

run:
	./$(BIN_NAME)
