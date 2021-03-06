#include "Common.h"
#include "IPCServer.h"

IPC::IPCServer::IPCServer()
	:
#ifdef _WIN32
	m_hMemory(NULL),
	m_hClientSendEvent(NULL),
#endif
	m_pBuffer(nullptr),
	m_bStop(false)
{

}

IPC::IPCServer::~IPCServer()
{
	finalize();
}

bool IPC::IPCServer::Initialize()
{
	_ASSERTE(NULL == m_hMemory);
	_ASSERTE(NULL == m_hClientSendEvent);
	_ASSERTE(nullptr == m_pBuffer);

	bool ret = false;
	do
	{
#ifdef _WIN32
		m_hClientSendEvent = CreateEventA(NULL,
										  TRUE,
										  FALSE,
										  ClientMsgEventName.c_str());
		if (NULL == m_hClientSendEvent)
		{
			std::cerr << "Could not Create Event. GetLastError: " << GetLastError() << std::endl;
			break;
		}

		m_hMemory = CreateFileMappingA(INVALID_HANDLE_VALUE,
									   NULL,
									   PAGE_READWRITE,
									   0,
									   1024,
									   PipeName.c_str());
		if (NULL == m_hMemory)
		{
			std::cerr << "Could not Create file Mapping object. GetLastError: " << GetLastError() << std::endl;
			break;
		}

		m_pBuffer = (char*)MapViewOfFile(m_hMemory,   // handle to map object
										 FILE_MAP_ALL_ACCESS, // read/write permission
										 0,
										 0,
										 1024);
		if (nullptr == m_pBuffer)
		{
			std::cerr << "Could not map view of file. GetLastError: " << GetLastError() << std::endl;
			break;
		}
#endif

		ret = true;
	} while (false);

	if (false == ret)
	{
#ifdef _WIN32
		if (NULL != m_hClientSendEvent)
		{
			CloseHandle(m_hClientSendEvent);
			m_hClientSendEvent = NULL;
		}

		if (NULL != m_hMemory)
		{
			CloseHandle(m_hMemory);
			m_hMemory = NULL;
		}
#endif

		if (nullptr != m_pBuffer)
		{
#ifdef _WIN32
			UnmapViewOfFile(m_pBuffer);
#endif // _WIN32
		}
	}

	return ret;
}

void IPC::IPCServer::finalize()
{
#ifdef _WIN32
	if (NULL != m_hClientSendEvent)
	{
		SetEvent(m_hClientSendEvent);
		CloseHandle(m_hClientSendEvent);
		m_hClientSendEvent = NULL;
	}

	if (nullptr != m_pBuffer)
	{
		UnmapViewOfFile(m_pBuffer);
		m_pBuffer = nullptr;
	}

	if (NULL != m_hMemory)
	{
		CloseHandle(m_hMemory);
		m_hMemory = NULL;
	}
#endif
}

bool IPC::IPCServer::read(std::string & msg)
{
	while (::WaitForSingleObject(m_hClientSendEvent, INFINITE))
	{
		if (true == m_bStop)
		{
			break;
		}

		size_t* size = (size_t*)m_pBuffer;
		if (*size < 1)
		{
			std::cerr << "Read Failed" << std::endl;
			break;
		}
		m_pBuffer = m_pBuffer + sizeof(size_t);

		char* buffer = new char[(*size)];
		if (nullptr == buffer)
		{
			std::cerr << "Memory Allocation Failed." << std::endl;
			break;
		}

		memcpy(buffer, m_pBuffer, *size);
		msg = std::string(buffer);
		m_pBuffer = m_pBuffer + (*size);

		if (nullptr != buffer)
		{
			delete[] buffer;
			buffer = nullptr;
		}

		std::cout << "Received Message: " << msg.c_str() << std::endl;
	}

	return true;
}

bool IPC::IPCServer::write(const std::string & msg)
{
	size_t size = msg.size() + 1;
	memcpy(m_pBuffer, &size, sizeof(size_t));
	m_pBuffer = m_pBuffer + sizeof(size_t);

	memcpy(m_pBuffer, msg.c_str(), size);
	m_pBuffer = m_pBuffer + size;

	std::cout << "Sent Message: " << msg.c_str() << std::endl;
	return true;
}
