#pragma once

#include <string>
#include <chrono>
#include <ctime>
#include <iostream>

namespace file_server {

	// ��������꣬���ص�ǰ���е��ĸ��ļ�����һ��
#define FILE_LOCATION __FILE__,__LINE__

	// ������־�ȼ�
	enum class log_level {
		debug = 0,
		info,
		warn,
		error
	};

	// ��־��
	class Logger {
	public:
		Logger(const std::string& file_path, unsigned long line, log_level level, const std::string& message);
		~Logger();

	private:
		std::string GetLevelString();
		std::string MakeLog();
		void PrintLog(const std::string& log);
		// ��ȡ��ǰʱ��
		std::string Currtime();

		log_level _level;
		std::string _buff;
	};

}

