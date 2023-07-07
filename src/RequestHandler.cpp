#include <fstream>
#include <boost/asio.hpp>
#include "RequestHandler.h"
#include "Logger.h"


namespace file_server {

	bool RequestHandler::HandleRequest(Request& req, Reply& rep) {
		if (req.reqType == "download") {
			// ��filePath���м򵥵Ĵ���
			auto it = std::find(req.content.rbegin(), req.content.rend(), '/');
			size_t index = 0;
			if (it != req.content.rend())
				index = req.content.size() - std::distance(req.content.rbegin(), it);
			std::string fileName(req.content.begin() + index, req.content.end());
			Logger(FILE_LOCATION, log_level::info, "The file being read is " + fileName + ".");

			std::ifstream is("../testfile/" + fileName, std::ifstream::in | std::ios::binary);
			if (is)
			{
				rep.Clear();
				rep.repType = "download";
				rep.fileType = req.fileType;

				// ��ȡ�ļ�����
				is.seekg(0, is.end);
				long length = is.tellg();
				rep.content.resize(length);

				// תΪ�����ֽ���
				long newLength = boost::asio::detail::socket_ops::host_to_network_long(length);
				// �洢����
				rep.contentLength = std::to_string(newLength);

				is.seekg(0, is.beg);
				is.read(rep.content.data(), length);
				req.Clear();
			}
			else {
				Logger(FILE_LOCATION, log_level::error, "File not found!");
				return false;
			}
			is.close();
		}
		else if (req.reqType == "upload") {
			static int k = 1000;
			std::string fileName = "../testfile/" + std::to_string(k) + "." + req.fileType;
			std::ofstream out(fileName, std::ios::binary);
			if (!out)
			{
				Logger(FILE_LOCATION, log_level::error, "File not found!");
				return false;
			}
			else
			{
				// ����������ļ�
				out.write(req.content.data(), req.contentLength);
				out.close();
			}
			k++;
			Logger(FILE_LOCATION, log_level::info, "�ɹ������ļ�");
			return false;
		}
		else {

		}


		

		return true;
	}

}
