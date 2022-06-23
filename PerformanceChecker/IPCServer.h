#ifndef _IPCSERVER_HEADER_INCLUDED_
#define _IPCSERVER_HEADER_INCLUDED_

#include <iostream>
#include <string>
#include <queue>
#include <mutex>

#ifdef _WIN32
#include <Windows.h>
#elif __APPLE__
#elif __linux__
#else
#	error "Unknown compiler"
#endif

namespace IPC {

	class IPCServer
	{
	public:
		IPCServer();
		virtual ~IPCServer();

	public:
		bool Initialize();
		void finalize();

		bool write(const std::string& msg);

	private:
		bool read(std::string& msg);

	private:
		bool m_bStop;

#ifdef _WIN32
		HANDLE m_hMemory;
		HANDLE m_hClientSendEvent = NULL;
		char* m_pBuffer;
#elif __APPLE__
#elif __linux__
#else
#	error "Unknown compiler"
#endif
	};
}

#endif