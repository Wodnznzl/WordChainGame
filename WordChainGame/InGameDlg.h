#pragma once


// CInGameDlg ��ȭ �����Դϴ�.

class CInGameDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInGameDlg)

public:
	CInGameDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CInGameDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INGAME };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
