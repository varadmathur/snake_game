#include <stdio.h>

#define SDL_DISABLE_IMMINTRIN_H

#include <SDL2/SDL.h>
#include <time.h>

#define W 30
#define H 20

#define SIZE 16

#define SCREEN_W ((W) * (SIZE))
#define SCREEN_H ((H) * (SIZE))


#define DELAY 80 // 80 milliseconds
#define START_LEN 3

#define MAX_SNAKE_LEN 200


#define TRUE 1
#define FALSE 0;


SDL_Window  *window = NULL;
SDL_Surface *screen = NULL;

SDL_Surface *white = NULL,
            *red   = NULL,
            *green = NULL;


struct pos_t 
{
    int x, y;
};

struct pos_t s[MAX_SNAKE_LEN],
             f;


enum Last_key {LEFT, RIGHT, UP, DOWN} key = RIGHT;


int len = START_LEN,

    over = FALSE;



int setup() 
{
    if (SDL_Init(SDL_INIT_EVERYTHING)) 
    {
	printf("Error initializing SDL: %s\n", SDL_GetError());
	return (-1);
    }

    window = SDL_CreateWindow("Snake Game!",
			      SDL_WINDOWPOS_CENTERED,
			      SDL_WINDOWPOS_CENTERED,
					  
			      SCREEN_W,
			      SCREEN_H, SDL_WINDOW_SHOWN);

    if (window == NULL) 
    {
	printf("Error initializing window: %s\n", SDL_GetError());
	return (-1);
    }


    screen = SDL_GetWindowSurface(window);

    if (screen == NULL) 
    {
	printf("Error initializing window_surface: %s\n", SDL_GetError());
	return (-1);
    }


    white = SDL_LoadBMP("./images/white.bmp");
    red   = SDL_LoadBMP("./images/red.bmp");
    green = SDL_LoadBMP("./images/green.bmp");

    if (white == NULL || red == NULL || green == NULL)
    {
	printf("Unable to load image! SDL Error: %s\n", SDL_GetError());
	return (-1);
    }

    srand(time(NULL));


    for (int i = 0; i < MAX_SNAKE_LEN; i++) {
	s[i].x = 0;
	s[i].y = 0;
    }

    f.x = W / 2;
    f.y = 0;
    
    return (0);
}

void logic() 
{
    // Move the snake
    for (int i = len; i > 0; i--) 
    {
	s[i].x = s[i-1].x;
	s[i].y = s[i-1].y;
    }


    // Move the snake's head according to the last key pressed
    switch (key)
    {
    case LEFT:  s[0].x--; break;
    case RIGHT: s[0].x++; break;
    case UP:    s[0].y--; break;
    case DOWN:	s[0].y++; break;
    };

    
    // Check if the snake has hit the walls
    if (s[0].x+1 > W || s[0].x < 0 ||
	s[0].y+1 > H || s[0].y < 0)
    {
	over = TRUE;
	return;
    }

    // Check if the snake has hit itself
    for (int i = 1; i < len; i++)
	if (s[0].x == s[i].x && s[0].y == s[i].y)
	{
	    over = TRUE;
	    return;
	}

    // Head of snake has hit fruit
    if (s[0].x == f.x && s[0].y == f.y)
    {
        // Increment the length of the snake
        len++;

        // Spawn new fruit at random location and make sure it doesn't spawn
        // on a body part of the snake

        int fruit_overlapping;
        do
        {
            f.x = rand() % W;
            f.y = rand() % H;

            fruit_overlapping = FALSE;
            for (int i = 0; i < len; i++)
                if (s[i].x == f.x && s[i].y == f.y) // Snake ith part collides with fruit
                    fruit_overlapping = TRUE;

        } while (fruit_overlapping == TRUE);
    }
}

void draw()
{
    SDL_Rect pos;

    pos.w = SIZE;
    pos.h = SIZE;

    pos.x = 0;
    pos.y = 0;


    // Clear the window's surface
    SDL_FillRect(screen, NULL, 0x000000);


    // Draw the background
    for (int i = 0; i < SCREEN_H / SIZE; i++) 
    { 
	for (int j = 0; j < SCREEN_W / SIZE; j++) 
	{
	    SDL_BlitSurface(white, NULL, screen, &pos);
	    pos.x += SIZE;
	}

	pos.x = 0;
	pos.y += SIZE;

    }

    // Draw the snake
    for (int i = 0; i < len; i++) 
    {
	pos.x = s[i].x * SIZE;
	pos.y = s[i].y * SIZE;

	SDL_BlitSurface(red, NULL, screen, &pos);
    }
    

    // Draw the fruit
    pos.x = f.x * SIZE;
    pos.y = f.y * SIZE;

    SDL_BlitSurface(green, NULL, screen, &pos);


    // Display
    SDL_UpdateWindowSurface(window);
}


void free_stuff()
{
    SDL_DestroyWindow(window); window = NULL;

    SDL_FreeSurface(white); white = NULL;
    SDL_FreeSurface(red);   red = NULL;
    SDL_FreeSurface(green); green = NULL;

    SDL_Quit();
}


int main() 
{
    if (setup()) 
    {
	printf("setup() failed. Exiting...\n");
	goto end;
    }

    Uint32 last_time = 0;
    SDL_Event e;
    
    for (;;)
    {
	if (SDL_GetTicks() - last_time >= DELAY)
        {
	    last_time = SDL_GetTicks();

            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT)
                    goto end;

                if (e.type == SDL_KEYDOWN)
                    switch (e.key.keysym.sym)
                    {
                    case SDLK_LEFT:   if (s[0].x-1 != s[1].x || s[0].y != s[1].y) key = LEFT;  break;

                    case SDLK_RIGHT:  if (s[0].x+1 != s[1].x || s[0].y != s[1].y) key = RIGHT; break;

                    case SDLK_UP:     if (s[0].x != s[1].x || s[0].y-1 != s[1].y) key = UP;    break;

                    case SDLK_DOWN:   if (s[0].x != s[1].x || s[0].y+1 != s[1].y) key = DOWN;  break;
                    }
            }

            logic();

            if (over == TRUE)
            {
		printf("Game Over!\n");
		printf("Length: %d\n", len);
		goto end;
	    }

            draw();
	}
    }

end:
    free_stuff();
    
    return (0);
}
