
// GoBangView.cpp: CGoBangView 类的实现
//


#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "GoBang.h"
#endif

#include "GoBangDoc.h"
#include "GoBangView.h"
#include "GoBangDig.h"
#include "GoBangUCT.h"
#include "afxver_.h"

#include <math.h>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <Mmsystem.h>
#pragma comment(lib, "WINMM.LIB")
using namespace std;
//#define _CRT_SECURE_NO_WARNINGS

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

double GoBangUCT::para = 0.6;
int GoBangUCT::Board[EDGE][EDGE] = { 0 };

CRect rcWindow, rcBoard;//CRect类是MFC的常用类，矩形

// CGoBangView

IMPLEMENT_DYNCREATE(CGoBangView, CView)

BEGIN_MESSAGE_MAP(CGoBangView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_NEW, &CGoBangView::OnFileNew)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_32771, &CGoBangView::On32771)
	ON_COMMAND(ID_REPEAL, &CGoBangView::OnRepeal)
	ON_COMMAND(ID_moni, &CGoBangView::Onmoni)
	ON_COMMAND(ID_HINT, &CGoBangView::OnHint)
	ON_COMMAND(ID_import, &CGoBangView::Onimport)
	ON_COMMAND(ID_outport, &CGoBangView::Onoutport)
	ON_COMMAND(ID_INSTRUCTION, &CGoBangView::OnInstruction)
	ON_COMMAND(ID_EXIT, &CGoBangView::OnExit)
	ON_COMMAND(ID_SHOW, &CGoBangView::OnShow)
END_MESSAGE_MAP()


// CGoBangView 构造/析构

CGoBangView::CGoBangView() noexcept
{
	// TODO: 在此处添加构造代码
	boardcolor = RGB(193, 187, 97);
	GameSetup = true;
	boardwidth = 0;
	score = 0;
	d = 0;
	//	Game.Game_Init();
}

CGoBangView::~CGoBangView()
{
}

BOOL CGoBangView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CGoBangView 绘图

void CGoBangView::OnDraw(CDC* pDC)
{
	CGoBangDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	GetClientRect(&rcWindow);//GetClientRect为获得控件相自身的坐标大小，top和left都为0

	boardwidth = (rcWindow.Width() > rcWindow.Height() ? rcWindow.Height() : rcWindow.Width());
	boardwidth -= BOARD_MARGIN * 2;
	if (boardwidth < 0) return;

	rcBoard.top = (rcWindow.Height() - boardwidth) / 2;
	rcBoard.bottom = rcBoard.top + boardwidth;
	rcBoard.left = (rcWindow.Width() - boardwidth) / 2;
	rcBoard.right = rcBoard.left + boardwidth;

	CDC memDC;  //首先定义一个显示设备对象，所有的绘制首先绘制到这块内存中
	memDC.CreateCompatibleDC(pDC);//创建一个与指定设备兼容的内存设备上下文环境（DC）
	CBitmap memBitmap;//定义一个位图对象
	memBitmap.CreateCompatibleBitmap(pDC, rcWindow.Width(), rcWindow.Height());//建立一个与屏幕显示兼容的位图，至于位图的大小嘛，可以用窗口的大小
	CBitmap* pOldBitmap = memDC.SelectObject(&memBitmap);//保存old位图

	// start to draw board
	memDC.FillRect(&rcWindow, &CBrush(boardcolor));
	memDC.FillRect(&rcBoard, &CBrush(boardcolor));

	d = boardwidth / (EDGE + 1);	// stone width

	memDC.SetBkMode(TRANSPARENT);//使输出的字符背景透明。
	CFont font;
	CSize size;
	CString str;
	//char num[10], * np = num;
	font.CreateFont(d * 4 / 7, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));
	CFont* pOldFont = (CFont*)memDC.SelectObject(&font);

	int i, x, y;
	if (d > 0)
	{
		// draw frame 画框架的横竖线
		for (x = 0; x < EDGE; x++) {
			i = rcBoard.left + (x + 1) * boardwidth / (EDGE + 1);
			memDC.MoveTo(i, rcBoard.top + d);
			memDC.LineTo(i, rcBoard.bottom - d);

			str = (char)('A' + x);
			size = memDC.GetTextExtent(str);
			memDC.TextOut(i - size.cx / 2 + 5, rcBoard.top + d - size.cy - 20, str);//画顶端文字。
			memDC.TextOut(i - size.cx / 2 + 5, rcBoard.bottom - d + 20, str);//画底端文字。
		}
		for (y = 0; y < EDGE; y++) {
			i = rcBoard.top + (y + 1) * boardwidth / (EDGE + 1);
			memDC.MoveTo(rcBoard.left + d, i);
			memDC.LineTo(rcBoard.right - d, i);


			//str = CString(_itoa(EDGE - y, np,10));
			str.Format(_T("%d"), EDGE - y);
			size = memDC.GetTextExtent(str);
			memDC.TextOut(rcBoard.left + d - size.cx - 20, i - size.cy / 2, str);//画左端文字。
			memDC.TextOut(rcBoard.right - d - size.cx / 3 + 22, i - size.cy / 2, str);//画右端数字。
		}

		CDC WhiteStoneDC, BlackStoneDC;
		BlackStoneDC.CreateCompatibleDC(pDC);
		WhiteStoneDC.CreateCompatibleDC(pDC);

		CBitmap BlackStoneBmp, WhiteStoneBmp;
		BlackStoneBmp.CreateCompatibleBitmap(pDC, d, d);
		WhiteStoneBmp.CreateCompatibleBitmap(pDC, d, d);

		CBitmap* pOldBlackBmp = BlackStoneDC.SelectObject(&BlackStoneBmp);
		CBitmap* pOldWhiteBmp = WhiteStoneDC.SelectObject(&WhiteStoneBmp);

		DrawStone(d, &WhiteStoneDC, &BlackStoneDC);

		// show stones 画棋子
		int x1, y1;
		char stone;
		for (x = 0; x < EDGE; x++)
			for (y = 0; y < EDGE; y++)
			{
				x1 = rcBoard.left + (x + 1) * boardwidth / (EDGE + 1);
				y1 = rcBoard.top + (y + 1) * boardwidth / (EDGE + 1);
				stone = Game.Board[x][y];

				if (stone == BLACK)//
					memDC.BitBlt(x1 - d / 2, y1 - d / 2, d, d, &BlackStoneDC, 0, 0, SRCCOPY);
				else if (stone == WHITE)
					memDC.BitBlt(x1 - d / 2, y1 - d / 2, d, d, &WhiteStoneDC, 0, 0, SRCCOPY);////将MemDC的图拷贝到屏幕（dc）上进行显示
			}
		//当前走步显示红十字。
		CPen pen(PS_SOLID, 1, RGB(255, 0, 0));
		CPen* pOldPen = memDC.SelectObject(&pen);
		x = Game.dx;
		y = Game.dy;

		if (x > -1 && y > -1)//如果是有效的点。
		{
			x1 = rcBoard.left + (x + 0) * boardwidth / (EDGE + 1);
			y1 = rcBoard.top + (y + 0) * boardwidth / (EDGE + 1);
			memDC.MoveTo(x1 + d, y1 + d * 3 / 4);
			memDC.LineTo(x1 + d, y1 + d * 5 / 4);
			memDC.MoveTo(x1 + d * 3 / 4, y1 + d);
			memDC.LineTo(x1 + d * 5 / 4, y1 + d);
		}

		memDC.SelectObject(pOldPen);

		//ShowPointValue(&memDC,&rcBoard,boardwidth,d,side);//显示点的估值。

		WhiteStoneDC.SelectObject(pOldWhiteBmp);
		BlackStoneDC.SelectObject(pOldBlackBmp);
	}

	pDC->BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOldBitmap);
}

///////////////////////////////////////////////////////////////////////////
//
//   DrawStone() is based on the source code of Jago 棋子
//
////////////////////////////////////////////////////////////////////////////

void CGoBangView::DrawStone(int d, CDC* pWhiteStoneDC, CDC* pBlackStoneDC)
{

	double pixel = 0.8, shadow = 0.7;
	BOOL Alias = true;

	int red = 255 - GetRValue(boardcolor);
	int green = 255 - GetGValue(boardcolor);
	int blue = 255 - GetBValue(boardcolor);

	COLORREF pb, pw;
	double di, dj, d2 = (double)d / 2.0 - 5e-1, r = d2 - 2e-1, f = sqrt(3.0);
	double x, y, z, xr, xg, hh;
	int g;

	//if (GF.getParameter("smallerstones",false)) r-=1;
	for (int i = 0; i < d; i++) {
		for (int j = 0; j < d; j++)
		{
			di = i - d2; dj = j - d2;
			hh = r - sqrt(di * di + dj * dj);
			if (hh >= 0)
			{
				z = r * r - di * di - dj * dj;
				if (z > 0) z = sqrt(z) * f;
				else z = 0;
				x = di; y = dj;
				xr = sqrt(6 * (x * x + y * y + z * z));
				xr = (2 * z - x + y) / xr;
				if (xr > 0.9) xg = (xr - 0.9) * 10;
				else xg = 0;
				if (hh > pixel || !Alias) {
					g = (int)(10 + 10 * xr + xg * 140);
					//pb=(255<<24)|(g<<16)|(g<<8)|g;
					pb = (g << 16) | (g << 8) | g;
					g = (int)(200 + 10 * xr + xg * 45);
					//pw=(255<<24)|(g<<16)|(g<<8)|g;
					pw = (g << 16) | (g << 8) | g;
				}
				else
				{
					hh = (pixel - hh) / pixel;
					g = (int)(10 + 10 * xr + xg * 140);
					double shade;
					if (di - dj < r / 3) shade = 1;
					else shade = shadow;

					pb = (//(255<<24)|
						(((int)((1 - hh) * g + hh * shade * red)) << 16)
						| (((int)((1 - hh) * g + hh * shade * green)) << 8)
						| ((int)((1 - hh) * g + hh * shade * blue)));

					g = (int)(200 + 10 * xr + xg * 45);

					pw = (//(255<<24)|
						(((int)((1 - hh) * g + hh * shade * red)) << 16)
						| (((int)((1 - hh) * g + hh * shade * green)) << 8)
						| ((int)((1 - hh) * g + hh * shade * blue)));

				}
			}
			else pb = pw = boardcolor;

			pBlackStoneDC->SetPixel(j, i, pb);
			pWhiteStoneDC->SetPixel(j, i, pw);
		}
	}
}


// CGoBangView 打印

BOOL CGoBangView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CGoBangView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CGoBangView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CGoBangView 诊断

#ifdef _DEBUG
void CGoBangView::AssertValid() const
{
	CView::AssertValid();
}

void CGoBangView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGoBangDoc* CGoBangView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGoBangDoc)));
	return (CGoBangDoc*)m_pDocument;
}
#endif //_DEBUG


// CGoBangView 消息处理程序


void CGoBangView::OnFileNew()//新界面
{
	// TODO: 在此添加命令处理程序代码
	GoBangDig Dialog;
	if (Dialog.DoModal() == IDOK)
	{
		GameSetup = false;
		//选择
		Game.ComputerPattern = (Dialog.m_Pattern == 0 ? HC : HH);
		Game.ComputerColor = (Dialog.m_player == 0 ? BLACK : WHITE);
		Game.ComputerEngine = (Dialog.m_engine == 0 ? AlphaBeta_engine : Single_engine);
		Game.Game_Init();
		CPoint cp;
		cp.x = (EDGE - 1) / 2;
		cp.y = (EDGE - 1) / 2;
		OnLButtonDown(1, cp);
		//Invalidate();
		//RedrawWindow();

		return;
	}
}

bool CGoBangView::HumanGamePlay(CPoint point)//人下棋
{
	CString msg;
	if (boardwidth < 0)
		return false;
	int x = (point.x + d / 2 - rcBoard.left) * (EDGE + 1) / boardwidth - 1;
	int y = (point.y + d / 2 - rcBoard.top) * (EDGE + 1) / boardwidth - 1;
	if (x < 0 || y < 0 || x >= EDGE || y >= EDGE || Game.Board[x][y] != EMPTY)
		return false;
	int isWin = Game.Human(x, y);
	score = GoBangUCT::assess(x, y);
	PlaySound((LPCTSTR)IDR_WAVE1, NULL, SND_RESOURCE | SND_ASYNC);
	RedrawWindow();
	//int x = -1, y = -1;
	//CString msg;
	//int isWin = Game.Computer(x, y, MC_engine);
	//PlaySound((LPCTSTR)IDR_WAVE1, NULL, SND_RESOURCE | SND_ASYNC);
	//RedrawWindow();
	if (isWin == 1)
	{
		if (Game.Board[x][y] == BLACK)
		{
			msg = "黑方人赢!";
		}
		else
		{
			msg = "白方人赢!";
		}
		PlaySound((LPCTSTR)IDR_WAVE2, NULL, SND_RESOURCE | SND_ASYNC);
		GameSetup = true;
		MessageBox(msg);
	}
	else if (isWin == 0)
	{
		GameSetup = true;
		MessageBox(_T("平局!"));
	}
	if (isWin < 0)
		return false;
	else
		return true;
}
bool CGoBangView::ComputerGamePlay()
{
	int x = -1, y = -1;
	CString msg;
	int isWin = Game.Computer(x, y, Game.ComputerEngine);
	if (Game.ComputerEngine == Single_engine)
		score = GoBangUCT::assess(x, y);
	else
		score = GoBangUCT::AlphaBetaAssess(x, y);
	PlaySound((LPCTSTR)IDR_WAVE1, NULL, SND_RESOURCE | SND_ASYNC);
	RedrawWindow();
	if (isWin == 1)
	{
		if (Game.Board[x][y] == BLACK)
			msg = "黑方计算机赢!";
		else
			msg = "白方计算机赢!";
		PlaySound((LPCTSTR)IDR_WAVE2, NULL, SND_RESOURCE | SND_ASYNC);
		GameSetup = true;
		MessageBox(msg);
	}
	else if (isWin == 0)
	{
		GameSetup = true;
		MessageBox(_T("平局!"));
	}
	if (isWin < 0)
		return false;
	else
		return true;
}
void CGoBangView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (GameSetup)
		return;
	if (Game.ComputerPattern == HH)
	{
		if (HumanGamePlay(point))return;
	}
	else
	{
		if (Game.Bushu % 2 + 1 != Game.ComputerColor)
		{
			if (HumanGamePlay(point))return;
		}
		if (ComputerGamePlay())return;
	}
	CView::OnLButtonDown(nFlags, point);
}

void CGoBangView::On32771()
{
	// TODO: 在此添加命令处理程序代码
	GoBangDig Dialog;
	if (Dialog.DoModal() == IDOK)
	{
		GameSetup = false;
		Game.ComputerPattern = (Dialog.m_Pattern == 0 ? HC : HH);
		Game.ComputerColor = (Dialog.m_player == 0 ? BLACK : WHITE);
		Game.ComputerEngine = (Dialog.m_engine == 0 ? AlphaBeta_engine : Single_engine);
		Game.Game_Init();
		CPoint cp;
		cp.x = (EDGE - 1) / 2;
		cp.y = (EDGE - 1) / 2;
		OnLButtonDown(1, cp);
		//Invalidate();
		//RedrawWindow();

		return;
	}
}


void CGoBangView::OnRepeal()
{
	// TODO: 在此添加命令处理程序代码
	int ret = 0;
	if (GameSetup != BOOL(true))
		ret = Game.Repeal();
	if (ret == -1)
	{
		MessageBox(_T("无法悔棋！！！"));
	}
	RedrawWindow();
	return;
}


void CGoBangView::Onmoni()//复盘上次棋
{
	int  i = 0;
	// TODO: 在此添加命令处理程序代码
	Game.gengxin();
	while (i != Game.length)
	{
		Game.Qipu_Print(i);
		PlaySound((LPCTSTR)IDR_WAVE1, NULL, SND_RESOURCE | SND_ASYNC);
		RedrawWindow();
		i++;
		Sleep(1000);
	}
	MessageBox(_T("复盘结束！"));
	PlaySound((LPCTSTR)IDR_WAVE2, NULL, SND_RESOURCE | SND_ASYNC);
	return;
}


void CGoBangView::OnHint()//提示
{
	int x, y;
	int color = Game.Hint(x, y);
	// TODO: 在此添加命令处理程序代码
	if (GameSetup != BOOL(true))
	{
		for (int i = 1; i <= 2; i++)
		{
			Game.Board[x][y] = color;
			RedrawWindow();
			Sleep(500);
			Game.Board[x][y] = 0;
			RedrawWindow();
			Sleep(500);
		}
	}
	Game.Board[x][y] = EMPTY;
	return;
}


void CGoBangView::Onimport()//导入棋谱
{
	// TODO: 在此添加命令处理程序代码
	int n, i = 0;
	int import_Bushu = 0;
	CFileDialog dlg(TRUE);///TRUE为打开对话框，FALSE为另存为对话框
	if (IDOK == dlg.DoModal())
	{
		CString m_File;
		CString Type;
		m_File = dlg.GetPathName();//获取文件名
		strcpy(Game.m_filename, CT2A(m_File.GetString()));
		m_File.MakeReverse();//颠倒字符串顺序
		int pos = m_File.Find('.');//找到小数点的位置
		Type = m_File.Left(pos);//获取小数点左边的字符串
		Type.MakeReverse();//颠倒字符串顺序
		if (Type != "txt")
		{
			MessageBox(_T("类型错误，导入失败！"));
			PlaySound((LPCTSTR)IDR_WAVE3, NULL, SND_RESOURCE | SND_ASYNC);
		}
		else
		{
			n = Game.FileOpen();
			while (i != n)
			{
				Game.Board[Game.Import_Board[i].x][Game.Import_Board[i++].y] = import_Bushu++ % 2 + 1;
				//Invalidate();
				Game.dx = Game.Import_Board[i - 1].x;
				Game.dy = Game.Import_Board[i - 1].y;
				PlaySound((LPCTSTR)IDR_WAVE1, NULL, SND_RESOURCE | SND_ASYNC);
				RedrawWindow();
				Sleep(700);
			}
			MessageBox(_T("导入成功！"));
			PlaySound((LPCTSTR)IDR_WAVE2, NULL, SND_RESOURCE | SND_ASYNC);
		}
	}
	return;
}


void CGoBangView::Onoutport()//导出棋谱
{
	// TODO: 在此添加命令处理程序代码
	//FILE* file;
	int outport_Bushu = 0;
	char s[2][6] = { "BLACK", "WHITE" };
	//向文件夹ChessManual中导入棋谱
	char qip_name[100] = "..//ChessManual//";
	char acDay[5] = { 0 };
	char acHour[5] = { 0 };
	char acMin[5] = { 0 };
	time_t now;
	struct tm* timenow;
	time(&now);
	timenow = localtime(&now);
	strftime(acDay, sizeof(acDay), "%d", timenow);
	strftime(acHour, sizeof(acHour), "%H", timenow);
	strftime(acMin, sizeof(acMin), "%M", timenow);
	strncat(qip_name, acDay, 2);
	strncat(qip_name, acHour, 2);
	strncat(qip_name, acMin, 2);	//在文件名里添加天、时、分

	strcat(qip_name, "qipu.txt");	//在尾部加上后缀名
	ofstream fout(qip_name);
	for (int i = 0; i < Game.length; i++)
	{
		fout << s[i % 2];
		fout << "[" << Game.GoBang[i].x << "," << Game.GoBang[i].y << "]" << endl;
	}
	/*	file=fopen("1.txt", "w");
		for (int i = 0; i < Game.length; i++)
		{
			fputs(s[i % 2],file);
			fputc('[', file);
			fputc('0' + Game.GoBang[i].x, file);
			fputc(',', file);
			fputc('0' + Game.GoBang[i].y, file);
			fputc(']', file);
			fputc('\n', file);
		}
		fclose(file);*/
	MessageBox(_T("导出成功！"));
	PlaySound((LPCTSTR)IDR_WAVE2, NULL, SND_RESOURCE | SND_ASYNC);
	return;
}


void CGoBangView::OnInstruction()
{
	// TODO: 在此添加命令处理程序代码
	MessageBoxW(
		_T("1.点击新游戏，开始一个新的游戏\n"
			"2.在弹出窗口中选择人人/人机、机器先手/人先手、机器使用的算法\n"
			"3.点击导入、导出棋谱可以将储存的棋谱导入或者导出目前的棋盘\n"
			"4.导出的棋谱储存在GoBang中的ChessManual文件夹\n"
			"5.点击悔棋，可以回到上次下棋那步\n"
			"6.点击提示，可以给予本次下棋的建议\n"
			"7.点击回顾本局棋谱，可以一步步重现本局棋谱\n"),
		_T("使用说明"));
	MessageBox(_T("请点击文件，之后点击新游戏"));
	return;
}


void CGoBangView::OnExit()
{
	// TODO: 在此添加命令处理程序代码
	int ret;
	ret = MessageBox(_T("是否要退出游戏？"), _T("游戏选项"), MB_YESNO);
	if (ret == IDYES)
		exit(0);
	return;
}


void CGoBangView::OnShow()
{
	// TODO: 在此添加命令处理程序代码
	int ret;
	CString str;
	str.Format(_T("目前评价为%.2f"), score);
	ret = MessageBox(str);
	if (ret == IDYES)
		exit(0);
}