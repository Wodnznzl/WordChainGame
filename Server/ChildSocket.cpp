// ChildSocket.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Server.h"
#include "ChildSocket.h"
#include "ServerDlg.h"

// CChildSocket

CChildSocket::CChildSocket()
{
	m_prevword = _T("");
	m_isfirst = 1;
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
	//map ���� ����
	CString name;
	name.Format(_T("%s"), pMain->m_usermap[uPort]);
	pMain->m_usermap.erase(uPort);
	pMain->m_ready.erase(name);
	pMain->m_mapScore.erase(name);

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

		CServerDlg* pDlg = (CServerDlg*)AfxGetMainWnd();
		pDlg->m_ctrlEdit.ReplaceSel(str);

		if (szBuffer[0] == '0') {
			//ȸ������
			CString ID, PASSWORD;
			AfxExtractSubString(ID, szBuffer, 1, ' ');
			AfxExtractSubString(PASSWORD, szBuffer, 2, ' ');
			pDlg->SignUp(ID, PASSWORD, uPortNumber);
		}
		else if (szBuffer[0] == '1') {
			//�α���
			CString ID, PASSWORD;
			AfxExtractSubString(ID, szBuffer, 1, ' ');
			AfxExtractSubString(PASSWORD, szBuffer, 2, ' ');
			pDlg->Login(ID, PASSWORD, uPortNumber);
		}
		else if (szBuffer[0] == '2') {
			//�غ� ����
			CString status;
			CString username;
			CString msg;
			AfxExtractSubString(status, szBuffer, 1, ' ');

			if (status == "ready") {
				//�غ�
				username = pDlg->m_usermap.at(uPortNumber);
				msg.Format(_T("2 %s y \r\n"), username);
				pDlg->Ready(1, username, msg);
			}
			else {
				//�غ� ���
				username = pDlg->m_usermap.at(uPortNumber);
				msg.Format(_T("2 %s n \r\n"), username);
				pDlg->Ready(0, username, msg);
			}
		}
		else if (szBuffer[0] == '5') {
			//Ÿ�� �ƿ�
			CString username;
			CString time, turn, query, temp;
			username = pDlg->m_usermap.at(uPortNumber);

			//���� ���
			if (pDlg->m_mapScore.at(username) < 5) {	//0���� ���ھ� ���� x
				pDlg->m_mapScore.at(username) = 0;
			}
			else {
				pDlg->m_mapScore.at(username) -= 5;	//���� ���� ���ϱ�
			}
			
			//�� �ѱ��
			
			for (auto it = pDlg->m_usermap.begin(); it != pDlg->m_usermap.end(); it++) {
				if (username != it->second) {
					turn = it->second;
					break;
				}
			}

			time.Format(_T("5000"));
			query.Format(_T("5 %s %s "), turn, time);
			for (auto it = pDlg->m_mapScore.begin(); it != pDlg->m_mapScore.end(); it++) {
				temp.Format(_T("%s %d "), it->first, it->second);
				query.Append(temp);
			}
			temp.Format(_T("1 "));	//Ÿ�Ӿƿ��� ���� �� ����
			query.Append(temp);
			temp.Format(_T("\r\n"));
			query.Append(temp);
			m_pListenSocket->Broadcast(query);	//���� �� ������� �޽��� ������

			pDlg->m_ctrlEdit.ReplaceSel(query);
		}
		else if (szBuffer[0] == '6') {	//�ܾ� ����
			
			CString word;
			CString query;
			CString prev_R;
			CString now_L;
			int length;
			int islink = 1;
			int wrong_char = 0;	//�߸��� ���ڰ� ���� �� 1
			int success;

			AfxExtractSubString(word, szBuffer, 1, ' ');

			//�߸��� ���� �ִ��� Ȯ�� �߰� �ʿ�
			wrong_char = 0;
			length = word.GetLength();

			//�ܾ� Ȯ��


			//��ũ Ȯ��
			prev_R = m_prevword.Right(1);
			now_L = word.Left(1);
			if (prev_R == now_L || m_isfirst == 1) {
				islink = 1;
			}
			else {
				islink = 0;
			}

			//rest Ȯ���߰�

			
			if (islink == 1 & length != 0 & wrong_char == 0) {
				success = 1;
			}
			else {
				success = 0;
			}

			//rest Ȯ���߰�




			//������ 1 ���н� 0
			query.Format(_T("6 %s %d \r\n"), word, success);

			m_pListenSocket->Broadcast(query);
			pDlg->m_ctrlEdit.ReplaceSel(query);
			
			if (success == 1) {	//����
				m_isfirst = 0;
				m_prevword = word;	//prev�� word ����
				//�� �ѱ�
				CString username;
				CString time, turn, temp;
				username = pDlg->m_usermap.at(uPortNumber);

				//���� �ֱ�
				pDlg->m_mapScore.at(username) += 10;	//�߰��� ���� ���ϱ�

				//�� �ѱ��

				for (auto it = pDlg->m_usermap.begin(); it != pDlg->m_usermap.end(); it++) {
					if (username != it->second) {
						turn = it->second;
						break;
					}
				}

				time.Format(_T("5000"));	//�ð� ���ϱ�
				query.Format(_T("5 %s %s "), turn, time);
				for (auto it = pDlg->m_mapScore.begin(); it != pDlg->m_mapScore.end(); it++) {
					temp.Format(_T("%s %d "), it->first, it->second);
					query.Append(temp);
				}
				temp.Format(_T("0 "));	//Ÿ�Ӿƿ��� ���� �� ����(�ֱ�)�ƴϴ�
				query.Append(temp);
				temp.Format(_T("\r\n"));
				query.Append(temp);
				m_pListenSocket->Broadcast(query);	//���� �� ������� �޽��� ������

				pDlg->m_ctrlEdit.ReplaceSel(query);
			}
			else {	//����

				
			}

		}

		break;
	}
	CSocket::OnReceive(nErrorCode);
}