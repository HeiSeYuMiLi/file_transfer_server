#include "Logger.h"

namespace file_server {

	Logger::Logger(const std::string& file_path, unsigned long line,
		log_level level, const std::string& message) :_level(level) {
		int pos = file_path.find_last_of('/');
		if (pos == std::string::npos) {
			pos = file_path.find_last_of('\\');
		}
		_buff = file_path.substr(pos + 1) + ": " + std::to_string(line) + " | " + message;
	}

	Logger::~Logger() {
		PrintLog(MakeLog());
	}

	std::string Logger::GetLevelString()
	{
		switch (_level)
		{
		case log_level::error: return "[error]";
		case log_level::warn: return "[warn]";
		case log_level::info: return "[info]";
		case log_level::debug: return "[debug]";
		default: return "[debug]";
		}
	}

	std::string Logger::MakeLog()
	{
		std::string log;
		log += GetLevelString();
		log += " ";
		log += Currtime();
		log += " ";
		log += _buff;

		return log;
	}

	void Logger::PrintLog(const std::string& log)
	{
		// 根据不同的等级进行不同的颜色的输出
		switch (_level)
		{
		case log_level::error:
			std::cout << "\033[31m" << log << "\n\033[0m";
			break;
		case log_level::warn:
			std::cout << "\033[33m" << log << "\n\033[0m";
			break;
		default:
			std::cout << log << std::endl;
			break;
		}
	}

	std::string Logger::Currtime() {


		// 获取当前系统时间点
		std::chrono::time_point<std::chrono::system_clock> tp = std::chrono::system_clock::now();
		// 获取当前年月日时分秒
		std::time_t t = std::chrono::system_clock::to_time_t(tp);
		tm* result = new tm;
#ifdef _linux_
		localtime_r(&t, result);
#elif _WIN32
		localtime_s(result, &t);
#endif
		char buffer[30];
		strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", result);

		return std::string(buffer);

	}
}
