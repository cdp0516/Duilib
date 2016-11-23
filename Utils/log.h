#pragma once
#include <fstream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>

#define LOG_FILE_NAME  "duilib.log"

class Log
{
public:
	enum log_type
	{
		log2_console = 0,	
		log2_output,
		log2_file,
	};

public:
	Log();
	~Log();
	void initLog(log_type type, const std::wstring& logName, const std::wstring& logpath);
	void writeLog(wchar_t* format, ...);
	void writeLog(char* format, ...);

private:
	void writeProc();
	bool getLog(std::string& str);
	std::string formatLogW(const std::wstring& str);
	std::string formatLogA(const std::string& str);

private:
	log_type m_type;
	bool m_stop;
	std::ofstream m_file;
	std::queue<std::string> m_que;
	std::mutex m_mutex;
	std::thread m_writeThread;
};

template <class T>
inline T& getMyInstance()
{
	static T t;
	return t;
}

#define LOG_ getMyInstance<Log>()
#define LOG(a,...) LOG_.writeLog(a, __VA_ARGS__)
#define LOCKLOCK std::lock_guard<std::mutex> lock(m_mutex)