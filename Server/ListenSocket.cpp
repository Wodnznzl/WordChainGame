// ListenSocket.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Server.h"
#include "ListenSocket.h"
#include "ChildSocket.h"
#include "ServerDlg.h"

// CListenSocket

CListenSocket::CListenSocket()
{
}

CListenSocket::~CListenSocket()
{
}


// CListenSocket ��� �Լ�


void CListenSocket::OnAccept(int nErrorCode)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CChildSocket* pChild = new CChildSocket;
	BOOL check = Accept(*pChild);
	if (check == FALSE) {
		delete pChild;
		return;
	}
	m_pChildSocketList.AddTail(pChild);

	CString strSocketName; UINT uPort;
	pChild->GetPeerName(strSocketName, uPort);
	CString str; str.Format(_T("[%s:%d] ���� ������ �����մϴ�.\r\n"), strSocketName, uPort);
	CServerDlg* pMain = (CServerDlg*)AfxGetMainWnd();
	pMain->m_ctrlEdit.ReplaceSel(str);

	pChild->Send(str.GetBuffer(), str.GetLength());

	pChild->m_pListenSocket = this;

	CAsyncSocket::OnAccept(nErrorCode);
}
