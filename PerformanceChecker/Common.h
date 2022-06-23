#ifndef _IPC_NAMESPACE_HEADER_INCLUDED_
#define _IPC_NAMESPACE_HEADER_INCLUDED_

#include <functional>
#include <iostream>

const std::string PipeName("Global\\PerformanceChecker");
const std::string ServerMsgEventName("Global\\ServerSendEvent");
const std::string ClientMsgEventName("Global\\ClientSendEvent");

struct Message {
	size_t size;
	uint32_t cmd;
	std::string message;
};

#endif