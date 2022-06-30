// GoBangDig.cpp: 实现文件
//

#include "pch.h"
#include "GoBang.h"
#include "GoBangDig.h"
#include "afxdialogex.h"


// GoBangDig 对话框

IMPLEMENT_DYNAMIC(GoBangDig, CDialogEx)

GoBangDig::GoBangDig(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_player(0)
	, m_engine(0)
	, m_Pattern(0)
{

}

GoBangDig::~GoBangDig()
{
}

void GoBangDig::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_PLAYER, m_player);
	DDX_CBIndex(pDX, IDC_COMBO3, m_engine);
	DDX_CBIndex(pDX, IDC_PATTERN, m_Pattern);
}


BEGIN_MESSAGE_MAP(GoBangDig, CDialogEx)
	ON_BN_CLICKED(IDOK, &GoBangDig::OnBnClickedOk)
END_MESSAGE_MAP()


// GoBangDig 消息处理程序


void GoBangDig::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}
