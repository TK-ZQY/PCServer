
// PCServerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CPCServerDlg �Ի���
class CPCServerDlg : public CDialogEx
{
// ����
public:
	CPCServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PCSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	NOTIFYICONDATA m_nid;
	HICON m_hIcon;
	// ���ɵ���Ϣӳ�亯��

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
	afx_msg void OnChangmenuitem(); //������Ӳ˵��������������
	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButton1();
};
