#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <windows.h>
#include <conio.h>

#define FIELD "\
 ------------------------------ \n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
|                              |\n\
                                \n"
#define BALL 'o'

#define LEFT 'h'
#define RIGHT 'l'

#define QUIT 'q'

typedef struct
{
	unsigned char m_x;
	unsigned char m_y;
	char m_vx;
	char m_vy;
}BALL_POINT;

typedef enum
{
	BALL_OK,
	BALL_X_NG,
	BALL_Y_NG,
	BALL_XY_NG
} BALL_CHECK;

void moniTask(void);
void inptTask(void);
unsigned short moniFieldPos(unsigned char, unsigned char);
int moniBallnew(BALL_POINT*, char*);
BALL_CHECK moniBallCheck(unsigned char, unsigned char, char, char);
void moniBlockCreate(char *);

unsigned char g_ucMoniFlg;
unsigned char g_ucInptFlg;
char g_cBarCnt;

unsigned int g_uiFieldInf[31] = 
{
	0xFFFFFFFF,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x80000001,
	0x00000000,
};

int main(void)
{
	pthread_t moniThread, inptThread;
	int ret1,ret2;

	ret1 = pthread_create(&moniThread,NULL,(void *)moniTask,NULL);

	if(ret1 != 0)
	{
		printf("Can not create moniThread");
		return 0;
	}

	ret2 = pthread_create(&inptThread,NULL,(void *)inptTask,NULL);
	if(ret2 != 0)
	{
		printf("Can not create inptThread");
		return 0;
	}

	ret2 = pthread_join(inptThread,NULL);
	if(ret2!=0)
	{
		printf("Can not join inptThread");
		return 0;

	}

	ret1 = pthread_join(moniThread,NULL);
	if(ret1!=0)
	{
		printf("Can not join moniThread");
		return 0;
	}

	return 0;
}

void moniTask(void)
{
	BALL_POINT stBall = {15,29,-1,-1};
	char cFieldStr[] = FIELD;
	int iRet = 0;
	unsigned short usBallPos;
	unsigned char ucBarPos = 0;

	g_ucMoniFlg = 1;
	g_cBarCnt = 14;

	moniBlockCreate(cFieldStr);

	while(g_ucMoniFlg)
	{
		g_uiFieldInf[30] = 0;
		cFieldStr[990 + ucBarPos] = ' ';
		cFieldStr[990 + ucBarPos + 1] = ' ';
		cFieldStr[990 + ucBarPos + 2] = ' ';

		if((char)ucBarPos + g_cBarCnt < 0)
		{
			ucBarPos = (unsigned char)0;
		}
		else if((char)ucBarPos + g_cBarCnt > 29)
		{
			ucBarPos = (unsigned char)29;
		}
		else
		{
			ucBarPos += g_cBarCnt;
		}
		g_cBarCnt = 0;

		g_uiFieldInf[30] = 0x000007 << ucBarPos;
		cFieldStr[990 + ucBarPos] = '-';
		cFieldStr[990 + ucBarPos + 1] = '-';
		cFieldStr[990 + ucBarPos + 2] = '-';

		usBallPos = moniFieldPos(stBall.m_x, stBall.m_y);
		cFieldStr[usBallPos] = ' ';
		iRet = moniBallnew(&stBall, cFieldStr);
		usBallPos = moniFieldPos(stBall.m_x, stBall.m_y);
		cFieldStr[usBallPos] = BALL;

		system("cls");
		if(iRet == -1)
		{
			g_ucMoniFlg = 0;
			printf("%s",cFieldStr);
			printf("                 q:END\n");
			printf("GAME OVER\n");
		}
		else if(iRet == 1)
		{
			g_ucMoniFlg = 0;
			printf("%s",cFieldStr);
			printf("                 q:END\n");
			printf("GAME CLEAR\n");
		}
		else
		{
			printf("%s",cFieldStr);
			printf("h:LEFT  l:RIGHT  q:END\n");
			Sleep(100);
		}

	}
}

void inptTask(void)
{
	char ucInput;
	int ret;

	g_ucInptFlg = 1;

	while(g_ucInptFlg)
	{
		ucInput = getch();

		switch(ucInput)
		{
			case LEFT:
				g_cBarCnt--;
				break;
			case RIGHT:
				g_cBarCnt++;
				break;
			case QUIT:
				g_ucInptFlg = 0;
				g_ucMoniFlg = 0;
			default:
				break;
		}
	}
}
unsigned short moniFieldPos(unsigned char x, unsigned char y)
{
	return (y * 33u) + x;
}

int moniBallnew(BALL_POINT *ball, char *field)
{
	BALL_CHECK eRes;
	unsigned int uiSum = 0;
	unsigned char x = 0;
	unsigned char y = 0;
	int iRet = 0;

	eRes = moniBallCheck(ball->m_x, ball->m_y, ball->m_vx, ball->m_vy);

	x = ball->m_x + ball->m_vx;
	y = ball->m_y + ball->m_vy;

	if(eRes != BALL_OK)
	{
		if((x > 0 && x < 31)
			&& (y > 0 && y < 21))
		{
			field[moniFieldPos(x,y)] = ' ';
			g_uiFieldInf[y] &= ~(0x000001 << x);

			for(int i = 1; i < 21; i++)
			{
				uiSum |= g_uiFieldInf[i] & (~0x80000001);
			}
			
			if(uiSum == 0)
			{
				iRet = 1;
			}
		}
	}


	switch(eRes)
	{
		case BALL_OK:
			ball->m_x += ball->m_vx;
			ball->m_y += ball->m_vy;
			break;
		case  BALL_X_NG:
			ball->m_vx *= -1;
			ball->m_x += ball->m_vx;
			ball->m_y += ball->m_vy;
			break;
		case  BALL_Y_NG:
			ball->m_vy *= -1;
			ball->m_x += ball->m_vx;
			ball->m_y += ball->m_vy;
			break;
		case  BALL_XY_NG:
			ball->m_vx *= -1;
			ball->m_vy *= -1;
			ball->m_x += ball->m_vx;
			ball->m_y += ball->m_vy;
			break;
		default:
			iRet = -1;
	}

	if(ball->m_y == 30)
	{
		iRet = -1;
	}

	return iRet;
}

BALL_CHECK moniBallCheck(unsigned char x, unsigned char y, char vx, char vy)
{
	unsigned char ucRet;

	if((g_uiFieldInf[y + vy] & (0x00000001 << (x + vx))) == 0)
	{
		ucRet = BALL_OK;
	}
	else if((g_uiFieldInf[y - vy] & (0x00000001 << (x + vx))) == 0)
	{
		ucRet = BALL_Y_NG;
	}
	else if((g_uiFieldInf[y + vy] & (0x00000001 << (x - vx))) == 0)
	{
		ucRet = BALL_X_NG;
	}
	else
	{
		ucRet = BALL_XY_NG;
	}

	return ucRet;
}

void moniBlockCreate(char *field)
{
	unsigned char x;
	unsigned char y;

	srand((unsigned int)time(NULL));

	for(char i = 0; i < 20; i++)
	{
		x = (rand() % 29) + 1;
		y = (rand() % 20) + 1;
		field[moniFieldPos(x,y)] = 'X';

		g_uiFieldInf[y] |= 0x000001 << x;
	}

}
