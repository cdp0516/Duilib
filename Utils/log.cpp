#include "log.h"
#include <stdarg.h>
#include <Windows.h>
#include <strstream>

Log::Log()
{
	m_stop = false;
	m_type = log2_output;
	m_writeThread = std::thread(std::bind(&Log::writeProc, this));
}

Log::~Log()
{
	m_stop = true;
	if (m_file)
		m_file.close();

	if (m_type == log2_console)
		FreeConsole();

	m_writeThread.join();
}

void Log::initLog(log_type type, const std::wstring& logName, const std::wstring& logpath)
{
	m_type = type;
	if (type == log2_file)
	{
		//std::vector<std::wstring> old_logs;
		//get_files_in_folder(logpath, old_logs);
		//std::sort(old_logs.begin(), old_logs.end(), [&](const std::wstring& str1, const std::wstring& str2){
		//	return str1 > str2;
		//});

		//for (unsigned int i = 4; i < old_logs.size(); i++)
		//{
		//	std::wstring file = logpath;
		//	if (*(file.rbegin()) != L'\\')
		//	{
		//		file += L"\\";
		//	}
		//	file += old_logs.at(i);
		//	delete_file(file);
		//}

		//time_t time_seconds = time(0);	
		//struct tm now_time;	
		//localtime_s(&now_time, &time_seconds);	
		//wchar_t tmp[100] = { 0 };
		//wsprintf(tmp, L"%s-%04d-%02d-%02d.log", logName.c_str(), now_time.tm_year + 1900, now_time.tm_mon + 1, now_time.tm_mday);
		//std::wstring new_log = logpath;
		//if (logpath.substr(logpath.length() - 1) != L"\\")
		//	new_log.append(L"\\");

		//new_log = new_log + tmp;

		//std::string file = w2utf8(new_log);
		//m_file.open(file, std::ios::app);
	}
	else if (type == log2_output)
	{
	}
	else if (type == log2_console)
	{
		AllocConsole();
	}
}

void Log::writeLog(wchar_t* format, ...)
{
	LOCKLOCK;

	va_list args;
	int len;
	wchar_t * buffer;
	va_start(args, format);
	len = _vscwprintf(format, args) + 1; 
	buffer = new wchar_t[len];
	vswprintf_s(buffer, len, format, args);
	std::string temp = formatLogW(buffer);
	m_que.push(temp);
	delete [] buffer;
	va_end(args);
}

void Log::writeLog(char* format, ...)
{
	LOCKLOCK;

	va_list args;
	int len;
	char * buffer;
	va_start(args, format);
	len = _vscprintf(format, args) + 1;
	buffer = new char[len];
	vsprintf_s(buffer, len, format, args);
	std::string temp = formatLogA(buffer);
	m_que.push(temp);
	delete[] buffer;
	va_end(args);
}

void Log::writeProc()
{
	while (!m_stop)
	{
		std::string str;
		if (!getLog(str))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			continue;
		}
		
		switch (m_type)
		{
		case Log::log2_console:
			WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), str.c_str(), str.size(), NULL, NULL);
			break;
		case Log::log2_output:
			OutputDebugStringA(str.c_str());
			break;
		case Log::log2_file:
			if (m_file.is_open())
			{
				m_file.write(str.c_str(), str.length());
				m_file.flush();
			}
			break;
		default:
			break;
		}
	}
}

bool Log::getLog(std::string& str)
{
	LOCKLOCK;
	if (m_que.size() != 0)
	{
		str = m_que.front();
		m_que.pop();
		return true;
	}

	return false;
}

std::string w2a(const std::wstring& str)
{
	int size = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, 0, 0, 0, 0);
	char *ch = new char[size];
	if (!WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, ch, size, 0, 0)) {
		return "";
	}
	std::string temp = std::string(ch);
	delete ch;
	return temp;
}

std::string Log::formatLogW(const std::wstring& str)
{
	std::ostrstream out;
	std::string tmp = w2a(str);
	char buf[100] = { 0 };
	struct tm now;
	time_t t = time(0);
	localtime_s(&now, &t);
	sprintf_s(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
	out << "[" << buf << "--" << std::this_thread::get_id() << "]-->" << tmp << "\r\n" << std::ends;
	tmp = out.str();
	delete out.str();
	return tmp;
}

std::string Log::formatLogA(const std::string& str)
{
	std::ostrstream out;
	std::string tmp = str;
	char buf[100] = { 0 };
	struct tm now;
	time_t t = time(0);
	localtime_s(&now, &t);
	sprintf_s(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
	out << "[" << buf << "--" << std::this_thread::get_id() << "]-->" << tmp << "\r\n" << std::ends;
	tmp = out.str();
	delete out.str();
	return tmp;
}
