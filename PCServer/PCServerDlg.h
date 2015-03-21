
// PCServerDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CPCServerDlg 对话框
class CPCServerDlg : public CDialogEx
{
// 构造
public:
	CPCServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PCSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	NOTIFYICONDATA m_nid;
	HICON m_hIcon;
	// 生成的消息映射函数

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void resize();
	static DWORD WINAPI threadFunc(LPVOID threadNum);
	LRESULT OnShowTask(WPARAM wParam, LPARAM lParam);
	afx_msg void OnStnClickedStaticCmd();
	afx_msg void OnStnClickedrights();
	afx_msg void OnBnClickedBtnStart();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClose();
	afx_msg void OnChangmenuitem(); //这里添加菜单命令处理函数的声明
	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButton1();
};
