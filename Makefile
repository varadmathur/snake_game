snake_game: snake.c
	gcc snake.c -o snake `sdl2-config --libs --cflags`
