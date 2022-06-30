
// GoBangView.h: CGoBangView 类的接口
//

#pragma once
#pragma warning(disable : 4996)
#include "GoBangUCT.h"
#include "GoBangDoc.h"

#define BOARD_MARGIN  10

class CGoBangView : public CView
{
protected: // 仅从序列化创建
	CGoBangView() noexcept;
	DECLARE_DYNCREATE(CGoBangView)

	// 特性
public:
	CGoBangDoc* GetDocument() const;

	// 操作
public:
	int boardwidth, d;
	COLORREF boardcolor;
	GoBangUCT Game;
	BOOL GameSetup;

	// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void DrawStone(int d, CDC* pWhiteStoneDC, CDC* pBlackStoneDC);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// 实现
public:
	virtual ~CGoBangView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileNew();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void On32771();
	afx_msg void OnRepeal();
	afx_msg void Onmoni();
	afx_msg void OnHint();
	afx_msg void Onimport();
	afx_msg void Onoutport();
	afx_msg void OnInstruction();
	afx_msg void OnExit();
	afx_msg void OnShow();

public:
	bool HumanGamePlay(CPoint point);
	bool ComputerGamePlay();
	double score;
	double getscore() { return score; }
};

#ifndef _DEBUG  // GoBangView.cpp 中的调试版本
inline CGoBangDoc* CGoBangView::GetDocument() const
{
	return reinterpret_cast<CGoBangDoc*>(m_pDocument);
}
#endif

