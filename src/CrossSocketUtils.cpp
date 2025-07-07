#include "CrossSocketUtils.h"

namespace CrossSocket
{
	bool CS_Utils::initialized = false;

	bool CS_Utils::Initialize()
	{
		bool initialized = false;
#ifdef _WIN32
		if (!initialized)
		{
			WSADATA wsaData;
			if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			{
				throw std::runtime_error("WSAStartup failed");
			}
			initialized = true;
		}
#else
		initialized = true;
#endif
		return initialized;
	}

	void CS_Utils::Cleanup()
	{
#ifdef _WIN32
		WSACleanup();
#endif
	}
}
