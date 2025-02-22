// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.hpp"

bool main()
{
	if (!m_memory.is_initialized())
	{
		LOG_ERROR("failed to initialize memory class");
		std::this_thread::sleep_for(std::chrono::seconds(5));
		return true;
	}

	// @easywsclient
	WSADATA wsa_data{};
	const auto wsa_startup = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (wsa_startup != 0)
	{
		LOG_ERROR("failed to initialize winsock");
		std::this_thread::sleep_for(std::chrono::seconds(5));
		return true;
	}

	const auto formatted_address = std::format("ws://{}:22006/cs2_webradar", utils::get_ipv4_address());
	static auto web_socket = easywsclient::WebSocket::from_url(formatted_address);
	if (!web_socket)
	{
		LOG_ERROR("failed to connect to the web socket ('%s')", formatted_address.data());
		std::this_thread::sleep_for(std::chrono::seconds(5));
		return true;
	}

	LOG_INFO("connected to the web socket ('%s')", formatted_address.data());

	auto start = std::chrono::system_clock::now();

	for (;;)
	{
		const auto now = std::chrono::system_clock::now();
		const auto duration = now - start;
		if (duration >= std::chrono::milliseconds(100))
		{
			start = now;

			m_features.run();

			// LOG_INFO("%s", m_features.get_data().dump().data());
			web_socket->send(m_features.get_data().dump());
		}

		web_socket->poll();
	}

	system("pause");

	return true;
}