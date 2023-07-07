#pragma once
#include <string>

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