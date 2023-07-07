#pragma once
#include <boost/asio.hpp>
#include <map>
#include <memory>

using boost::asio::ip::tcp;

namespace file_server {

	class Session;
	class Server
	{
	public:
		Server(boost::asio::io_context&, short port);
		Server(const Server&) = delete;
		Server& operator=(const Server&) = delete;

		
		void ClearSession(std::string uuid);
	private:
		void Start();
		void AcceptHandler(std::shared_ptr<Session> new_session, const boost::system::error_code& ec);

		boost::asio::io_context& _ioc;
		tcp::acceptor _acceptor;
		std::map<std::string, std::shared_ptr<Session>> _sessions;
	};

}


