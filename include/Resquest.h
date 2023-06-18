#pragma once

/*
* 规定
* 
* 客户端发送的请求 分为两种：下载、上传
*	格式：
*		第一行：请求类型\r长度\r\n
*		第二行：文件名称\r\n
* 
* 服务端接收请求后，对请求进行处理
*		下载请求：将对应文件传给客户端
*		上传请求：对客户端发送同意响应
*	格式：
*		第一行：响应状态\r\n
*		第二行：正文\r\n
*/

/*
* 本服务器的主要功能就是实现客户端与服务器之间的文件互传
* 客户端可以上传和下载文件
* 可以一次下载多个文件
*/

namespace file_transfer_server {

	const int MAX_LENGTH = 1024;

	class Request
	{
	public:
		Request(char* msg, short length) :_len(length) {
			//_data = new char[length+1];
			_data = new char[length];
			memcpy(_data, msg, length);
			//_data[_len] = '\0';
		}
		~Request() {
			delete[] _data;
		}

		void Clear() {
			memset(_data, 0, _len);
		}
	private:
		int _len;
		char* _data;
	};

}
