#pragma once
#include "Game.h"

class Bird
{
public:
	int birddot[2][5][5]; //the shape of the bird
	double vx,vy; //speed on two axis
	double x,y;   //x,y location
	double g;     //gravity, indicate the acceleration of the falling speed 
	double upa;   //the speed when press the space key
	int birdstate;
public:
	void jump();  //when pressing the space key, it will change y-axis speed "vy" to "upa"
	void move();  //flash the bird's position
	void drawBD(); //draw the bird
	void changeBdSt(); //change the bird's posture
public:
	Bird(void);
	virtual ~Bird(void);
};

