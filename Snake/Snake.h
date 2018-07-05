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
	int score, tot, per, level; //score��ʾ���յĵ÷֣� tot��ʾ�ܹ��Ե���Ʒ�� per��ʾ����ÿ����Ʒ�ķ���
	int direct;			//����0,2 UP/DOWN 3,4 LEFT/RIGHT,˳ʱ�뷽��
	int speed;
	int nGameState;

	SnakeFood food;
	
	Snake();			//��ʼ��
	virtual ~Snake();

	void MoveSnake();	//ǰ��һ��
	void DrawSnake();	//�����ߵ���״
	void UnDrawSnake();	//���»����ν��и���
	void DrawFood();	//��ʳ��

	void TurnSnake();	//��ͷ��β����
	bool RandomFood();	//�������һ��ʳ����ɹ��򷵻�TRUE��������ռ�����������򷵻�FALSE
	void EatFood();		//��ǰ����ɳԣ��������Ӧ�Ĳ������ۼƵȷ֣��������ʳ����󳤶�
	bool GameOver();	//��ײ���� ���� ͨ�أ��������������ܰ�~~
};

#endif // !defined(AFX_SNAKE_H__BE95910C_AE72_4EB8_BD5A_3BED7834A623__INCLUDED_)
