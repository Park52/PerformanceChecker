#ifndef _IPCCLIENT_HEADER_INCLUDED_
#define _IPCCLIENT_HEADER_INCLUDED_
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#elif __APPLE__
#elif __linux__
#else
#	error "Unknown compiler"
#endif

namespace IPC {

	class IPCClient
	{
	public:
		IPCClient();
		virtual ~IPCClient();

	public:
		bool Initialize();
		void finalize();

	public:
		bool read(std::string& msg);
		bool write(const std::string& msg);

	private:
#ifdef _WIN32
		HANDLE m_hMemory;
		HANDLE m_hServerSendEvent = NULL;
		char* m_pBuffer;
#elif __APPLE__
#elif __linux__
#else
#	error "Unknown compiler"
#endif
	};
}
#endif

