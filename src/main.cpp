#include <boost/thread.hpp>
#include "Server.h"
#include "Logger.h"

using namespace file_server;

int main() {
	try {

		boost::asio::io_context ioc;
		Server s(ioc, 10086);

		// ����һ���źż��ϣ����ڲ���SIGINT��SIGTERM�ź�
		boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
		// �첽�ȴ��źţ�ʹ��lambda���ʽ��Ϊ�ص�����
		signals.async_wait([&ioc](auto, auto) {
			ioc.stop(); // ֹͣio_context����
			});

		// ����һ���������󣬷�ֹio_context������ǰ�˳�
		boost::asio::io_context::work work(ioc);

		// ����һ���߳��飬��������io_context����
		boost::thread_group threads;
		for (int i = 0; i < 4; ++i)
		{
			// ����һ���µ��̣߳�������һ���ɵ��ö�����Ϊ�̺߳�����
			// ����ʹ��boost::bind����io_context�����run��������ָ�룬�൱����ÿ���̶߳�����io_context�����run����
			threads.create_thread(boost::bind(&boost::asio::io_context::run, &ioc)); 
		}

		/*
		* ����߳�������������io_context����ģ�
		* ��io_context�������������������첽�����ģ������������ӡ���д���ݵȡ�
		* ��������߳���ʵ�����Ƕ����е�����ͨ�Ž��д���ģ���������һ�������������ϵĶ�д��
		* 
		* ÿһ���߳��е�io_context.runҲ����ֻ����һ�����������ӵ��첽������
		* ʵ���ϣ����е��̶߳�����ͬһ��io_context����
		* �������Ƕ����Դ����κ��첽�������������ĸ����������ӻ�ͻ������ӵġ�
		* io_context������Զ������첽���������е��̣߳���ʵ�ָ��ؾ���Ͳ�������
		*/

		threads.join_all(); // �ȴ������߳��˳�
	}
	catch (std::exception& e) {
		Logger(FILE_LOCATION, log_level::error, "Error is " + std::string(e.what()) + "!");
	}
}

