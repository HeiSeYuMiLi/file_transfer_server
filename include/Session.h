#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <queue>
#include <vector>
#include <mutex>
#include "Request.h"
#include "RequestHandler.h"

using boost::asio::ip::tcp;

namespace file_server {

	class Server;
	class Session : public std::enable_shared_from_this<Session>
	{
	public:
		Session(boost::asio::io_context& ioc, Server* server);
		Session(const Session&) = delete;
		Session& operator=(const Session&) = delete;
		~Session();

		void Start();
		tcp::socket& GetSocket();
		std::string& GetUuid();

	private:
		void ReadHeadHandler(const boost::system::error_code& ec,
			size_t bytes_transferred, std::shared_ptr<Session> shared_self);
		void ReadContentHandler(const boost::system::error_code& ec,
			size_t bytes_transferred, std::shared_ptr<Session> shared_self);
		void WriteHandler(const boost::system::error_code& ec, std::shared_ptr<Session> shared_self);
		void Send();
		std::shared_ptr<Session> SharedSelf();
		void Close();

		tcp::socket _sock;
		std::string _uuid;
		Server* _server;

		RequestHandler _reqh;

		// 读取数据的流对象
		boost::asio::streambuf _buffer;

		// 存储请求和响应的结构
		Request _request;
		Reply _reply;

		std::mutex _mutex;
		std::queue<std::shared_ptr<boost::asio::streambuf>> _repQue;
	};

}

