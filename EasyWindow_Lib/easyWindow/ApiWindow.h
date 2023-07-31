#pragma once
#ifndef __EASY_WINDOW_APIWINDOW__
#define __EASY_WINDOW_APIWINDOW__

#include <string>
#include <memory>
#include <map>

#include "Lexical.h"
#include "Parser.h"
#include "VmWindow.h"

namespace easyWindow
{
	class ApiWindow
	{
	public:
		ApiWindow(std::wstring text);
		using AttributeMap = std::map<std::wstring, std::wstring>;
	public:
		std::shared_ptr<ASTClass>  getAst();

	private:
		std::wstring m_text;
	};

	using AttributeMap = ApiWindow::AttributeMap;
}


#endif

