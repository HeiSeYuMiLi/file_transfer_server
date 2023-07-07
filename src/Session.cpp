#include "Session.h"
#include "Server.h"
#include <string>
#include "Logger.h"

namespace file_server {

	Session::Session(boost::asio::io_context& ioc, Server* server) :
		_sock(ioc), _server(server) {
		boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
		_uuid = boost::uuids::to_string(a_uuid);
	}

	Session::~Session() {
		Logger(FILE_LOCATION, log_level::warn, "Session destruct!");
	}

	void Session::Start() {
		Logger(FILE_LOCATION, log_level::info, "Session start:");

		// 读取请求
		// 读写接口会自动调用 streambuf::prepare 函数给 streambuf 分配足够的空间
		boost::asio::async_read_until(_sock, _buffer, "\r\n\r\n", std::bind(&Session::ReadHeadHandler,
			this, std::placeholders::_1, std::placeholders::_2, SharedSelf()));
		// 这样的读取方式，即是读到了分割符\r\n时，也有可能会多读取分隔符后面的数据
		// 因为当你调用async_read_until函数时，它会从底层的socket或者其他设备中读取数据，
		// 并将其存储到streambuf中，直到遇到指定的分隔符或者发生错误。这个过程是异步的，也就是说，你不能确定它会一次性读取多少数据。
	}

	tcp::socket& Session::GetSocket() {
		return _sock;
	}

	std::string& Session::GetUuid() {
		return _uuid;
	}

	std::shared_ptr<Session> Session::SharedSelf() {
		return shared_from_this();
	}

	void Session::ReadHeadHandler(const boost::system::error_code& ec,
		size_t bytes_transferred, std::shared_ptr<Session> shared_self) {
		if (!ec) {
			std::iostream requestStream(&_buffer);
			std::string requestHeadLine;
			size_t index = 0;

			// 读取请求类型
			requestStream >> _request.reqType;
			Logger(FILE_LOCATION, log_level::info, "This is a " + _request.reqType + " request.");

			// 读取文件类型
			requestStream >> requestHeadLine;
			index = requestHeadLine.find('=');
			_request.fileType = requestHeadLine.substr(index + 1);
			// 读取正文长度
			requestStream >> requestHeadLine;
			index = requestHeadLine.find('=');
			_request.contentLength = boost::asio::detail::socket_ops::
				network_to_host_long(atoi(requestHeadLine.substr(index + 1).data()));

			int length = 0;
			if (_buffer.size() < _request.contentLength)
				length = _request.contentLength - _buffer.size();

			boost::asio::async_read(_sock, _buffer, boost::asio::transfer_at_least(length),
				std::bind(&Session::ReadContentHandler, this, std::placeholders::_1,
					std::placeholders::_2, SharedSelf()));
		}
		else {
			Logger(FILE_LOCATION, log_level::error, "Handler read type error, error is " + ec.message() + "!");
			Close();
			_server->ClearSession(_uuid);
		}
	}

	void Session::ReadContentHandler(const boost::system::error_code& ec,
		size_t bytes_transferred, std::shared_ptr<Session> shared_self) {
		if (!ec) {
			// 去掉\r\n\r\n
			for (int i = 0; i < 4; i++) {
				if (_buffer.sgetc() == '\r' || _buffer.sgetc() == '\n')
					_buffer.sbumpc();
			}

			_request.content.resize(_request.contentLength);
			_buffer.sgetn(_request.content.data(), _request.contentLength);

			if (_reqh.HandleRequest(_request, _reply)) {
				Send();
			}
			else {
				Close();
				_server->ClearSession(_uuid);
				return;
			}

			boost::asio::async_read_until(_sock, _buffer, "\r\n\r\n", std::bind(&Session::ReadHeadHandler,
				this, std::placeholders::_1, std::placeholders::_2, SharedSelf()));
		}
		else {
			Logger(FILE_LOCATION, log_level::error, "Handler read head error, error is " + ec.message() + "!");
			Close();
			_server->ClearSession(_uuid);
		}
	}

	void Session::WriteHandler(const boost::system::error_code& ec,
		std::shared_ptr<Session> shared_self) {
		if (!ec) {
			std::lock_guard<std::mutex> lock(_mutex);
			_repQue.pop();
			if (!_repQue.empty()) {
				boost::asio::streambuf& repFile = *_repQue.front();
				boost::asio::async_write(_sock, repFile, std::bind(&Session::WriteHandler,
					this, std::placeholders::_1, shared_self));
			}
		}
		else {
			Logger(FILE_LOCATION, log_level::error, "Handler write error, error is " + ec.message() + "!");
			Close();
			_server->ClearSession(_uuid);
		}
	}

	void Session::Send() {
		bool pending = false;
		std::lock_guard<std::mutex> lock(_mutex);
		if (_repQue.size() > 0) {
			pending = true;
		}
		_repQue.push(_reply.toBuffer());
		if (pending) {
			return;
		}
		boost::asio::streambuf& repFile = *_repQue.front();
		boost::asio::async_write(_sock, repFile, std::bind(&Session::WriteHandler,
			this, std::placeholders::_1, SharedSelf()));
		Logger(FILE_LOCATION, log_level::info, "File sending completed.");
	}

	void Session::Close() {
		_sock.close();
	}

}