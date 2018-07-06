#define AREASTARTX 200 //game area offset pixels on x-axis
#define AREASTARTY 0
#define AREAWIDTH 10   //game area width by grid
#define AREAHEITH 15
#define UNITWIDTH 20	//grid width by pixels

#define BRICKSTARTX 40	//offset pixels on x-axis of area displaying the next brick 
#define BRICKSTARTY 90	
#define BRICKWIDTH 4    //grid width of next brick display area
#define BRICKHEITH 4
#define BKUWIDTH 15

class Brick{
public:
	//mark whether a grid of the game area has been filled by a brick
	int gameArea[AREAHEITH][AREAWIDTH];

	int score;//score of the game now
	int speed;//speed of the game, it increases when levels up

	struct BK{
		int unit[4][2];
		int minX,maxX;
		int minY, maxY;
		int locaX, locaY;
	}activeBK,nextBK; //the current brick in the window, and the next waited one

	int nextType; //the type of the next brick
	int nextDirect; //the coming direction of the next brick

	Brick();
	virtual ~Brick();

	//move the brick to one of the three direction, except upper
	bool moveBK(int direct);
	//rotate the brick anticlockwise by the degree of "deg" multiple by 90, 
	//when parameter "ifJudge" is true, it will call judgeBK() to judge the brick position
	bool rotateBK(int deg,int ifJudge);
	//generate the first random brick
	void firstrandBK();
	//generate a next random brick
	void randBK();
	//judge whether the brick has go out of the boundary, or hit the other bricks
	//returns true if there is no problem
	bool judgeBK();
	//if the brick has move to the end, just update the game area, which set the grid filled by the brick to 1
	void updateArea();
	//draw the active brick
	void drawBK();
	//draw the next brick
	void drawNextBk();
	//draw the game area
	void drawArea();
	//count how many lines have been filled by bricks
	bool judgeScore();
	//remove the line full of bricks
	void removeRow(int row);
	//return true is game over
	bool gameOver();
	//fill nextBK struct by the type of it
	void formNext();
private:
	
};