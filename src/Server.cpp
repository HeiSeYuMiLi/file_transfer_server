#include "Server.h"
#include "Session.h"
#include <iostream>

namespace file_transfer_server {

	Server::Server(boost::asio::io_context& ioc,short port):_ioc(ioc),_acceptor(_ioc,
		tcp::endpoint(tcp::v4(),port)) {
		std::cout << "Server start success, listen on port : " << port << std::endl;
		Start();
	}

	void Server::Start() {
		std::shared_ptr<Session> new_session = std::make_shared<Session>(_ioc, this);
		_acceptor.async_accept(new_session->GetSocket(), std::bind(&Server::AcceptHandler,
			this, new_session, std::placeholders::_1));
	}

	void Server::ClearSession(std::string uuid) {
		_sessions.erase(uuid);
	}

	void Server::AcceptHandler(std::shared_ptr<Session> new_session, const boost::system::error_code& ec) {
		if (!ec) {
			new_session->Start();
			_sessions.insert(std::make_pair(new_session->GetUuid(),new_session));
		}
		else {
			std::cout << "Session accept failed, error is " << ec.what() << std::endl;
		}

		Start();
	}
}