#include<iostream>
using namespace std;
#pragma once
#pragma warning(disable : 4996)
#define EDGE 15		//棋盘大小EDGE*EDGE 
#define EMPTY 0		//棋盘未落子点，空
#define BLACK 1		//黑子
#define WHITE 2		//白子
#define Single_engine 1   //单层算法
#define AlphaBeta_engine 2   //AlphaBeta剪枝搜索树
#define HC 1	//人机对弈
#define HH 2	//人人对弈
#define WIN -1		//落子后赢
#define NOTWIN 0	//落子后没有赢

#define EVAL_MAX  1e9		//min层初始化alpha值为 inf，max层初始化beta值为 inf，
#define EVAL_MIN  -1e9		//max层初始化alpha值为 -inf， min层初始化beta值为 -inf，

const int Direc[4][2] = { {1,0},{0,1},{1,1},{1,-1} };	//四方向，其余四个可以通过取反得到
const int search_depth = 2; //搜索深度

struct Point
{
	int x;
	int y;
};
class GoBangUCT
{
public:
	static double para;						//单层估值函数参数
	int Bushu;								//棋步，即棋盘已落子数
	static int Board[EDGE][EDGE];			//棋盘
	Point Import_Board[EDGE * EDGE];		//导入备份棋盘
	Point Outport_Board[EDGE * EDGE];		//导出备份棋盘
	Point GoBang[EDGE * EDGE];				//棋盘下棋记录
	Point Blank[EDGE * EDGE];				//记录空白的点

	int dx;									//靶心位置，坐标x
	int dy;									//靶心位置，坐标y
	int length;

	int ComputerColor;						//计算机执黑or执白
	int ComputerEngine;						//计算机选择单层还是αβ剪枝
	int ComputerPattern;					//人机还是人人

	char m_filename[100];					//棋谱路径

public:
	GoBangUCT(void);						//构造函数
	~GoBangUCT(void);						//析构函数
	void Game_Init();						//游戏初始化
	void qipan_dayin();						//输出棋盘(未加可视化界面时使用)
	bool judge(int x, int y);				//判断赢否
	static int getscore(int a[10]);			//计算估值
	static void getval(string s, int col, int type[2][10]);
	static double assess(int x, int y);		//单层估值函数
	double SingleSearch(int& x, int& y);	//单层搜索
	bool isempty(int x, int y);				//判断周围有无棋子
	static double AlphaBetaAssess(int x, int y);//αβ剪枝估值函数
	double AlphabetaSearch(int depth, int& x, int& y, double alpha = EVAL_MIN, double beta = EVAL_MAX);	//αβ剪视搜索树
	int Human(int& x, int& y);				//人下棋	
	int Computer(int& x, int& y, int computer_engine);//机器下棋
	int Repeal();							//悔棋
	void Qipu_Print(int i);					//输出棋谱	
	void gengxin();
	int Hint(int& x, int& y);				//提示
	int FileOpen();							//输入棋谱
};
