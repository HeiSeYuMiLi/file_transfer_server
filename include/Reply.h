#pragma once
#include <vector>
#include <memory>
#include <boost/asio.hpp>

/*
* 根据http报文格式
* 对请求报文有如下规定：
* 第一行：请求类型(下载请求、上传请求)\r\n
* 第二行：File-Type=文件类型\r\n
* 第三行：Content-Length=长度\r\n
* 第四行：\r\n
* 第五行：正文
*
* 对应答报文有如下规定：
* 第一行：应答类型(下载应答、上传应答)\r\n
* 第二行：File-Type=文件类型\r\n
* 第三行：Content-Length=长度\r\n
* 第四行：\r\n
* 第五行：正文
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