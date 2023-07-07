#pragma once

#include <string>
#include <chrono>
#include <ctime>
#include <iostream>

namespace file_server {

	// 定义这个宏，返回当前运行到哪个文件的哪一行
#define FILE_LOCATION __FILE__,__LINE__

	// 定义日志等级
	enum class log_level {
		debug = 0,
		info,
		warn,
		error
	};

	// 日志类
	class Logger {
	public:
		Logger(const std::string& file_path, unsigned long line, log_level level, const std::string& message);
		~Logger();

	private:
		std::string GetLevelString();
		std::string MakeLog();
		void PrintLog(const std::string& log);
		// 获取当前时间
		std::string Currtime();

		log_level _level;
		std::string _buff;
	};

}

