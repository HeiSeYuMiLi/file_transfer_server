#pragma once
#include <string>

/*
* ����http���ĸ�ʽ
* �������������¹涨��
* ��һ�У���������(���������ϴ�����)\r\n
* �ڶ��У�File-Type=�ļ�����\r\n
* �����У�Content-Length=����\r\n
* �����У�\r\n
* �����У�����
*
* ��Ӧ���������¹涨��
* ��һ�У�Ӧ������(����Ӧ���ϴ�Ӧ��)\r\n
* �ڶ��У�File-Type=�ļ�����\r\n
* �����У�Content-Length=����\r\n
* �����У�\r\n
* �����У�����
*/


namespace file_server {

	const int HEAD_LENGTH = 2;
	const int MAX_LENGTH = 1024;

	struct Request
	{

		void Clear() {
			reqType.clear();
			fileType.clear();
			contentLength = 0;
			content.clear();
		}

		std::string reqType;
		std::string fileType;
		int contentLength;
		std::string content;
	};

}