#pragma once
#ifndef __EASY_WINDOW_WINDOWMANAGER__
#define __EASY_WINDOW_WINDOWMANAGER__


// ʹ�� ���ÿ� 6.0 �汾 �� ��Ҫvsc++ 2006
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
		HWND Window(AttributeMap attribute);					// ���� ����
		HWND Button(AttributeMap attribute,HWND parent);		// ���� ��ť
		HWND Edit(AttributeMap attribute, HWND parent);			// ���� �༭��
		HWND ComboBox(AttributeMap attribute, HWND parent);		// ���� ��Ͽ�
		HWND Layer(AttributeMap attribute, HWND parent);		// ���� �㼶��
		HWND Progress(AttributeMap attribute, HWND parent);		// ���� ������
		HWND Scroll(AttributeMap attribute, HWND parent);		// ���� ������
		HWND ListBox(AttributeMap attribute, HWND parent);		// ���� �б��
		HWND Static(AttributeMap attribute, HWND parent);		// ���� ���ֿ�
		HWND Image(AttributeMap attribute, HWND parent);		// ���� ͼ�ο�

		void RemoveHwndMap(HWND hwnd);						// �Ƴ� hwnd��ָ��� ƥ���ϵ

	public:
		static LRESULT CALLBACK Window_WndProc(HWND, UINT, WPARAM, LPARAM);
		static LRESULT CALLBACK Layer_WndProc(HWND, UINT, WPARAM, LPARAM);
		static LRESULT CALLBACK Scroll_WndProc(HWND, UINT, WPARAM, LPARAM);
		static LRESULT CALLBACK Image_WndProc(HWND, UINT, WPARAM, LPARAM);

		std::queue<std::pair<HWND,std::wstring>> Message;	// callback ���� ��Ϣ����
		std::deque<std::wstring> wStringMessage;			// callback �ַ��� ��Ϣ����

	public:
		static long wstringTonumber(std::wstring str);				// �ַ���ת������
		static std::wstring numberTowstring(long number);			// ����ת���ַ���
		std::list<std::wstring> 
			splitwString(std::wstring string, wchar_t tok);	// ����ַ�

		HMENU menuGet(std::wstring menuStr);			// ��ȡmenu

		static std::map<HWND, WindowManager*> HwndMap;		// hwnd��ָ���ƥ���ϵ
		static std::map<HWND, fontColor> FontMap;			// static ������ɫ��Ϣ
		static std::map<HWND, ImageInfo> ImageMap;			// image �ļ���Ϣ

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

