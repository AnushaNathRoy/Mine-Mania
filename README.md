# Mine Mania

Welcome to the Mines of Moria! Collect your riches miners but beaware of the enemies lurking in the dark. Good Luck on your voyage!

## Demo
![demo](https://github.com/AnushaNathRoy/Mine-Mania/blob/main/demo/game_demo.gif)

## Features of the Game

### Room
- The world you play in consists of 3 mine caves. There is a door at the edge of each cabe which leads to the next cave. The final cave can contain a special door which upon reaching finishes the game.
- This is a top-down game with fixed frame dungeon mine caves.
- The room contains randomly generated walls which you cannot pass through.
- Each new level is harder than the previous. Pass all the 3 levels to win the game and escape the mines.

### Player
- You can move up, down, left and right. 
- You cannot pass the walls.

### Riches 
- There are 3 types of riches 
- Bronze : 5 points
- Gold : 10 points
- Diamonds : 15 points

### Enemies
- Your enemies are the bombs roaming around the cave.
- Escape them as on one touch of the bomb the game will end.
- Beaware the bombs can pass walls.


## Installation
Install these libraries (instructions for ubuntu):

```
sudo apt-get update
sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev
```

Instructions to test:

```
mkdir build
cd build
cmake ..
make -j
```

If build is aldready present remove the dirctory and run the above commands.

Play the game using

```
cd build
./app
```

