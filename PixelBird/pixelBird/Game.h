#pragma once
#include "Bird.h"
#include "Column.h"

#define CONSOLEX 800
#define CONSOLEY 400
#define FONTHEITH 15

bool ifGameover();
void movecolumn();
void drawBG();
void PutOutText(char* lpsz, int X, int Y, unsigned long fontcolor);
void printResult();