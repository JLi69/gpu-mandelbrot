BIN_NAME=mandelbrot
FLAGS=-O2
LD_FLAGS=-lglfw3 -lGL -lm

output: mandelbrot.c
	cc mandelbrot.c -o $(BIN_NAME) $(FLAGS) $(LD_FLAGS) 

clean:
	rm -rf $(BIN_NAME)

run:
	./$(BIN_NAME)
