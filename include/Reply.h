#pragma once
#include <vector>
#include <memory>
#include <boost/asio.hpp>

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

	struct Reply {

		std::shared_ptr<boost::asio::streambuf> toBuffer() {

			std::shared_ptr<boost::asio::streambuf> replyBuf(new boost::asio::streambuf);
			replyBuf->prepare(content.size()+100);
			std::ostream replyStream(replyBuf.get());

			replyStream << repType << "\r\n";
			replyStream << "File-Type=" << fileType << "\r\n";
			replyStream << "Content-Length=" << contentLength << "\r\n\r\n";
			replyStream.write(content.data(), content.size());

			return replyBuf;
		}

		void Clear() {
			repType.clear();
			fileType.clear();
			contentLength.clear();
			content.clear();
		}

		std::string repType;
		std::string fileType;
		std::string contentLength;
		std::vector<char> content;
	};

}