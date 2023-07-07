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

		// ��ȡ����
		// ��д�ӿڻ��Զ����� streambuf::prepare ������ streambuf �����㹻�Ŀռ�
		boost::asio::async_read_until(_sock, _buffer, "\r\n\r\n", std::bind(&Session::ReadHeadHandler,
			this, std::placeholders::_1, std::placeholders::_2, SharedSelf()));
		// �����Ķ�ȡ��ʽ�����Ƕ����˷ָ��\r\nʱ��Ҳ�п��ܻ���ȡ�ָ������������
		// ��Ϊ�������async_read_until����ʱ������ӵײ��socket���������豸�ж�ȡ���ݣ�
		// ������洢��streambuf�У�ֱ������ָ���ķָ������߷�����������������첽�ģ�Ҳ����˵���㲻��ȷ������һ���Զ�ȡ�������ݡ�
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

			// ��ȡ��������
			requestStream >> _request.reqType;
			Logger(FILE_LOCATION, log_level::info, "This is a " + _request.reqType + " request.");

			// ��ȡ�ļ�����
			requestStream >> requestHeadLine;
			index = requestHeadLine.find('=');
			_request.fileType = requestHeadLine.substr(index + 1);
			// ��ȡ���ĳ���
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
			// ȥ��\r\n\r\n
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