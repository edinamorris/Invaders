#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "Invader.h"
#include "string.h"
#include <stdbool.h>
#include <time.h>

// the width and height of the screen
#define WIDTH (800)
#define HEIGHT (600)

//frameRate is used to slow down the invaders and invaderBulletDelay is used as the timer for invaders to shoot bullets again
//constant variables
const int frameRate=70;
const int DefenderSpeed=4;
const int bulletSpeed=12;
const int invaderBulletSpeed=4;
const int bulletWidth=61;
const int spriteSpeed=1;
const int spaceShipWidth=40;
const int shipSpeed=3;
const int shipDelay=2000;
const int bulletNumber=5;
const int shelterNumber=4;
const int invaderBulletDelay=100;
const int shelterHealth=20;
const int shelterWidth=22;
const int shelterHeight=16;

void initializeInvaders(Invader _invaders[ROWS][COLS]);
void updateInvaders(Invader _invaders[ROWS][COLS]);

void drawInvaders(SDL_Renderer *_ren,SDL_Texture *_tex,Invader _invaders[ROWS][COLS]);
void moveDefender(SDL_Renderer *_ren, SDL_Texture *_texDefender, Defender* io_player);

void invaderHit(Bullet* io_playerBullet, Invader _invaders[ROWS][COLS]);

void playerFire(SDL_Renderer *_ren, SDL_Texture *_texBullet, Bullet *io_playerBullet, Defender* io_player, Shelter _shelter[shelterNumber]);
void invaderFire(SDL_Renderer *_ren, SDL_Texture *_texEnemyFire, Shelter _shelter[shelterNumber], Bullet _invaderBullet[bulletNumber], Bullet *io_playerBullet, Defender *io_player);

void drawSpaceShip(SDL_Renderer *_ren, SDL_Texture *_tex,Ship *io_spaceShip);

void spaceShipHit(Bullet *io_playerBullet, Ship *io_spaceShip);

void invaderReachBottom(Defender *io_player, Invader _invaders[ROWS][COLS], Bullet *io_playerBullet);

void drawShelter(SDL_Renderer *_ren, SDL_Texture *_texShelter, Shelter _shelter[shelterNumber]);

int main()
{
  Invader invaders[ROWS][COLS];
  Defender player;
  Bullet playerBullet;
  Ship spaceShip;
  Bullet invaderBullet[bulletNumber];
  Shelter shelter[shelterNumber];

  initializeInvaders(invaders);

  //Initialise playerBullet and initial invader bullet firing setting
  playerBullet.fire=0;
  playerBullet.invaderCount=0;
  playerBullet.score=0;
  playerBullet.lives=3;
  playerBullet.bulletHitPlayer=0;
  for(int i=0; i<bulletNumber; ++i)
  {
    invaderBullet[i].fire=1;
    invaderBullet[i].active=1;
  }


  // initialise SDL and check that it worked otherwise exit
  // see here for details http://wiki.libsdl.org/CategoryAPI
  if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
  {
    printf("%s\n",SDL_GetError());
    return EXIT_FAILURE;
  }
  // we are now going to create an SDL window

  SDL_Window *win = 0;
  win = SDL_CreateWindow("Invaders", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  if (win == 0)
  {
    printf("%s\n",SDL_GetError());
    return EXIT_FAILURE;
  }

  // the renderer is the core element we need to draw, each call to SDL for drawing will need the
  // renderer pointer
  SDL_Renderer *ren = 0;
  ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  // check to see if this worked
  if (ren == 0)
  {
    printf("%s\n",SDL_GetError() );
    return EXIT_FAILURE;
  }
  // this will set the background colour to white.
  // however we will overdraw all of this so only for reference
  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);

//  // SDL image is an abstraction for all images
  SDL_Surface *image;
//  // we are going to use the extension SDL_image library to load a png, documentation can be found here
//  // http://www.libsdl.org/projects/SDL_image/
  image=IMG_Load("invaders.png");
  if(!image)
  {
    printf("IMG_Load: %s\n", IMG_GetError());
    return EXIT_FAILURE;
  }
//  // SDL texture converts the image to a texture suitable for SDL rendering  / blitting
//  // once we have the texture it will be store in hardware and we don't need the image data anymore

  SDL_Texture *tex = 0;
  tex = SDL_CreateTextureFromSurface(ren, image);
  // free the image
  SDL_FreeSurface(image);

  // Initialise player
  player.move = NONE;
  player.pos.x = WIDTH/2;
  player.pos.y = HEIGHT - 30;
  player.pos.w = DEFENDERSIZE;
  player.pos.h = DEFENDERSIZE/2;

  player.srcDefender.w=188;
  player.srcDefender.h=105;
  player.srcDefender.x=0;
  player.srcDefender.y=0;

  // Initialise player bullet
  playerBullet.pos.x = (WIDTH/2)+(DEFENDERSIZE/2);
  playerBullet.pos.y = HEIGHT - 50;
  playerBullet.pos.w = 2;
  playerBullet.pos.h = 15;

  playerBullet.srcBullet.w=bulletWidth;
  playerBullet.srcBullet.h=250;
  playerBullet.srcBullet.x=0;
  playerBullet.srcBullet.y=0;

  // Initialise invader bullet
  for(int i=0; i<bulletNumber; ++i)
  {
    //a random column is chosen, it then goes through the invaders one by one starting with the bottom one
    //if the invader is active then it will shoot and break the loop
    //otherwise it will go to the next invader up until it finds the top
    //so only the bottom invader for each column is firing
    int x = 0;
    int y = rand()%(COLS-2);
    y +=2;
    for(int j=ROWS-1; j>=0; j--)
    {
      if(invaders[j][y].active==1)
      {
        invaders[j][y].shooting=1;
        x=j;
        break;
      }
    }

    //if the invader rnadomly chosen is active then the bullet is placed at it's x and y coordinates and fired
    if(invaders[x][y].shooting==1)
    {
      invaderBullet[i].pos.y=invaders[x][y].pos.y;
      invaderBullet[i].pos.x=((SPRITEWIDTH/2)-(5))+invaders[x][y].pos.x;
    }
    else if(invaders[x][y].shooting==0)
    {
      invaderBullet[i].fire=0;
    }

    invaderBullet[i].pos.w = 10;
    invaderBullet[i].pos.h = 20;

    invaderBullet[i].srcBullet.w=24;
    invaderBullet[i].srcBullet.h=56;
    invaderBullet[i].srcBullet.x=0;
    invaderBullet[i].srcBullet.y=0;
  }

  //randomise spaceship start direction
  //will end up either appearing from the right or the left
  //if its from the left the shipDelay has to be minus so the image is off screen and the opposite for the right
  srand(time(NULL));
  spaceShip.randomDir = rand()%2;
  int x;
  if (spaceShip.randomDir==0)
  {
    x=-shipDelay;
  }
  else if (spaceShip.randomDir==1)
  {
    x=shipDelay;
  }

  //initialise shelter
  for(int j=0; j<shelterNumber; j++)
  {
    shelter[j].pos.y=500;
    shelter[j].pos.x=(j*200)+50;
    shelter[j].pos.w=75;
    shelter[j].pos.h=25;

    shelter[j].srcShelter.y=0;
    shelter[j].srcShelter.w=shelterWidth;
    shelter[j].srcShelter.h=shelterHeight;
  }

  //initialise space ship
  spaceShip.pos.x=(spaceShip.randomDir*(WIDTH-spaceShipWidth)+x);
  spaceShip.pos.y=20;
  spaceShip.pos.w=spaceShipWidth;
  spaceShip.pos.h=25;

  spaceShip.srcShip.x=0;
  spaceShip.srcShip.y=204;
  spaceShip.srcShip.w=196;
  spaceShip.srcShip.h=88;

  int quit=0;
  // now we are going to loop forever, process the keys then draw

  //loading in the Defender spritesheet
  SDL_Surface *imageDefender;
  imageDefender=IMG_Load("Defender.png");
  if(!imageDefender)
  {
    printf("IMG_Load: %s\n", IMG_GetError());
    return EXIT_FAILURE;
  }
  SDL_Texture *texDefender = 0;
  texDefender = SDL_CreateTextureFromSurface(ren, imageDefender);
  SDL_FreeSurface(imageDefender);

  //loading in the enemy fire spritesheet
  SDL_Surface *imageenemyFire;
  imageenemyFire=IMG_Load("EnemyShot1.png");
  if(!imageenemyFire)
  {
    printf("IMG_Load: %s\n", IMG_GetError());
    return EXIT_FAILURE;
  }
  SDL_Texture *texEnemyFire = 0;
  texEnemyFire = SDL_CreateTextureFromSurface(ren, imageenemyFire);
  SDL_FreeSurface(imageenemyFire);

  //loading in the Shelter spritesheet
  SDL_Surface *imageShelter;
  imageShelter=IMG_Load("bunker.png");
  if(!imageShelter)
  {
    printf("IMG_Load: %s\n", IMG_GetError());
    return EXIT_FAILURE;
  }
  SDL_Texture *texShelter = 0;
  texShelter = SDL_CreateTextureFromSurface(ren, imageShelter);
  SDL_FreeSurface(imageShelter);

  //loading in the laser spritesheet
  SDL_Surface *imageBullet;
  imageBullet=IMG_Load("laser.png");
  if(!imageBullet)
  {
    printf("IMG_Load: %s\n", IMG_GetError());
    return EXIT_FAILURE;
  }
  SDL_Texture *texBullet = 0;
  texBullet = SDL_CreateTextureFromSurface(ren, imageBullet);
  SDL_FreeSurface(imageBullet);

  while (quit !=1)
  {
    SDL_Event event;
    // grab the SDL even (this will be keys etc)
    while (SDL_PollEvent(&event))
    {
      player.move=NONE;
      playerBullet.start=NONE;
      // look for the x of the window being clicked and exit
      if (event.type == SDL_QUIT)
        quit = 1;
      // check for a key down
      if (event.type == SDL_KEYDOWN)
      {
        switch (event.key.keysym.sym)
        {
          // if we have an escape quit
          case SDLK_ESCAPE :
          {
            quit=1;
            break;
          }
          //if we have a right key then the defender moves right along with the bullet before its fired
          case SDLK_RIGHT :
          case SDLK_d :
          {
            player.move=RIGHT;
            playerBullet.start=RIGHT;
            break;
          }
          //if we have a left key then the defender moves left along with the bullet before its fired
          case SDLK_LEFT :
          case SDLK_a :
          {
            player.move=LEFT;
            playerBullet.start=LEFT;
            break;
          }
          //if we have a space key then the player bullet is fired
          case SDLK_SPACE :
          {
            playerBullet.fire=1;
            break;
          }
        }
      }
    }


    // now we clear the screen (will use the clear colour set previously)
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);

    updateInvaders(invaders);
    drawInvaders(ren,tex,invaders);
    // Up until now everything was drawn behind the scenes.
    // This will show the new, red contents of the window.

    moveDefender(ren, texDefender, &player);
    playerFire(ren, texBullet, &playerBullet, &player, shelter);
    invaderFire(ren, texEnemyFire, shelter, invaderBullet, &playerBullet, &player);
    invaderHit(&playerBullet, invaders);
    drawSpaceShip(ren, tex, &spaceShip);
    spaceShipHit(&playerBullet, &spaceShip);
    invaderReachBottom(&player,invaders, &playerBullet);
    drawShelter(ren, texShelter, shelter);

    SDL_RenderPresent(ren);

    //if the player is hit or the bullet has gone offscreen, or has hit the shelter then
    //the random position for the next invader is chosen
    for(int i=0; i<bulletNumber; ++i)
    {
      if((invaderBullet[i].bulletHitPlayer==1)||
         (invaderBullet[i].bulletOffScreen==1)||
         (invaderBullet[i].bulletHitShelter==1))
      {
        //a random column is chosen, it then goes through the invaders one by one starting with the bottom one
        //if the invader is active then it will shoot and break the loop
        //otherwise it will go to the next invader up until it finds the top
        //so only the bottom invader for each column is firing
        int x = 0;
        int y = rand()%COLS;
        for(int j=ROWS-1; j>=0; j--)
        {

          invaders[j][y].shooting=0;
          if(invaders[j][y].active==1)
          {
            invaders[j][y].shooting=1;
            x=j;
            break;
          }
        }

        //only if the invader is active and is the bottom of it's column will the shooting value be set to 1
        //then the bullet is placed at the coordinates of the random invader chosen
        if(invaders[x][y].shooting==1)
        {
          invaderBullet[i].pos.y=invaders[x][y].pos.y;
          invaderBullet[i].pos.x=((SPRITEWIDTH/2)-(5))+invaders[x][y].pos.x;
          invaderBullet[i].active=1;
          invaderBullet[i].fire=1;
        }
        else if(invaders[x][y].shooting==0)
        {
          invaderBullet[i].fire=0;
        }

        invaderBullet[i].bulletOffScreen=0;
        invaderBullet[i].bulletHitPlayer=0;

      }
    }

    //when the invaders shot = the amount of invaders on screen then they redraw and
    // reset themselves at the top of the screen along with the counter
    //shelters do not reset
    if(playerBullet.invaderCount==ROWS*COLS)
    {
      initializeInvaders(invaders);
      updateInvaders(invaders);
      drawInvaders(ren,tex,invaders);
      playerBullet.invaderCount=0;
      playerBullet.lives+=1;
    }

    //if the escpae button is hit or player loses all of their lives then the game ends
    if((playerBullet.lives<=0)||(player.gameOver==1))
    {
      printf("you lose!\n");
      quit=1;
    }
  }

  printf("Your score = [%d]\n",playerBullet.score);
  printf("Your lives = [%d]\n",playerBullet.lives);

  SDL_Quit();
  return 0;
}


//function for drawing the shelters
//the amount of shelters are determined by the shelterNumber variable
//all the shelters start off as acitve, the image that is loaded from the sprite sheet depends on the counter
//which is increased if the player or invader bullets hit it
//if the bullet count is bigger than the shelterHealth then the shelters are set to not active
void drawShelter(
                 SDL_Renderer *_ren,
                 SDL_Texture *_texShelter,
                 Shelter _shelter[shelterNumber]
                )
{
  for(int j=0; j<shelterNumber; j++)
  {
    _shelter[j].active=1;
    _shelter[j].srcShelter.x=shelterWidth*((_shelter[j].counter)/12);

    if((_shelter[j].counter)/12>=shelterHealth)
    {
      _shelter[j].active=0;
    }

    if(_shelter[j].active==1)
    {
      SDL_RenderFillRect(_ren,&_shelter[j].pos);
      SDL_RenderCopy(_ren, _texShelter,&_shelter[j].srcShelter,&_shelter[j].pos);
    }
  }
}


//function for firing bullet from invader
//uses the info from the randomising invader selection to know whether to fire or not
//also checks whether the bullet has gone past the bottom of the screen and changes the variable bulletOffScreen
//checks whether there has been a collison with the player(defender) and if so changes the variable bulletHitPlayer
//also checks for a collison with any of the shelters and changes the variable bulletHitShelter
void invaderFire(
                 SDL_Renderer *_ren,
                 SDL_Texture *_texEnemyFire,
                 Shelter _shelter[shelterNumber],
                 Bullet _invaderBullet[bulletNumber],
                 Bullet* _playerBullet,
                 Defender* _player
                )
{
  SDL_Delay(20);

  for(int i=0; i<bulletNumber; i++)
  {
    _invaderBullet[i].fire=1;

    if(_invaderBullet[i].fire==1)
    {
      _invaderBullet[i].pos.y+=invaderBulletSpeed;
      _invaderBullet[i].pos.x+=0;
    }

    //test
    if(_invaderBullet[i].frame>0)
    {
      _invaderBullet[i].count=0;
    }
    if(_invaderBullet[i].frame>=20)
    {
      _invaderBullet[i].count=1;
    }

    if(_invaderBullet[i].frame>=40)
    {
      _invaderBullet[i].frame=0;
    }
    else
    {
      _invaderBullet[i].frame++;
    }

    _invaderBullet[i].srcBullet.x=_invaderBullet[i].count*24;

    //checking whether the bullet has gone off screen
    if(_invaderBullet[i].pos.y>HEIGHT)
    {
      _invaderBullet[i].timer+=1;
      _invaderBullet[i].active=0;
      if(_invaderBullet[i].timer>=invaderBulletDelay)
      {
        _invaderBullet[i].bulletOffScreen=1;
        _invaderBullet[i].timer=0;
      }
    }

    //checking collison between invader bullet and the shelters (if active)
    //increases the shelter counter to cycle through spritesheet in the shelter function
    //only changes the bulletHitShelter variable after a delay
    for(int j=0; j<shelterNumber; j++)
    {
      if(SDL_HasIntersection(&_invaderBullet[i].pos, &_shelter[j].pos)&&
                            (_shelter[j].active==1))
      {
        _invaderBullet[i].timer+=1;
        _invaderBullet[i].active=0;
        _shelter[j].counter+=1;
        if(_invaderBullet[i].timer>=invaderBulletDelay)
        {
          _invaderBullet[i].bulletHitShelter=1;
          _invaderBullet[i].timer=0;
          _shelter[j].counter+=0;
        }
      }
    }

    //checking collison between player and invaderbullet if invaderbullet is active
    //only changes the bulletHitPlayer variable after a delay
    if(SDL_HasIntersection(&_invaderBullet[i].pos, &_player->pos)&&(_invaderBullet[i].active==1))
    {
      _player->pos.x = 50;
      _player->pos.y = HEIGHT - 30;
      _playerBullet->lives-=1;
      _playerBullet->pos.x=(_player->pos.x)+(DEFENDERSIZE/2);
      _invaderBullet[i].timer+=1;
      _invaderBullet[i].active=0;
      if(_invaderBullet[i].timer>=invaderBulletDelay)
      {
        _invaderBullet[i].bulletHitPlayer=1;
        _invaderBullet[i].timer=0;
      }
    }

    //renders the bullet onto the screen only if the bullet is active
    if(_invaderBullet[i].active==1)
    {
      SDL_RenderFillRect(_ren,&_invaderBullet[i].pos);
      SDL_RenderCopy(_ren, _texEnemyFire,&_invaderBullet[i].srcBullet,&_invaderBullet[i].pos);
    }
  }
}

//function for checking if the bottom row of invaders on screen have reached the defender
//if they do then the gameOver variable is set to 1 and the game ends
void invaderReachBottom(
                        Defender *_player,
                        Invader _invaders[ROWS][COLS],
                        Bullet *_playerBullet
                       )
{
  int rowsDeleted=0;
  int temp;
  //chooses the inital bottom row first and then checks for every invader along (column number) whether there
  //are any active on the row, if there are then the loop is broken as the row is still the bottom row
  //if they're arent any active invaders on the row then the variable rowsDeleted is inceased by 1 to know
  //that the entire row has been deleted
  for(int i=ROWS-1; i>0; --i)
  {
    temp=0;
    for(int j=0; j<COLS; ++j)
    {
      if(_invaders[i][j].active==0)
      {
      }
      else
      {
        temp=1;
        break;
      }
    }
    if(temp==1)
    {
      break;
    }
    else
    {
      rowsDeleted+=1;
    }
  }

  //rowsDeleted ensures that its checking for collison with the bottom row of invaders
  if(_invaders[(ROWS-1)-rowsDeleted][0].pos.y>(HEIGHT-100))
  {
    _player->gameOver=1;
    _playerBullet->lives=0;
  }
  else
  {
    _player->gameOver=0;
  }

}

//this function checks for a collison between the playerbullet and the spaceship
//it then resets the spaceship's position on the opposite side that it appeared from
void spaceShipHit(
                  Bullet *_playerBullet,
                  Ship *_spaceShip
                 )
{
  //only if the player's bullet has been fired will it check for a collision
  //if there is one then the score is changed
  if(_playerBullet->fire==1)
  {
    if(SDL_HasIntersection(&_spaceShip->pos, &_playerBullet->pos))
    {
      _playerBullet->invaderHit=1;
      _playerBullet->score+=100;
      _spaceShip->active=0;
      if(_spaceShip->randomDir==0)
      {
        _spaceShip->pos.x=WIDTH+shipDelay;
      }
      else if(_spaceShip->randomDir==1)
      {
        _spaceShip->pos.x=0-shipDelay;
      }
    }
  }
}

//this function checks whether an invader has been hit by a playerBullet and sets it's state to deactive
void invaderHit(
                Bullet *_playerBullet,
                Invader _invaders[ROWS][COLS]
               )
{
  for(int r=0; r<ROWS; ++r)
  {
    for(int c=0; c<COLS; ++c)
    {
      //the score added is dependant on the invader type
      if(_invaders[r][c].type==TYPE3)
      {
        _invaders[r][c].scoreAmount=10;
      }
      if(_invaders[r][c].type==TYPE2)
      {
        _invaders[r][c].scoreAmount=20;
      }
      if(_invaders[r][c].type==TYPE1)
      {
        _invaders[r][c].scoreAmount=40;
      }
      //only if the bullet is fired should it check for collision and increase the invaderCount variable and modify the score
      if(_playerBullet->fire==1&&_invaders[r][c].active==1)
      {
        if(SDL_HasIntersection(&_invaders[r][c].pos, &_playerBullet->pos))
        {
          _invaders[r][c].active=0;
          _playerBullet->score+=_invaders[r][c].scoreAmount;
          _playerBullet->invaderHit=1;
          _playerBullet->invaderCount+=1;
        }
      }
    }
  }
}

//this function deals with the defender firing and where to load the bullet again onto the screen
//it also checks that the bullet has either hit an invader, spaceship, gone off screen or hit a shelter
// before the player is able to fire again
void playerFire(
                SDL_Renderer *_ren,
                SDL_Texture *_texBullet,
                Bullet *_playerBullet,
                Defender *_player,
                Shelter _shelter[shelterNumber]
               )
{

  //checking collison between player bullet (if active) and the shelters (if active)
  //increases the shelter counter to cycle through spritesheet in the shelter function
  for(int j=0; j<shelterNumber; j++)
  {
    if(SDL_HasIntersection(&_playerBullet->pos, &_shelter[j].pos)&&
                          (_playerBullet->fire==1)&&
                          (_shelter[j].active==1))
    {
      _playerBullet->shelterHit=1;
      _shelter[j].counter+=12;
    }
  }


  //changed the reached top value to true
  if (_playerBullet->pos.y<=0)
  {
    _playerBullet->bulletOffScreen=1;
  }
  //same as when bullet reaches the top of the screen
  //resets invaderHit value so space bar can be hit again and bullet fired after a shelter or invader has been hit
  if ((_playerBullet->invaderHit==1)||(_playerBullet->shelterHit==1))
  {
    _playerBullet->pos.y=HEIGHT - 50;
    _playerBullet->pos.x=(_player->pos.x)+(DEFENDERSIZE/2);
    _playerBullet->fire=0;
    _playerBullet->bulletOffScreen=0;
    _playerBullet->invaderHit=0;
    _playerBullet->shelterHit=0;
  }
  //the bullet is placed back down the bottom at the position of the Defender
  //also makes sure the bullet stays where it is until the space bar is hit again
  if (_playerBullet->bulletOffScreen==1)
  {
    _playerBullet->pos.y=HEIGHT - 50;
    _playerBullet->pos.x=(_player->pos.x)+(DEFENDERSIZE/2);
    _playerBullet->fire=0;
    _playerBullet->bulletOffScreen=0;
  }
  // if the space bar is hit then the bullet is fired upwards at a constant predetermined speed
  // the next line stops the bullet being moved by the arrow keys whilst in flight
  if (_playerBullet->fire)
  {
    _playerBullet->pos.y-=bulletSpeed;
    _playerBullet->start=NONE;
    _playerBullet->srcBullet.x=0;
    _playerBullet->srcBullet.y=0;
  }
  //stops the bullet from being shown until the bullet has been fired
  else
  {
    _playerBullet->srcBullet.x=500;
    _playerBullet->srcBullet.y=500;
  }

  //bullet follows the Defender left and right and doesnt go offscreen so when its fired its in the right space
  if ((_playerBullet->start==RIGHT)&&_playerBullet->pos.x<WIDTH-DEFENDERSIZE)
  {
    _playerBullet->pos.x+=DefenderSpeed;
  }
  else if ((_playerBullet->start==LEFT)&&_playerBullet->pos.x>0)
  {
    _playerBullet->pos.x-=DefenderSpeed;
  }
  else if (_playerBullet->start==NONE)
  {
    _playerBullet->pos.x+=0;
  }
  //renders the bullet onto the screen
  SDL_RenderFillRect(_ren,&_playerBullet->pos);
  SDL_RenderCopy(_ren, _texBullet,&_playerBullet->srcBullet,&_playerBullet->pos);
}

//function will render spaceship on the screen and depending on which side is placed each time
//will move it in a certain direction and will also update the value which will indicate when it
//has reached the edge of the screen
void drawSpaceShip(
                   SDL_Renderer *_ren,
                   SDL_Texture *_tex,
                   Ship *_spaceShip
                  )
{
  int x;
  _spaceShip->reachedSide=1;
  //if the space ship is randomly selected to start on the right hand side then
  // the code will wait until it has reached the left hand side to change the value of reached side
  // space ship is then placed on opposite side and direction changed
  if (_spaceShip->randomDir==0)
  {
    x=shipDelay;
    if(_spaceShip->pos.x>WIDTH)
    {
      _spaceShip->reachedSide=1;
      _spaceShip->pos.x=(WIDTH+x);
      _spaceShip->randomDir=1;
    }
  }
  //if the space ship is randomly selected to start on the left hand side then
  // the code will wait until it has reached the right hand side to change the value of reached side
  // space ship is then placed on opposite side and direction changed
  else if (_spaceShip->randomDir==1)
  {
    x=-shipDelay;
    if(_spaceShip->pos.x+spaceShipWidth<0)
    {
      _spaceShip->reachedSide=1;
      _spaceShip->pos.x=(0+x);
      _spaceShip->randomDir=0;
    }
  }
  //if spaceship has reached the side and is starting from the left then the ship will move towards the right
  //otherwise if spaceship has reached the side and is starting from the right then the ship will move towards the left
  if ((_spaceShip->randomDir==0)&&(_spaceShip->reachedSide==1))
  {
    _spaceShip->pos.x+=shipSpeed;
    _spaceShip->reachedSide=0;
  }
  else if ((_spaceShip->randomDir==1)&&(_spaceShip->reachedSide==1))
  {
    _spaceShip->pos.x-=shipSpeed;
    _spaceShip->reachedSide=0;
  }

  //render the spaceship on screen
  SDL_RenderFillRect(_ren,&_spaceShip->pos);
  SDL_RenderCopy(_ren, _tex,&_spaceShip->srcShip,&_spaceShip->pos);
}

//this function uses the DIRECTION enum values from the keyboard keys to move the defender in the correct direction
//or not at all when no keys are pressed
void moveDefender(
              SDL_Renderer *_ren,
              SDL_Texture *_texDefender,
              Defender *_player
             )
{
  //as long as the player is inside the bounds the direction keys will make the Defender
  // move left and right, will also stop the Defender from moving if there is no key press
  if ((_player->move==RIGHT)&&_player->pos.x<WIDTH-DEFENDERSIZE)
  {
    _player->pos.x+=DefenderSpeed;
  }
  else if ((_player->move==LEFT)&&_player->pos.x>0)
  {
    _player->pos.x-=DefenderSpeed;
  }
  else if (_player->move==NONE)
  {
    _player->pos.x+=0;
  }
  //renders the defender on screen
  SDL_RenderFillRect(_ren,&_player->pos);
  SDL_RenderCopy(_ren, _texDefender,&_player->srcDefender,&_player->pos);

}

//this function is used to initalise the invaders positions at the start of the game and sets up what
//type of invader each row is
void initializeInvaders(
                        Invader _invaders[ROWS][COLS]
                       )
{
  SDL_Rect pos;
  pos.w=SPRITEWIDTH;
  pos.h=SPRITEHEIGHT;
  int ypos=GAP;

  for(int r=0; r<ROWS; ++r)
  {
    int xpos=GAP;
    for(int c=0; c<COLS; ++c)
    {
      pos.x=xpos+SPRITEWIDTH;
      pos.y=ypos+SPRITEHEIGHT;
      xpos+=(GAP+SPRITEWIDTH);
      _invaders[r][c].pos=pos;
      _invaders[r][c].active=1;
      _invaders[r][c].frame=0;
      if(r==0)
      {
        _invaders[r][c].type=TYPE1;
      }
      else if(r==1 || r==2)
      {
        _invaders[r][c].type=TYPE2;
      }
      else
      {
        _invaders[r][c].type=TYPE3;
      }
    }
    ypos+=(GAP+SPRITEHEIGHT);
  }
}

//this function draws the invaders on screen throughout the game
//it also handles the animation speed by use of a counter
void drawInvaders(
                  SDL_Renderer *_ren,
                  SDL_Texture *_tex,
                  Invader _invaders[ROWS][COLS]
                 )
{
  for(int r=0; r<ROWS; ++r)
  {
    for(int c=0; c<COLS; ++c)
    {
      if(_invaders[r][c].active==1)
      {
        _invaders[r][c].SrcR.w=98;
        _invaders[r][c].SrcR.h=66;
        //if the frameRate is reached then reset to 0, otherwise counter +1
        if (_invaders[r][c].frame==frameRate)
        {
          _invaders[r][c].frame=0;
        }
        else
        {
          _invaders[r][c].frame++;
        }
        //handles the animation of the invaders
        //frameRate controls the animation speed
        if (_invaders[r][c].frame<(frameRate/2))
        {
          _invaders[r][c].SrcR.x=0;
        }
        else
        {
          _invaders[r][c].SrcR.x=98;
        }
      }

      //source pixel number depending on invader type
      switch(_invaders[r][c].type)
      {
      case TYPE1 : _invaders[r][c].SrcR.y=0; break;
      case TYPE2 : _invaders[r][c].SrcR.y=68; break;
      case TYPE3 : _invaders[r][c].SrcR.y=136; break;
      }
      //only if invaders are active, draw them on screen
      if(_invaders[r][c].active==1)
      {
        SDL_RenderFillRect(_ren,&_invaders[r][c].pos);
        SDL_RenderCopy(_ren, _tex,&_invaders[r][c].SrcR,&_invaders[r][c].pos);
      }

    }
  }
}

//this function updates the invaders and makes sure that when a column is deleted that the next column with
//any invaders in it moves to the edge of the screen
void updateInvaders(
                    Invader _invaders[ROWS][COLS]
                   )
{
  enum DIR{FWD,BWD};
  static int DIRECTION=FWD;
  int COLSFWD=0;
  int COLSBWD=COLS-1;
  int yinc=0;
  int temp;

  //left hand side column check
  //the column check starts at the left hand side, for each column every invader(row) is checked for an active state
  //if there is at least one invader active on the column then the loop is broken
  //otheriwse there are no invaders left on the column so the COLSFWD variable has one added to it and the edge
  //column is moved along to the next column
  for(int i=0; i<COLS; i++)
  {
    temp=0;
    for(int j=0; j<ROWS; j++)
    {
      if (_invaders[j][i].active==0)
      {
        temp=0;
      }
      else
      {
        temp=1;
        break;
      }
    }
    if(temp==1)
    {
     break;
    }
    else if(temp==0)
    {
     COLSFWD+=1;
    }
  }

  //right hand side column check
  //the column check starts at the right hand side, for each column every invader(row) is checked for an active state
  //if there is at least on invader active on the column then the loop is broken
  //otheriwse there are no invaders left on the column so the COLSBWD variable has one taken away from it and the edge
  //column is moved backward to the previous column
  for(int i=COLS-1; i>=0; i--)
  {
    temp=0;
    for(int j=0; j<ROWS; j++)
    {
      if (_invaders[j][i].active==0)
      {
        temp=0;
      }
      else
      {
        temp=1;
        break;
      }
    }
    if(temp==1)
    {
     break;
    }
    else if(temp==0)
    {
     COLSBWD-=1;
    }
  }

  //changes the DIRECTION enum to the opposite value if the invaders reach the side of the screen
  if((_invaders[0][COLSBWD].pos.x+SPRITEWIDTH)>=(WIDTH-SPRITEWIDTH))
  {
    DIRECTION=BWD;
    yinc=GAP;
  }
  else if(_invaders[0][COLSFWD].pos.x<=SPRITEWIDTH)
  {
    DIRECTION=FWD;
    yinc=GAP;
  }

  for(int r=0; r<ROWS; ++r)
  {
    for(int c=0; c<COLS; ++c)
    {
      //actually moves the invaders back the opposite way when they reach the edge
      if(DIRECTION==FWD)
      {
        _invaders[r][c].pos.x+=spriteSpeed;
      }
      else
      {
        _invaders[r][c].pos.x-=spriteSpeed;
      }
      _invaders[r][c].pos.y+=yinc;
    }
  }
}
