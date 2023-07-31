#pragma once
#ifndef __EASY_WINDOW_VMWINDOW__
#define __EASY_WINDOW_VMWINDOW__

#include "Lexical.h"
#include "Parser.h"
#include "WindowManager.h"

#include <list>
#include <memory>

namespace easyWindow
{

	/**********************************************
	* VmMemery Class
	* 内存类
	* 用来进行统一的内存扩大和使用以及回收
	* 重新设置内存，需要当内存小于当前值则不进行重置
	* 默认初始化内存值为 1024
	**********************************************/

	class VmMemery
	{
	public:
		VmMemery() :
			m_memery(new wchar_t[1024]),
			m_memlens(1024)
		{};
		~VmMemery() 
		{ 
			if (m_memery) 
			{ 
				delete m_memery; 
			} 
		};
	public:
		bool Realloc(size_t lens);
		wchar_t* getMemery() { return m_memery; }
		size_t getMemLens() { return m_memlens; }
	private:
		wchar_t* m_memery;
		size_t m_memlens;
	};

	/*************************
	* VmLoop Class
	* 窗口循环类
	* 用来进行loop的每帧系统更新
	* 更新 窗体描述 到 属性Map
	* 设置 属性Map 到 窗体描述
	***************************/
	namespace VmClassStruct 
	{
		struct tagWindowNode {
			HWND self;
			HWND parent;
			AttributeMap attribute;
			std::wstring name;
			Lexical::TokenType type;
		};

		enum class enumAutoRect
		{
			none = 0,
			horz = 1,
			vert = 2,
		};
	}

	class VmLoop
	{
	public:
		using TokenList = Lexical::TokenList;
		using Token = Lexical::Token;
		using TokenType = Lexical::TokenType;
		using AstType = ASTClass::AstType;
		using WindowNode = struct VmClassStruct::tagWindowNode;

	public:
		VmLoop();
		~VmLoop();

		static BOOL CALLBACK enumChildFinder(HWND hwndChild, LPARAM lParam);
	public:
		// 工具函数
		long double wStringToNumber(std::wstring);		// 字符串 转换 数字
		std::wstring NumberTowString(long number);		// 数字 转换 字符串

		void SetWindowHwndList(std::list<WindowNode>*);	// 获取用于更新的窗体列表
		void SetWindowManager(WindowManager* manager);  // 获取用于销毁的窗体类
		std::list<std::wstring> splitwString(std::wstring str, wchar_t split);

	public:
		void updateAutoRect(HWND hwnd, VmClassStruct::enumAutoRect type,long interval);	// 窗口的自动模式

	public:
		// 用户调用 将 属性map 更新到 窗体描述
		void updateWithAttribute(AttributeMap map, AttributeMap old, HWND hwnd);

		// window
		void Attribute_update_window(AttributeMap map, HWND hwnd);	// window 更新

		// button 
		void Attribute_update_button(AttributeMap map, HWND hwnd);	// button 更新
		
		// edit
		void Attribute_update_edit(AttributeMap map, HWND hwnd);	// edit 更新

		// combobox
		void Attribute_update_combobox(AttributeMap map, HWND hwnd);	// combobox 更新

		// layer
		void Attribute_update_layer(AttributeMap map, HWND hwnd);	// layer 更新

		// progress
		void Attribute_update_progress(AttributeMap map, AttributeMap old, HWND hwnd);	// progress 更新

		// scroll
		void Attribute_update_scroll(AttributeMap map, AttributeMap old, HWND hwnd);	// scroll 更新

		// listbox
		void Attribute_update_listbox(AttributeMap map, AttributeMap old, HWND hwnd);	// listbox 更新

		// static
		void Attribute_update_static(AttributeMap map, AttributeMap old, HWND hwnd);	// static 更新

		// image
		void Attribute_update_image(AttributeMap map, AttributeMap old, HWND hwnd);		// image 更新


	public:
		// 循环更新 获取 窗体描述 更新到 属性Map
		void loopUpdateAttribute(WindowNode& node);

		void Window_loop_attribute(WindowNode& node);
		void Edit_loop_attribute(WindowNode& node);
		void Botton_loop_attribute(WindowNode& node);
		void Combobox_loop_attribute(WindowNode& node);
		void Layer_loop_attribute(WindowNode& node);
		void Progress_loop_attribute(WindowNode& node);
		void ScrollBar_loop_attribute(WindowNode& node);
		void ListBox_loop_attribute(WindowNode& node);
		void Static_loop_attribute(WindowNode& node);
		void Image_loop_attribute(WindowNode& node);

	private:
		std::list<WindowNode>* m_window_hwnd;		// VmWindow 的窗体数据
		WindowManager *m_manager;					// 窗体管理类
		VmMemery m_memery;							// 动态内存
		std::list<HWND> m_child_list;				// 自动化窗口列表
	};


	/**********************************************
	* VmWindow Class
	* 窗体虚拟机
	* 用来处理Ast节点以及通过 Map属性 初始化窗体
	* 通过调用 VmLoop 类来进行窗体的基本循环
	* 自身处理用户层接口
	**********************************************/

	class VmWindow
	{
	public:
		using TokenList = Lexical::TokenList;
		using Token = Lexical::Token;
		using TokenType = Lexical::TokenType;
		using AstType = ASTClass::AstType;
		using WindowNode = struct VmClassStruct::tagWindowNode;
		using AutoRect = VmClassStruct::enumAutoRect;
		using AutoType = std::tuple<std::wstring, AutoRect,long>;
		using AutoRectList = std::list<AutoType>;

	public:
		VmWindow(std::shared_ptr<ASTClass>);
		~VmWindow();

	public:
		bool init();			// 初始化窗体
		bool loop();			// 窗体循环

		AttributeMap getAttributeById(std::wstring id);							// 通过id获取窗体属性
		void setAttributeById(std::wstring id, AttributeMap map);				// 通过id设置窗体属性
		void setWindowAutoRect(std::wstring id, AutoRect type, long interval);	// 设置窗口自动大小

		bool hasMessage();			// 是否含有Message事件
		void clearMessage();		// 清理所有Message事件
		std::wstring getMessage();	// 消耗第一个Message事件

	private:
		bool VmProgram(ASTClass*);					// program Ast节点处理
		bool VmBody(ASTClass*);						// body Ast节点处理
		bool VmChild(ASTClass*, HWND parent);		// Child Ast节点处理

	private:
		std::shared_ptr<ASTClass> m_ast_tree;		// 节点树
		std::list<WindowNode> m_window_hwnd;		// 窗口数据合集列表
		WindowManager m_manager;					// 窗体实例
		VmLoop m_loop;								// 动态更新类
		AutoRectList m_auto_list;					// 自动窗口大小

		bool m_init;
	};

}


#endif
