
#include "pch.h"
//#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <time.h>
#include <cstring>
#include <algorithm>
#include <unordered_map>
#include "GoBangUCT.h"
#include <vector>
#define _CRT_SECURE_NO_WARNINGS
using namespace std;


const int typenum = 73;			//五子棋棋形数量
const pair<string, int> BlackType[100] = { {"",0},
	{"11111",0},//五连,1
	{"011110",1},//活四,1
	{"011113",2},{"011112",2},{"10111",2},{"11011",2},{"11101",2},{"211110",2},{"311110",2},//冲四,7
	{"011100",3},{"010110",3},{"011010",3},{"001110",3},//活三,4
	{"001112",4},{"010112",4},{"011012",4},{"001113",4},{"010113",4},{"011013",4},
	{"211100",4},{"211010",4},{"210110",4},{"311100",4},{"311010",4},{"310110",4},
	{"10011",4},{"10101",4},{"11001",4},{"2011102",4},{"3011103",4},{"2011103",4},{"3011102",4},//冲三,19
	{"00110",5},{"01100",5},{"01010",5},{"010010",5},//活二,4
	{"000112",6},{"001012",6},{"010012",6},{"10001",6},{"2010102",6},{"2011002",6},{"3010102",6},{"3011002",6},
	{"211000",6},{"210100",6},{"210010",6},{"2001102",6},{"3001102",6},
	{"000113",6},{"001013",6},{"010013",6},{"2010103",6},{"2011003",6},{"3011003",6},{"3010103",6},
	{"311000",6},{"310100",6},{"310010",6},{"3001103",6},{"2001103",6},//冲二,25
	{"211112",7},{"311113",7},{"211113",7},{"311112",7},//死四,4
	{"21112",8},{"31113",8},{"21113",8},{"31112",8},//死三,4
	{"2112",9},{"3113",9},{"2113",9},{"3112",9}//死二,4
};							//黑棋所有棋形	
const pair<string, int> WhiteType[100] = { {"",0},
	{"22222",0},//五连
	{"022220",1},//活四
	{"022223",2},{"022221",2},{"20222",2},{"22022",2},{"22202",2},{"122220",2},{"322220",2},//冲四
	{"022200",3},{"020220",3},{"022020",3},{"002220",3},//活三
	{"002221",4},{"020221",4},{"022021",4},{"002223",4},{"020223",4},{"022023",4},
	{"122200",4},{"122020",4},{"120220",4},{"322200",4},{"322020",4},{"320220",4},
	{"20022",4},{"20202",4},{"22002",4},{"1022201",4},{"3022203",4},{"1022203",4},{"3022201",4},//冲三
	{"00220",5},{"02200",5},{"02020",5},{"020020",5},//活二
	{"000221",6},{"002021",6},{"020021",6},{"20002",6},{"1020201",6},{"1022001",6},{"3020201",6},{"3022001",6},
	{"122000",6},{"120200",6},{"120020",6},{"1002201",6},{"3002201",6},
	{"000223",6},{"002023",6},{"020023",6},{"1020203",6},{"1022003",6},{"3022003",6},{"3020203",6},
	{"322000",6},{"320200",6},{"320020",6},{"3002203",6},{"1002203",6},//冲二
	{"122221",7},{"322223",7},{"122223",7},{"322221",7},//死四
	{"12221",8},{"32223",8},{"12223",8},{"32221",8},//死三
	{"1221",9},{"3223",9},{"1223",9},{"3221",9}//死二
};							//白棋所有棋形

const int TypeVal[12] = {
	1000000,//五连
	10000,//活四
	10000,//双冲四，双活三，冲四+活三
	1000,//冲四+冲三，活三+冲三
	500,//冲四
	500,//活三
	100,//双活二
	50,//冲三
	20,//活二+冲二
	10,//活二
	5,//冲二
	-10//死四、死三、死二
};							//各个棋形估值
GoBangUCT::GoBangUCT(void)//构造函数
{
	Bushu = 0;
	dx = dy = -1;
	length = 0;
	para = 0.45;
	memset(m_filename, 0, sizeof(m_filename));
	memset(Board, 0, sizeof(Board));
	memset(Outport_Board, 0, sizeof(Outport_Board));
	memset(Blank, 0, sizeof(Blank));
	memset(Import_Board, 0, sizeof(Import_Board));
}

GoBangUCT::~GoBangUCT(void)//析构函数
{
}

void GoBangUCT::Game_Init()//游戏变量初始化
{
	Bushu = 0;
	dx = dy = -1;
	length = 0;
	para = 0.45;
	memset(m_filename, 0, sizeof(m_filename));
	memset(Board, 0, sizeof(Board));
	memset(Outport_Board, 0, sizeof(Outport_Board));
	memset(Blank, 0, sizeof(Blank));
	memset(Import_Board, 0, sizeof(Import_Board));
}
void GoBangUCT::qipan_dayin()//输出棋盘(未加可视化界面时使用)
{
	char a = 'A', b = '1';
	cout << "  |";
	for (int i = 0; i < EDGE; i++)
	{
		cout << setw(2) << a++;
	}
	cout << endl;
	cout << "--+";
	for (int i = 0; i < EDGE; i++)
	{
		cout << "--";
	}
	cout << endl;
	for (int i = 0; i < EDGE; i++)
	{
		cout << setw(2) << b++ << "|";
		for (int j = 0; j < EDGE; j++)
		{
			if (Board[j][i] == 1)
				cout << "○";
			else if (Board[j][i] == 2)
				cout << "●";
			else
				cout << "  ";
		}
		cout << endl;
	}
	return;
}
bool GoBangUCT::judge(int x, int y)//判断赢否
{
	bool isWin = false;
	int x1, y1, x2, y2, len, col = Board[x][y];
	for (int i = 0; i < 4; i++)	//每个棋子四条线
	{
		len = 1;
		x1 = x2 = x, y1 = y2 = y;
		for (int j = 1; j <= 5; j++)
		{
			x1 += Direc[i][0], y1 += Direc[i][1];
			if (y1 < 0 || y1 >= EDGE || x1 < 0 || x1 >= EDGE || Board[x1][y1] != col)
				break;
			len++;
		}
		for (int j = 1; j <= 5; j++)
		{
			x2 -= Direc[i][0], y2 -= Direc[i][1];
			if (y2 < 0 || y2 >= EDGE || x2 < 0 || x2 >= EDGE || Board[x2][y2] != col)
				break;
			len++;
		}
		if (len >= 5)//五子连珠
		{
			isWin = true;
			break;
		}
	}
	return isWin;
}
int GoBangUCT::getscore(int a[10])//计算估值
{
	int sum = 0;
	if (a[0] >= 1)sum += TypeVal[0], a[0]--;//五连
	if (a[1] >= 1)sum += TypeVal[1] * a[1], a[1] = 0;//活四
	if (a[2] >= 2)sum += TypeVal[2], a[2] -= 2;//双冲四
	if (a[2] >= 1 && a[3] >= 1)sum += TypeVal[2], a[2]--, a[3]--;//冲四+活三
	if (a[3] >= 2)sum += TypeVal[2], a[3] -= 2;//双活三
	if (a[2] >= 1 && a[4] >= 1)sum += TypeVal[3], a[2]--, a[4]--;//冲四+冲三
	if (a[3] >= 1 && a[4] >= 1)sum += TypeVal[3], a[3]--, a[4]--;//活三+冲三
	if (a[2] >= 1)sum += TypeVal[4] * a[2], a[2] = 0;//冲四
	if (a[3] >= 1)sum += TypeVal[5] * a[3], a[3] = 0;//活三
	if (a[5] >= 2)sum += TypeVal[6], a[5] -= 2;//双活二
	if (a[4] >= 1)sum += TypeVal[7] * a[4], a[4] = 0;//冲三
	if (a[5] >= 1 && a[6] >= 1)sum += TypeVal[8], a[5]--, a[6]--;//活二+冲二
	if (a[5] >= 1)sum += TypeVal[9] * a[5], a[5] = 0;//活二
	if (a[6] >= 1)sum += TypeVal[10] * a[6], a[6] = 0;//冲二
	sum += TypeVal[11] * (a[7] + a[8] + a[9]);//死四、死三、死二
	return sum;
}
void GoBangUCT::getval(string s, int col, int type[2][10])//输入包含棋子长度为11的字符串
{
	int len = int(s.length()), idx = INT_MAX, typelen = 0;
	string t[8];
	if (col == BLACK)
	{
		for (int i = 0; i < 6; i++)//字符串匹配
		{
			for (int j = 4; j <= 7; j++)//建立不同长度的字符串
				t[j] = s.substr(i, j);
			for (int j = 1; j <= typenum; j++)//对应匹配棋形
			{
				if (t[BlackType[j].first.size()] == BlackType[j].first && idx > BlackType[j].second)
					idx = BlackType[j].second;
			}
		}
		if (idx <= 9)
			type[BLACK - 1][idx]++;//对应棋形数量加一
	}
	else
	{
		for (int i = 0; i < 6; i++)
		{
			for (int j = 4; j <= 7; j++)
				t[j] = s.substr(i, j);
			for (int j = 1; j <= typenum; j++)
			{
				if (t[WhiteType[j].first.size()] == WhiteType[j].first && idx > WhiteType[j].second)
					idx = WhiteType[j].second;
			}
		}
		if (idx <= 9)
			type[WHITE - 1][idx]++;
	}
}
double GoBangUCT::assess(int x, int y)//单层估值函数
{
	int x1, y1, x2, y2, col = Board[x][y];
	int type[2][10] = { 0 };
	double score = min(min(x, EDGE - 1 - x), min(y, EDGE - 1 - y));
	for (int i = 0; i < 4; i++)//每个棋子四条线
	{
		string s1, s2, t;
		x1 = x2 = x, y1 = y2 = y;
		for (int j = 1; j <= 5; j++)
		{
			x1 += Direc[i][0], y1 += Direc[i][1];
			if (y1 < 0 || y1 >= EDGE || x1 < 0 || x1 >= EDGE)
				s1 += '3';//墙壁
			else
				s1 += Board[x1][y1] + '0';//棋子或空白
		}
		for (int j = 1; j <= 5; j++)
		{
			x2 -= Direc[i][0], y2 -= Direc[i][1];
			if (y2 < 0 || y2 >= EDGE || x2 < 0 || x2 >= EDGE)
				s2 += '3';
			else
				s2 += Board[x2][y2] + '0';
		}
		reverse(s1.begin(), s1.end());
		t = s1 + char('0' + col) + s2;
		getval(t, col, type);
		t = s1 + char('0' + BLACK + WHITE - col) + s2;
		getval(t, 3 - col, type);
	}
	score += para * getscore(type[col - 1]) + (1 - para) * getscore(type[2 - col]);
	//既要考虑对我方棋子的影响，也要考虑对对方棋子的影响。
	return score;
}
double GoBangUCT::AlphaBetaAssess(int x, int y)//αβ剪枝估值函数
{
	int x1, y1, x2, y2, col = Board[x][y];
	int type[2][10] = { 0 };
	double score = min(min(x, EDGE - 1 - x), min(y, EDGE - 1 - y));
	for (int i = 0; i < 4; i++)
	{
		string s1, s2, t;
		x1 = x2 = x, y1 = y2 = y;
		for (int j = 1; j <= 5; j++)
		{
			x1 += Direc[i][0], y1 += Direc[i][1];
			if (y1 < 0 || y1 >= EDGE || x1 < 0 || x1 >= EDGE)
				s1 += '3';
			else
				s1 += Board[x1][y1] + '0';
		}
		for (int j = 1; j <= 5; j++)
		{
			x2 -= Direc[i][0], y2 -= Direc[i][1];
			if (y2 < 0 || y2 >= EDGE || x2 < 0 || x2 >= EDGE)
				s2 += '3';
			else
				s2 += Board[x2][y2] + '0';
		}
		reverse(s1.begin(), s1.end());
		t = s1 + char('0' + col) + s2;
		getval(t, col, type);
		t = s1 + char('0' + BLACK + WHITE - col) + s2;
		getval(t, 3 - col, type);
	}
	score += getscore(type[col - 1]);//主要调整估值函数
	//+ (search_depth % 2 == 1 ? 0.1 : -0.1) * getscore(type[2 - col]);
	return score;
}
double GoBangUCT::SingleSearch(int& x, int& y)//单层搜索
{
	int cnt = 0, imax = 0;
	double score = 0, maxscore = -1e9;
	for (int i = 0; i < EDGE; i++)
	{
		for (int j = 0; j < EDGE; j++)
		{
			if (Board[i][j] == EMPTY)//找全棋盘空点
			{
				Blank[++cnt].x = i;
				Blank[cnt].y = j;
			}
		}
	}
	//找出最大估值
	for (int i = 1; i <= cnt; i++)
	{
		score = 0;
		x = Blank[i].x, y = Blank[i].y;
		Board[x][y] = Bushu % 2 + 1;
		if (judge(x, y))score = 1e7;
		Board[x][y] = 3 - Board[x][y];
		if (judge(x, y))score = 1e6;
		Board[x][y] = 3 - Board[x][y];
		if (score == 0 && maxscore < 1e6)score = assess(x, y);
		if (maxscore < score)
		{
			maxscore = score;
			imax = i;
		}
		Board[x][y] = EMPTY;
	}
	x = Blank[imax].x;
	y = Blank[imax].y;
	return maxscore;
}

bool GoBangUCT::isempty(int x, int y)//判断周围有无棋子
{
	if (Board[x][y] == EMPTY)
	{
		for (int i = -2; i <= 2; i++)
		{
			for (int j = -2; j <= 2; j++)
			{
				if (abs(i) + abs(j) == 3)
					continue;

				if (x + i >= 0 && x + i < EDGE && y + j >= 0 && y + j < EDGE && Board[x + i][y + j])
					return true;
			}
		}
	}
	return false;
}

double GoBangUCT::AlphabetaSearch(int depth, int& x, int& y, double alpha, double beta)//αβ剪视搜索树
{
	double cur_score = 0;
	for (int i = 0; i < EDGE; i++)
	{
		for (int j = 0; j < EDGE; j++)
		{
			if (isempty(i, j))
			{
				Board[i][j] = (Bushu + depth) % 2 + 1;
				int next_x = 0, next_y = 0;
				if (depth == search_depth - 1)
					cur_score = AlphaBetaAssess(i, j);
				else
					cur_score = -AlphabetaSearch(depth + 1, next_x, next_y, -beta, -alpha);
				Board[i][j] = EMPTY;

				//剪枝
				if (alpha < cur_score)
				{
					alpha = cur_score;
					x = i;
					y = j;
					//剪枝关键判断
					if (beta <= alpha)
						break;
				}
			}
		}
	}
	if (x < 0 || y < 0)
		x = y = 7;
	return alpha;
}

int GoBangUCT::Human(int& x, int& y)//人下棋
{
	bool isEnd = false;
	Board[x][y] = Bushu++ % 2 + 1;
	isEnd = judge(x, y);
	GoBang[length].x = x;
	GoBang[length++].y = y;
	dx = x;
	dy = y;
	if (length == EDGE * EDGE)
		return 0;
	else
		return isEnd ? 1 : -1;
}

int GoBangUCT::Computer(int& x, int& y, int computer_engine)//机器下棋
{
	bool isWin = false;
	double score = SingleSearch(x, y);
	if (score < 4000.0 && computer_engine == AlphaBeta_engine)
		AlphabetaSearch(0, x, y);
	Board[x][y] = Bushu++ % 2 + 1;
	isWin = judge(x, y);
	GoBang[length].x = x;
	GoBang[length++].y = y;
	dx = x;
	dy = y;
	if (length == EDGE * EDGE)
		return 0;
	else
		return isWin ? 1 : -1;
}

int GoBangUCT::Repeal()//悔棋
{
	if (length < 2)return -1;//棋子太少无法悔棋
	Board[GoBang[length - 1].x][GoBang[length - 1].y] = EMPTY;
	Board[GoBang[length - 2].x][GoBang[length - 2].y] = EMPTY;
	length -= 2;
	dx = GoBang[length - 1].x;
	dy = GoBang[length - 1].y;
	Bushu -= 2;
	return 0;
}

void GoBangUCT::gengxin()
{
	memset(Board, EMPTY, sizeof(Board));
}
void GoBangUCT::Qipu_Print(int i)//棋谱打印
{
	Board[GoBang[i].x][GoBang[i].y] = i % 2 + 1;
	dx = GoBang[i].x;
	dy = GoBang[i].y;
}

int GoBangUCT::Hint(int& x, int& y)//提示
{
	SingleSearch(x, y);
	return Bushu % 2 + 1;
}

int GoBangUCT::FileOpen()//导入棋谱
{
	int n = 0;
	char a;
	FILE* file;
	file = fopen(m_filename, "r");
	memset(Board, 0, sizeof(Board));
	if (!file)
		return 0;
	while (!feof(file))
	{
		a = fgetc(file);
		if (!(a >= '0' && a <= '9'))
			continue;
		Import_Board[n].x = a - '0';
		fgetc(file);
		Import_Board[n++].y = fgetc(file) - '0';
	}
	fclose(file);
	return n;
}