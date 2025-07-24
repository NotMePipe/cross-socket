#include "CrossSocket/CrossSocketUtils.h"

#include <stdexcept>

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
#endif // _WIN32
		return initialized;
	}

	/**
	 * @brief Shut down CrossSocket
	 */
	void CS_Utils::Cleanup()
	{
#ifdef _WIN32
		WSACleanup();
#endif // _WIN32
		initialized = false;
	}

	uint32_t CS_Utils::cs_htonl(uint32_t val)
	{
		return htonl(val);
	}

	uint32_t CS_Utils::cs_ntohl(uint32_t val)
	{
		return ntohl(val);
	}
}
