#ifndef INVADER_H__
#define INVADER_H__

//for the direction of the invader
enum DIRECTION{LEFT,RIGHT,NONE};
enum InvaderType{TYPE1,TYPE2,TYPE3};

#define SPRITEWIDTH 30
#define SPRITEHEIGHT 30
#define DEFENDERSIZE 50
#define COLS 11
#define ROWS 5
#define GAP 12

//Invader sturcture which contains the variable which handle the image loading and placement along with the
//type of invader also contains variables for checking collision and altering the score
typedef struct
{
  SDL_Rect pos;
  SDL_Rect SrcR;
  int active;
  int shooting;
  int frame;
  int imageOffset;
  int checkCollisionBottom;
  int scoreAmount;
  enum InvaderType type;
}Invader;

//ship structure contains the variables which handle the image loading and inital placements
//also contains varaibles for checking whether it has reached the side and what direction to start on
typedef struct
{
  SDL_Rect pos;
  SDL_Rect srcShip;
  int reachedSide;
  int active;
  int randomDir;
}Ship;

//structure which contains the variables to deal with the inital position of the defender and the src image
//also contains the enum DIRECTION for use whether the defender is travelling in either direction or stationary
typedef struct
{
  SDL_Rect pos;
  SDL_Rect srcDefender;
  int gameOver;
  enum DIRECTION move;
  int canMove;
  int counter;
}Defender;

//sturcture which contains the variable for the src image and inital position, also for the bullet collison scenarios
//it also the ability to change the lives variable depending on whether the bullet collides with the player
//has a timer variable to delay the waves of bullets each time they have a collison/go off screen
typedef struct
{
  SDL_Rect pos;
  SDL_Rect srcBullet;
  int fire;
  int bulletOffScreen;
  int invaderHit;
  int shelterHit;
  int invaderCount;
  int score;
  int lives;
  int frame;
  int count;
  int active;
  int bulletHitShelter;
  int bulletHitPlayer;
  int timer;
  enum DIRECTION start;
}Bullet;

//structure which contains the variable for the source of the image and the position on the screen.
//it also contains the counter to know how many times an individual shelter ahs been hit, and whether
//it is still active or not
typedef struct
{
  SDL_Rect pos;
  SDL_Rect srcShelter;
  int counter;
  int active;
}Shelter;

#endif
