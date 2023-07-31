#pragma once
#ifndef __EASY_WINDOW_MENU__
#define __EASY_WINDOW_MENU__

#include <map>
#include <list>
#include <string>
#include <Windows.h>

namespace easyWindow 
{
	class Menu
	{
	private:
		struct menuData {
			bool isChild{ false };
			std::string name;
			long id;
			std::list<std::string> childs;
			std::list<std::string> parents;
			HMENU menu;
		};

	public:
		Menu(std::wstring buf);
		~Menu();

	public:
		HMENU get();

		std::wstring m_buf;
	};
}


#endif
