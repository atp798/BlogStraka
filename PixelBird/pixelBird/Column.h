#pragma once
#include "Game.h"

#define GAPMIN 75
#define GAPMAX 125
#define TWOCOL_DIVMAX 200 

class Column
{
public:
	Column(void);
	virtual ~Column(void);
public:
	double vx;//speed on x axis
	double x; //horizontal position
	double height; //height of the lower column
	double gapheight; //gap between upper and lower column
	double width; //width of the column
public:
	void move(); //flash the column position on x-axis
	void draw(); //draw the column
};

