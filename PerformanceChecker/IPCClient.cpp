#include "Common.h"
#include "IPCClient.h"

IPC::IPCClient::IPCClient()
	:
#ifdef _WIN32
	m_hMemory(NULL),
	m_hServerSendEvent(NULL),
#endif
	m_pBuffer(nullptr),
	m_bStop(false)
{
}

IPC::IPCClient::~IPCClient()
{
	finalize();
}

bool IPC::IPCClient::Initialize()
{
	_ASSERTE(NULL == m_hMemory);
	_ASSERTE(NULL == m_hServerSendEvent);
	_ASSERTE(nullptr == m_pBuffer);

	bool ret = false;
	do
	{
#ifdef _WIN32
		m_hServerSendEvent = CreateEventA(NULL,
										  TRUE,
										  FALSE,
										  ServerMsgEventName.c_str());
		if (NULL == m_hServerSendEvent)
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
		if (NULL != m_hServerSendEvent)
		{
			CloseHandle(m_hServerSendEvent);
			m_hServerSendEvent = NULL;
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

void IPC::IPCClient::finalize()
{
#ifdef _WIN32
	if (NULL != m_hServerSendEvent)
	{
		SetEvent(m_hServerSendEvent);
		CloseHandle(m_hServerSendEvent);
		m_hServerSendEvent = NULL;
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

bool IPC::IPCClient::read(std::string & msg)
{
	while (::WaitForSingleObject(m_hServerSendEvent, INFINITE))
	{
		size_t* size = (size_t*)m_pBuffer;
		if (*size < 1)
		{
			std::cerr << "Read Failed" << std::endl;
			return false;
		}
		m_pBuffer = m_pBuffer + sizeof(size_t);

		char* buffer = new char[*size];
		if (nullptr == buffer)
		{
			std::cerr << "Memory Allocation Failed." << std::endl;
			return false;
		}

		memcpy(buffer, m_pBuffer, *size);
		msg = std::string(buffer);
		m_pBuffer = m_pBuffer + *size;

		if (nullptr != buffer)
		{
			delete[] buffer;
			buffer = nullptr;
		}

		std::cout << "Received Message: " << msg.c_str() << std::endl;
	}
	return true;
}

bool IPC::IPCClient::write(const std::string & msg)
{
	size_t size = msg.size() + 1;
	memcpy(m_pBuffer, &size, sizeof(size_t));
	m_pBuffer = m_pBuffer + sizeof(size_t);

	memcpy(m_pBuffer, msg.c_str(), size);
	m_pBuffer = m_pBuffer + size;

	std::cout << "Sent Message: " << msg.c_str() << std::endl;
	return true;
}
