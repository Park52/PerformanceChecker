#ifndef _IPCSERVER_HEADER_INCLUDED_
#define _IPCSERVER_HEADER_INCLUDED_

#include <iostream>
#include <string>

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

	public:
		bool read(std::string& msg);
		bool write(const std::string& msg);

	private:
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