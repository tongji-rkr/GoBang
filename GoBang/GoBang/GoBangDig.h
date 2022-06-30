#pragma once


// GoBangDig 对话框

class GoBangDig : public CDialogEx
{
	DECLARE_DYNAMIC(GoBangDig)

public:
	GoBangDig(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~GoBangDig();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_player;
	afx_msg void OnBnClickedOk();
	//	afx_msg void OnCbnSelchangePlayer();
	int m_engine;
	int m_Pattern;
};
