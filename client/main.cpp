#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <vector>
#include <string>
using boost::asio::ip::tcp;
const int MAX_LENGTH = 1024;
const int REQ_HEAD_LENGTH = 2;
const int REP_HEAD_LENGTH = 4;

/*
void PackageRequest(const std::string& filePath, std::vector<char>& msgNode) {
	msgNode.clear();
	msgNode.shrink_to_fit();
	msgNode.resize(REQ_HEAD_LENGTH);

	short length = boost::asio::detail::socket_ops::host_to_network_short(filePath.length());

	char* p = reinterpret_cast<char*>(&length); //��shortֵ�ĵ�ַת��Ϊcharָ��
	msgNode[0] = p[0]; //��shortֵ�ĵ�8λ���Ƶ�v�ĵ�һ��Ԫ��
	msgNode[1] = p[1]; //��shortֵ�ĸ�8λ���Ƶ�v�ĵڶ���Ԫ��

	for (auto i : filePath)
		msgNode.push_back(i);

	for (auto i : msgNode)
		std::cout << i;
	std::cout << std::endl;
}

int main()
{
	try {
		// ���������ķ���
		boost::asio::io_context ioc;
		// ����endpoint
		tcp::endpoint remote_ep(boost::asio::ip::address::from_string("127.0.0.1"), 10086);
		tcp::socket sock(ioc);
		boost::system::error_code error = boost::asio::error::host_not_found;
		sock.connect(remote_ep, error);
		if (error) {
			std::cout << "connect failed, code is " << error.value() << " error msg is " << error.message();
			return 0;
		}

		std::string file1 = "F:/C++����/asio_work/Mproject/file_transfer_server/testfile/assignment.doc";
		std::string file2 = "F:/C++����/asio_work/Mproject/file_transfer_server/testfile/beauty.jpg";
		std::string file3 = "F:/C++����/asio_work/Mproject/file_transfer_server/testfile/LeBronJames.jpg";

		std::vector<std::string> vec;
		vec.push_back(file1);
		vec.push_back(file2);
		vec.push_back(file3);

		std::vector<char> request;
		std::vector<char> reply;

		for (auto i : vec) {
			PackageRequest(i, request);
			boost::asio::write(sock, boost::asio::buffer(request));
			std::cout << "���ݷ��ͳɹ�" << std::endl;
		}

		int k = 0;
		std::vector<std::string> fileName{ "assignment.doc","beauty.jpg","LeBronJames.jpg" };

		while (k < vec.size()) {
			char replyHead[REP_HEAD_LENGTH];
			boost::asio::read(sock, boost::asio::buffer(replyHead, REP_HEAD_LENGTH));
			long msgLen = 0;
			char* q = reinterpret_cast<char*>(&msgLen);
			q[0] = replyHead[0];
			q[1] = replyHead[1];
			q[2] = replyHead[2];
			q[3] = replyHead[3];
			msgLen = boost::asio::detail::socket_ops::network_to_host_long(msgLen);
			std::cout << "length = " << msgLen << std::endl;

			reply.clear();
			reply.shrink_to_fit();
			reply.resize(msgLen);
			size_t msgLength = boost::asio::read(sock, boost::asio::buffer(reply));
			std::cout << "�������������ݳ���" << msgLength << std::endl;

			std::ofstream out(fileName[k], std::ios::binary);
			if (!out)
			{
				std::cout << "�ļ����ܴ�" << std::endl;
				return 0;
			}
			else
			{
				//2.����������ļ�
				std::cout << "�ļ���С��" << msgLen << std::endl;
				out.write(&reply[0], msgLen);
				out.close();
			}
			k++;
		}

	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	return 0;
}
*/

/*
* ����http���ĸ�ʽ
* �������������¹涨��
* ��һ�У���������(���������ϴ�����)\r\n
* �ڶ��У�File-Type=�ļ�����\r\n
* �����У�Content-Length=����\r\r\n
* �����У�����
*
* ��Ӧ���������¹涨��
* ��һ�У�Ӧ������(����Ӧ���ϴ�Ӧ��)\r\n
* �ڶ��У�File-Type=�ļ�����\r\n
* �����У�Content-Length=����\r\r\n
* �����У�����
*/

struct FileMsg {

	void Clear() {
		repType.clear();
		fileType.clear();
		fileLength = 0;
		content.clear();
	}

	std::string repType;
	std::string fileType;
	long fileLength;
	std::vector<char> content;
};

std::vector<char> ReadFromFile(const std::string& filePath) {
	std::vector<char> fileMsg;
	size_t index = filePath.find_last_of('/');
	if (index != std::string::npos) {
		std::string fileName = filePath.substr(index + 1);
		std::ifstream in(fileName, std::fstream::in | std::ios::binary);

		if (!in) {
			std::cout << "�ļ����ܴ�" << std::endl;
			return fileMsg;
		}

		in.seekg(0, in.end);
		long length = in.tellg();
		fileMsg.resize(length);

		in.seekg(0, in.beg);
		in.read(fileMsg.data(), length);
		in.close();
	}
	else {
		return fileMsg;
	}

	return fileMsg;
}

void WriteToFile(const FileMsg& fileMsg) {
	static int k = 1000;
	std::string fileName = std::to_string(k) + "." + fileMsg.fileType;
	std::ofstream out(fileName, std::ios::binary);
	if (!out)
	{
		std::cout << "�ļ����ܴ�" << std::endl;
		return;
	}
	else
	{
		// ����������ļ�
		std::cout << "�ļ���С��" << fileMsg.fileLength << std::endl;
		out.write(fileMsg.content.data(), fileMsg.fileLength);
		out.close();
	}
	k++;
}

// ��������
void SendRequest(std::string reqType, const std::string& reqMsg, tcp::socket& sock) {
	boost::asio::streambuf request;
	std::ostream reqStream(&request);
	size_t index = reqMsg.find_last_of('.');
	if (index != std::string::npos) {
		std::string fileType = reqMsg.substr(index + 1);

		if (reqType == "download") {
			reqStream << reqType << "\r\n";
			reqStream << "File-Type=" << fileType << "\r\n";
			reqStream << "Content-Length=" << boost::asio::detail::socket_ops::
				host_to_network_long(reqMsg.length()) << "\r\n\r\n";
			reqStream << reqMsg.data();
		}
		else if (reqType == "upload") {
			std::vector<char> file = ReadFromFile(reqMsg);
			request.prepare(file.size() + 100);
			std::cout << "�����С" << request.capacity() << std::endl;
			reqStream << reqType << "\r\n";
			reqStream << "File-Type=" << fileType << "\r\n";
			std::cout << "ԭ������" << file.size() << std::endl;
			int length = boost::asio::detail::socket_ops::
				host_to_network_long(file.size());
			std::cout << "ת��󳤶�" << length << std::endl;
			reqStream << "Content-Length=" << length << "\r\n\r\n";
			reqStream.write(file.data(), file.size());
		}
		else
			return;

		std::cout << "׼�����ͣ�request�к���" << request.size() << "��Ԫ��" << std::endl;

		boost::asio::write(sock, request);
	}
	else {
		return;
	}
}

// ������Ӧ
void RecReply(tcp::socket& sock) {
	boost::asio::streambuf reply;
	std::iostream repStream(&reply);
	std::string replyHeadLine;
	FileMsg fileMsg;

	// ����Ӧ����
	boost::asio::read_until(sock, reply, "\r\n\r\n");
	std::cout << "reply�к���" << reply.size() << "��Ԫ��" << std::endl;

	repStream >> fileMsg.repType;
	std::cout << "�յ�����Ӧ����" << fileMsg.repType << std::endl;
	// ���ļ�����
	repStream >> replyHeadLine;
	size_t index = replyHeadLine.find('=');
	fileMsg.fileType = replyHeadLine.substr(index + 1);
	std::cout << "�յ����ļ�����" << fileMsg.fileType << std::endl;
	// ���ļ�����
	repStream >> replyHeadLine;
	index = replyHeadLine.find('=');
	fileMsg.fileLength = boost::asio::detail::socket_ops::
		network_to_host_long(atoi(replyHeadLine.substr(index + 1).data()));
	std::cout << "�յ����ļ�����" << fileMsg.fileLength << std::endl;
	// ������
	std::cout << "reply�к���" << reply.size() << "��Ԫ��" << std::endl;
	int length = 0;
	if (reply.size() < fileMsg.fileLength)
		length = fileMsg.fileLength - reply.size();
	boost::asio::read(sock, reply, boost::asio::transfer_at_least(length));
	std::cout << "reply�к���" << reply.size() << "��Ԫ��" << std::endl;
	// ȥ��\r\n\r\n
	for (int i = 0; i < 4; i++)
		reply.sbumpc();
	fileMsg.content.resize(fileMsg.fileLength);
	reply.sgetn(fileMsg.content.data(), fileMsg.fileLength);
	std::cout << "��ʼ���ļ�" << std::endl;
	WriteToFile(fileMsg);
	fileMsg.Clear();
}




int main()
{
	try {
		// ���������ķ���
		boost::asio::io_context ioc;
		// ����endpoint
		tcp::endpoint remote_ep(boost::asio::ip::address::from_string("127.0.0.1"), 10086);
		tcp::socket sock(ioc);
		boost::system::error_code error = boost::asio::error::host_not_found;
		sock.connect(remote_ep, error);
		if (error) {
			std::cout << "connect failed, code is " << error.value() << " error msg is " << error.message();
			return 0;
		}

		std::vector<std::string> vec;
		vec.emplace_back("F:/C++����/asio_work/Mproject/file_transfer_server/testfile/beauty.jpg");
		vec.emplace_back("F:/C++����/asio_work/Mproject/file_transfer_server/testfile/assignment.doc");
		vec.emplace_back("F:/C++����/asio_work/Mproject/file_transfer_server/testfile/LeBronJames.jpg");
		vec.emplace_back("F:/C++����/asio_work/Mproject/Client/Client/������.xlsx");

		// ��������
		for (int i = 0; i < vec.size()-1; i++) {

			SendRequest("download", vec[i], sock);
			RecReply(sock);
		}
		// �����ϴ�
		SendRequest("upload", vec[3], sock);


	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	return 0;
}