#include <boost/thread.hpp>
#include "Server.h"
#include "Logger.h"

using namespace file_server;

int main() {
	try {

		boost::asio::io_context ioc;
		Server s(ioc, 10086);

		// 创建一个信号集合，用于捕获SIGINT和SIGTERM信号
		boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
		// 异步等待信号，使用lambda表达式作为回调函数
		signals.async_wait([&ioc](auto, auto) {
			ioc.stop(); // 停止io_context对象
			});

		// 创建一个工作对象，防止io_context对象提前退出
		boost::asio::io_context::work work(ioc);

		// 创建一个线程组，用于运行io_context对象
		boost::thread_group threads;
		for (int i = 0; i < 4; ++i)
		{
			// 创建一个新的线程，并传入一个可调用对象作为线程函数，
			// 这里使用boost::bind来绑定io_context对象的run方法和其指针，相当于让每个线程都运行io_context对象的run方法
			threads.create_thread(boost::bind(&boost::asio::io_context::run, &ioc)); 
		}

		/*
		* 这个线程组是用来运行io_context对象的，
		* 而io_context对象是用来管理所有异步操作的，包括接受连接、读写数据等。
		* 所以这个线程组实际上是对所有的网络通信进行处理的，不仅仅是一个服务器连接上的读写。
		* 
		* 每一个线程中的io_context.run也不是只管理一个服务器连接的异步操作。
		* 实际上，所有的线程都共享同一个io_context对象，
		* 所以它们都可以处理任何异步操作，不管是哪个服务器连接或客户端连接的。
		* io_context对象会自动分配异步操作给空闲的线程，以实现负载均衡和并发处理。
		*/

		threads.join_all(); // 等待所有线程退出
	}
	catch (std::exception& e) {
		Logger(FILE_LOCATION, log_level::error, "Error is " + std::string(e.what()) + "!");
	}
}

