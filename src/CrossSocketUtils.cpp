#include "CrossSocketUtils.h"

namespace CrossSocket
{
	bool CS_Utils::initialized = false;

	/**
	* @brief Initialize CrossSocket
	* 
	* @return True if CrossSocket is initialized. False if CrossSocket is not initialized
	*/
	bool CS_Utils::Initialize()
	{
		initialized = false;
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

	/**
	* @brief Shut down CrossSocket
	*/
	void CS_Utils::Cleanup()
	{
#ifdef _WIN32
		WSACleanup();
#endif
		initialized = false;
	}
}
