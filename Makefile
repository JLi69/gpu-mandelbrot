BIN_NAME=mandelbrot
FLAGS=-O2
LD_FLAGS=-Lglad -lglad -lglfw3 -lGL -lm

output: mandelbrot.c glad/libglad.a
	cc mandelbrot.c -o $(BIN_NAME) $(FLAGS) $(LD_FLAGS) 

glad/libglad.a:
	cd glad && cmake . && make

clean:
	rm -rf $(BIN_NAME)

run:
	./$(BIN_NAME)
