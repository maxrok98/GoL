#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ALIVE 1
#define DEAD 0

typedef char cell;

cell *space;
cell *prev_space;
int HEIGHT = 25;
int WIDTH = 50;

void random_init(){
	for(int i = 0; i < HEIGHT; i++){
		for(int j = 0; j < WIDTH; j++){
			space[(i*WIDTH)+j] = (rand() % 100 > 20) ? DEAD : ALIVE;
		}
	}
}
void glider_init(){
	space[1*WIDTH+2] = ALIVE;
	space[2*WIDTH+3] = ALIVE;
	space[3*WIDTH+1] = ALIVE;
	space[3*WIDTH+2] = ALIVE;
	space[3*WIDTH+3] = ALIVE;
}
void glider_gun(){
	int gun_width = 36, gun_height = 9;
	int gun[] = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,
		0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,
		1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		1,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	};
	for(int i = 0; i < gun_height; i++){
		for(int j = 0; j < gun_width; j++){
			space[(i+1)*WIDTH+j+1] = gun[i*gun_width+j];
		}
	}
}

void draw(){
	printf("\u250C");
	for(int i = 0; i < WIDTH*2; i++){
		printf("\u2500");
	}
	printf("\u2510\n");

	for(int i = 0; i < HEIGHT; i++){
		printf("\u2502");
		for(int j = 0; j < WIDTH; j++){
			printf("%s", (space[(i*WIDTH)+j] == ALIVE) ? "\u2588\u2588" : "  ");
		}
		printf("\u2502\n");
	}

	printf("\u2514");
	for(int i = 0; i < WIDTH*2; i++){
		printf("\u2500");
	}
	printf("\u2518\n");
}

int bounded_space(int i, int j){
	int valid_top = (i != 0), valid_bottom = (i != HEIGHT-1);
	int valid_left = (j != 0), valid_right = (j != WIDTH-1); 
	int n = 0;

	if(valid_top && valid_left) 			n += prev_space[((i-1)*WIDTH)+j-1];
	if(valid_top)								 			n += prev_space[((i-1)*WIDTH)+j];
	if(valid_top && valid_right) 			n += prev_space[((i-1)*WIDTH)+j+1];
	if(valid_left) 										n += prev_space[(i*WIDTH)+j-1];
	if(valid_right) 									n += prev_space[(i*WIDTH)+j+1];
	if(valid_bottom && valid_left) 		n += prev_space[((i+1)*WIDTH)+j-1];
	if(valid_bottom)								 	n += prev_space[((i+1)*WIDTH)+j];
	if(valid_bottom && valid_right) 	n += prev_space[((i+1)*WIDTH)+j+1];
	
	return n;
}

int toroidal_space(int i, int j){
	int top = 		(i == 0) ? HEIGHT - 1 : i-1;
	int bottom = 	(i == HEIGHT - 1) ? 0 : i+1;
	int left = 		(j == 0) ? WIDTH - 1 : j-1;
	int right = 	(j == WIDTH - 1) ? 0 : j+1;
	int n = 0;

	n += prev_space[(top*WIDTH)+left];
	n += prev_space[(top*WIDTH)+j];
	n += prev_space[(top*WIDTH)+right];
	n += prev_space[(i*WIDTH)+left];
	n += prev_space[(i*WIDTH)+right];
	n += prev_space[(bottom*WIDTH)+left];
	n += prev_space[(bottom*WIDTH)+j];
	n += prev_space[(bottom*WIDTH)+right];
	
	return n;
}

int (*get_neighbours)(int, int);

void evolve(){
	memcpy(prev_space, space, HEIGHT * WIDTH);
	memset(space, DEAD, HEIGHT * WIDTH);

	for(int i = 0; i < HEIGHT; i++){
		for(int j = 0; j < WIDTH; j++){
			int neighbours = get_neighbours(i, j);
			// Any live cell with two or tree neighbours survives
			if(prev_space[(i*WIDTH)+j] && (neighbours == 2 || neighbours == 3)){
				space[(i*WIDTH)+j] = ALIVE;
			// Any dead cell with tree neighbours become a live 
			} else if(!prev_space[(i*WIDTH)+j] && neighbours == 3) {
				space[(i*WIDTH)+j] = ALIVE;
			}
		}
	}
}

int main(){
	time_t t;
	srand((unsigned) time(&t));

	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	WIDTH = (w.ws_col-2)/2; HEIGHT = w.ws_row-3; // space for frame
	
	space = (cell*)malloc(WIDTH * HEIGHT * sizeof(cell));
	prev_space = (cell*)malloc(WIDTH * HEIGHT * sizeof(cell));

	get_neighbours = toroidal_space;

	random_init();
	//glider_init();
	//glider_gun();
	while(1){
		draw();
		evolve();
		usleep(100000);
	}
	
}
