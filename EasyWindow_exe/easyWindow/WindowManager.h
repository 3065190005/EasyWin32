#pragma once
#ifndef __EASY_WINDOW_WINDOWMANAGER__
#define __EASY_WINDOW_WINDOWMANAGER__


// 使用 公用库 6.0 版本 ， 需要vsc++ 2006
#ifdef _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif _M_IA64
#pragmacomment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <queue>
#include <list>
#include <map>

#include "Parser.h"

#include <Windows.h>
#include <WinUser.h>
#include <windowsx.h>
#include <commctrl.h>
#include <olectl.h>
#include <gdiplus.h> 
#pragma comment (lib, "GdiPlus.lib")



namespace easyWindow
{
	class WindowManager
	{
	public:
		WindowManager() {
			static Gc gc;
		};
		~WindowManager() {};

		struct fontColor
		{
			std::wstring bgcolor;
			std::wstring fontcolor;
			std::wstring bgvisible;
		};

		struct ImageInfo
		{
			std::wstring path;
		};

	public:
		HWND Window(AttributeMap attribute);					// 创建 窗体
		HWND Button(AttributeMap attribute,HWND parent);		// 创建 按钮
		HWND Edit(AttributeMap attribute, HWND parent);			// 创建 编辑框
		HWND ComboBox(AttributeMap attribute, HWND parent);		// 创建 组合框
		HWND Layer(AttributeMap attribute, HWND parent);		// 创建 层级框
		HWND Progress(AttributeMap attribute, HWND parent);		// 创建 进度条
		HWND Scroll(AttributeMap attribute, HWND parent);		// 创建 滚动条
		HWND ListBox(AttributeMap attribute, HWND parent);		// 创建 列表框
		HWND Static(AttributeMap attribute, HWND parent);		// 创建 文字框
		HWND Image(AttributeMap attribute, HWND parent);		// 创建 图形框

		void RemoveHwndMap(HWND hwnd);						// 移除 hwnd与指针的 匹配关系

	public:
		static LRESULT CALLBACK Window_WndProc(HWND, UINT, WPARAM, LPARAM);
		static LRESULT CALLBACK Layer_WndProc(HWND, UINT, WPARAM, LPARAM);
		static LRESULT CALLBACK Scroll_WndProc(HWND, UINT, WPARAM, LPARAM);
		static LRESULT CALLBACK Image_WndProc(HWND, UINT, WPARAM, LPARAM);

		std::queue<std::pair<HWND,std::wstring>> Message;	// callback 代号 消息队列
		std::deque<std::wstring> wStringMessage;			// callback 字符串 消息队列

	public:
		static long wstringTonumber(std::wstring str);				// 字符串转换数字
		static std::wstring numberTowstring(long number);			// 数字转换字符串
		std::list<std::wstring> 
			splitwString(std::wstring string, wchar_t tok);	// 拆分字符

		HMENU menuGet(std::wstring menuStr);			// 获取menu

		static std::map<HWND, WindowManager*> HwndMap;		// hwnd与指针的匹配关系
		static std::map<HWND, fontColor> FontMap;			// static 字体颜色信息
		static std::map<HWND, ImageInfo> ImageMap;			// image 文件信息

	protected:
		class Gc
		{
		public:
			Gc() {
				Gdiplus::GdiplusStartupInput gdiInput;
				Gdiplus::GdiplusStartup(&gdiplusStartupToken, &gdiInput, NULL);
			}
			~Gc() {
				Gdiplus::GdiplusShutdown(gdiplusStartupToken);
			}
		public:
			ULONG_PTR gdiplusStartupToken;
		};
	};
}

#endif

