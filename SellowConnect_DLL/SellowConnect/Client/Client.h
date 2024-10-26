#pragma once
//#include "../../pch.h"
#include <string>

class Client
{
	public:
		std::string Username = "User";

	private:
		void createClient();
		void updateClient();

		void clientJoined();
		void clientLeft();
};

