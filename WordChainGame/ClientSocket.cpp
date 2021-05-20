// ClientSocket.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WordChainGame.h"
#include "ClientSocket.h"
#include "WordChainGameDlg.h"
#include "InGameDlg.h"

// CClientSocket

CClientSocket::CClientSocket()
{
}

CClientSocket::~CClientSocket()
{
}


// CClientSocket ��� �Լ�


void CClientSocket::OnClose(int nErrorCode)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	this->ShutDown();
	this->Close();
	AfxMessageBox(_T("������ ������ �����Ͽ����ϴ�"));
	::PostQuitMessage(0);
	CSocket::OnClose(nErrorCode);
}


void CClientSocket::OnReceive(int nErrorCode)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	TCHAR szBuff[1024];
	int nRead = Receive(szBuff, sizeof(szBuff));
	switch (nRead) {
	case 0: this->Close(); break;
	case SOCKET_ERROR: if (GetLastError() != WSAEWOULDBLOCK) {
		AfxMessageBox(_T("Error occured"));
		this->Close();
	}break;
	default: 
		szBuff[nRead] = _T('\0');
		CWordChainGameDlg* pMain = (CWordChainGameDlg*)AfxGetMainWnd();
		switch (szBuff[0]) {
		case '0':	//ȸ������
			break;
		case '1':	//�α���
			break;
		case '2':	//�غ�
			break;
		case '3':	//���� ����
			break;
		case '4':	//���� ����
			break;
		default:
			break;
		}


		if (szBuff[0] == '0') {	//ȸ������
			CString name;
			CString msg;
			AfxExtractSubString(name, szBuff, 1, ' ');
			msg.Format(_T("%s ���� ȸ������ �ϼ̽��ϴ�\r\n"), name);
			pMain->m_ctrlEdit.ReplaceSel(msg);
		}
		else if (szBuff[0] == '1') {	//�α���
			CString name;
			CString msg;
			AfxExtractSubString(name, szBuff, 1, ' ');
			msg.Format(_T("%s ���� �α��� �ϼ̽��ϴ�\r\n"), name);
			pMain->m_ctrlEdit.ReplaceSel(msg);
		}
		else if(szBuff[0] == '2'){	//�غ� ���� �޽���
			CString name;
			CString ready;
			CString msg;
			AfxExtractSubString(name, szBuff, 1, ' ');
			AfxExtractSubString(ready, szBuff, 2, ' ');
			if (ready == 'y') {
				msg.Format(_T("%s ���� �غ� �Ͽ����ϴ� \r\n"), name);
			}
			else {
				msg.Format(_T("%s ���� �غ���� �Ͽ����ϴ� \r\n"), name);
			}
			pMain->m_ctrlEdit.ReplaceSel(msg);
		}
		else if (szBuff[0] == '3') {	//���� ���� ���
			CString str1;
			CString str2;
			CString str3;
			CString str4;
			CString msg;
			AfxExtractSubString(str1, szBuff, 1, ' ');
			AfxExtractSubString(str2, szBuff, 2, ' ');
			AfxExtractSubString(str3, szBuff, 3, ' ');
			AfxExtractSubString(str4, szBuff, 4, ' ');
			msg.Format(_T("%s %s %s %s"), str1, str2, str3, str4);
			pMain->m_ctrlEdit.ReplaceSel(msg);
		}
		else {	//���� ����

			CInGameDlg game;
			game.DoModal();

			game.DestroyWindow();

		}
	}
	CSocket::OnReceive(nErrorCode);
}
