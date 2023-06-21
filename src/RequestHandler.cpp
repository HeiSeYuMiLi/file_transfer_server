#include "RequestHandler.h"
#include <iostream>
#include <fstream>

namespace file_transfer_server {

	bool RequestHandler::HandlerRequest(const std::vector<char>& data, std::vector<char>& msgNode) {
		std::ifstream is("./LeBronJames.jpg",
			std::ifstream::in | std::ios::binary);
		if (is)
		{
			msgNode.clear();

			// 获取文件长度
			is.seekg(0, is.end);
			short length = is.tellg();

			// 存储长度
			char* p = reinterpret_cast<char*>(&length); //将short值的地址转换为char指针
			msgNode.resize(length + HEAD_LENGTH);
			msgNode[0] = p[0]; //将short值的低8位复制到v的第一个元素
			msgNode[1] = p[1]; //将short值的高8位复制到v的第二个元素

			is.seekg(0, is.beg);
			is.read(&msgNode[HEAD_LENGTH], length);
			is.close();
		}
		else
			return false;
		return true;
	}

}
