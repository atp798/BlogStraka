// Snake.h: interface for the Snake class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SNAKE_H__BE95910C_AE72_4EB8_BD5A_3BED7834A623__INCLUDED_)
#define AFX_SNAKE_H__BE95910C_AE72_4EB8_BD5A_3BED7834A623__INCLUDED_

#define MOVESTARTX 200
#define MOVESTARTY 0 
#define MOVEWIDTH 200
#define MOVEHEITH 300
#define UNITWIDTH 10

#define ID_TIMER 1

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Snake  
{
public:
	typedef struct SnakeNode
	{
		int x;
		int y;
	}SnakeNode;

	typedef struct SnakeFood
	{
		int x;
		int y;
		int special;
	}SnakeFood;

	SnakeNode data[100];
	int curr;  //
	int score, tot, per, level; //score表示最终的得分， tot表示总共吃的物品， per表示现在每个物品的分数
	int direct;			//方向0,2 UP/DOWN 3,4 LEFT/RIGHT,顺时针方向
	int speed;
	int nGameState;

	SnakeFood food;
	
	Snake();			//初始化
	virtual ~Snake();

	void MoveSnake();	//前进一步
	void DrawSnake();	//画出蛇的形状
	void UnDrawSnake();	//重新画矩形进行覆盖
	void DrawFood();	//画食物

	void TurnSnake();	//蛇头蛇尾交换
	bool RandomFood();	//随机生成一个食物，若成功则返回TRUE，否则即蛇占满整个画面则返回FALSE
	void EatFood();		//当前如果可吃，则进行相应的操作：累计等分，随机生成食物，增大长度
	bool GameOver();	//蛇撞死了 或者 通关？？：几乎不可能啊~~
};

#endif // !defined(AFX_SNAKE_H__BE95910C_AE72_4EB8_BD5A_3BED7834A623__INCLUDED_)
