// ChildSocket.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Server.h"
#include "ChildSocket.h"
#include "ServerDlg.h"
#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/uri.h>
#include <cpprest/http_headers.h>
#include <cpprest/json.h>
#include <string.h>
#include "tinyxml2.h"

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

// CChildSocket

CChildSocket::CChildSocket()
{
	m_prevword = _T("");
	m_isfirst = 1;
	m_istrue = 0;
	m_istrue = 0;
}

CChildSocket::~CChildSocket()
{
}

auto GET_request(const wchar_t* query_word) {
	std::wcout << uri_builder(U("search.do")).append_query(U("q"), query_word).append_query(U("key"), "04765A233874C1C149B09678B3343868").to_string() << std::endl;
	const char* word_true;
	int m_isok = 0;
	auto requestJson = http_client(U("https://stdict.korean.go.kr/api/"))
		.request(methods::GET, uri_builder(U("search.do")).append_query(U("q"), query_word).append_query(U("key"), "04765A233874C1C149B09678B3343868").to_string())
		.then([&](http_response response) mutable
			{
				if (response.status_code() != 200)
				{
					throw std::runtime_error("Returned " + std::to_string(response.status_code()));
				}

				std::string data = response.extract_utf8string().get();
				//std::cout << data << std::endl;
				const char* xml_data = data.c_str();
				tinyxml2::XMLDocument doc;
				doc.Parse(xml_data);
				tinyxml2::XMLElement* titleElement = doc.FirstChildElement("channel")->FirstChildElement("total");
				word_true = titleElement->GetText();
				std::cout << word_true << std::endl;
				m_isok = atoi(word_true);
				return m_isok;
			}).wait();
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
	pMain->m_numlogged--;	//�α����� ����� -1

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
			temp.Format(_T("1 0 0 "));	//Ÿ�Ӿƿ��� ���� �� ����, ���� ���� �ƴϴ�, �Ѱ��� �ð� ���� x
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
			

			//�ѱ� 2byte ���� 1byte

			//��ũ Ȯ��
			prev_R = m_prevword.Right(2);
			now_L = word.Left(2);

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
				temp.Format(_T("0 0 "));	//Ÿ�Ӿƿ��� ���� �� ����(�ֱ�)�ƴϴ�, ���ӽ��� �ƴϴ�, �Ѱ��ӽð� ���� x
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