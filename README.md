# Game of life
#### I was bored, that is why you can see yet another terminal based [Game of life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life).
Basicaly whole complexity emerges from simple rules, which you can see on the following code.
``` c
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
```
#### Life can be lived on bounded space.
![Life can be lived on bounded space.](https://github.com/maxrok98/bounded.gif)
#### Life can be lived on toroidal space.
![Life can be lived on toroidal space.](https://github.com/maxrok98/toroidal.gif)
