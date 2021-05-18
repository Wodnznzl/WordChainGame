// ChildSocket.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Server.h"
#include "ChildSocket.h"
#include "ServerDlg.h"

// CChildSocket

CChildSocket::CChildSocket()
{
}

CChildSocket::~CChildSocket()
{
}


// CChildSocket ��� �Լ�


void CChildSocket::OnClose(int nErrorCode)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	POSITION pos = m_pListenSocket->m_pChildSocketList.Find(this);
	m_pListenSocket->m_pChildSocketList.RemoveAt(pos);
	CString strSocketName; UINT uPort;
	this->GetPeerName(strSocketName, uPort);
	CString str; str.Format(_T("[%s:%d] ���� ����\r\n"), strSocketName, uPort);
	CServerDlg* pMain = (CServerDlg*)AfxGetMainWnd();
	pMain->m_ctrlEdit.ReplaceSel(str);
	this->ShutDown();
	this-> Close();
	delete this;
	CSocket::OnClose(nErrorCode);
}


void CChildSocket::OnReceive(int nErrorCode)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	//�޽��� �ް� �� ��
	CString strlPAddress; UINT uPortNumber;
	GetPeerName(strlPAddress, uPortNumber);

	TCHAR szBuffer[1024];
	int nRead = Receive(szBuffer, sizeof(szBuffer));
	switch (nRead) {
	case 0: this->Close(); break;
	case SOCKET_ERROR: if (GetLastError() != WSAEWOULDBLOCK) {
		AfxMessageBox(_T("Error occured"));
		this->Close();
	}break;
	default:
		szBuffer[nRead] = _T('\0');
		CString str; str.Format(_T("[%s:%u] %s"), strlPAddress, uPortNumber, szBuffer);
		//CString str; str.Format(_T("[%s:%u] %c"), strlPAddress, uPortNumber, szBuffer[0]);

		CServerDlg* pMain = (CServerDlg*)AfxGetMainWnd();
		pMain->m_ctrlEdit.ReplaceSel(str);

		CServerDlg* pDlg = (CServerDlg*)AfxGetMainWnd();

		if (szBuffer[0] == '0') {
			//ȸ������
			CString ID, PASSWORD;
			AfxExtractSubString(ID, szBuffer, 1, ' ');
			AfxExtractSubString(PASSWORD, szBuffer, 2, ' ');
			pDlg->SignUp(ID, PASSWORD);
		}
		else if (szBuffer[0] == '1') {
			//�α���
			CString ID, PASSWORD;
			AfxExtractSubString(ID, szBuffer, 1, ' ');
			AfxExtractSubString(PASSWORD, szBuffer, 2, ' ');
			pDlg->Login(ID, PASSWORD);
		}


		break;
	}
	CSocket::OnReceive(nErrorCode);
}