# Invaders
Invaders classic arcade game

All image files are used in the game.

Tasks for game:
- Create the defender at the bottom of the screen and allow it to be moved via key presses
(arrows left and right). It may be a simple block or loaded from an image.

- When pressing a key (space) fire a single missile that will travel from the current position of the
defender to the top of the screen where it will stop and allow another missile to be fired. There
should only ever be one missile active.

- If a missile hits an invader set that invader to be inactive and unable to be drawn or hit.

- As the invaders become inactive (i.e. destroyed by the missile) the range of the first and last
columns of the invaders to the screen borders will change. Write to / collisions detection to allow
this. (as a column becomes empty the next active column will become the edge of the invaders)

- Using a suitable set of sprite sheets use attempt to animate the invaders and missiles so it looks
like the original game.

- Invaders fire at the defender and take away lives if defender is hit, invader bullets stopped by shelters

- Shelters lose healthpoints if hit by an invader or defender bullet and parts of them crumble away

- a score is set up which increases as the player hits invaders and the mothership, displayed at the end of the game
