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
	* �ڴ���
	* ��������ͳһ���ڴ������ʹ���Լ�����
	* ���������ڴ棬��Ҫ���ڴ�С�ڵ�ǰֵ�򲻽�������
	* Ĭ�ϳ�ʼ���ڴ�ֵΪ 1024
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
	* ����ѭ����
	* ��������loop��ÿ֡ϵͳ����
	* ���� �������� �� ����Map
	* ���� ����Map �� ��������
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
		// ���ߺ���
		long double wStringToNumber(std::wstring);		// �ַ��� ת�� ����
		std::wstring NumberTowString(long number);		// ���� ת�� �ַ���

		void SetWindowHwndList(std::list<WindowNode>*);	// ��ȡ���ڸ��µĴ����б�
		void SetWindowManager(WindowManager* manager);  // ��ȡ�������ٵĴ�����
		std::list<std::wstring> splitwString(std::wstring str, wchar_t split);

	public:
		void updateAutoRect(HWND hwnd, VmClassStruct::enumAutoRect type,long interval);	// ���ڵ��Զ�ģʽ

	public:
		// �û����� �� ����map ���µ� ��������
		void updateWithAttribute(AttributeMap map, AttributeMap old, HWND hwnd);

		// window
		void Attribute_update_window(AttributeMap map, HWND hwnd);	// window ����

		// button 
		void Attribute_update_button(AttributeMap map, HWND hwnd);	// button ����
		
		// edit
		void Attribute_update_edit(AttributeMap map, HWND hwnd);	// edit ����

		// combobox
		void Attribute_update_combobox(AttributeMap map, HWND hwnd);	// combobox ����

		// layer
		void Attribute_update_layer(AttributeMap map, HWND hwnd);	// layer ����

		// progress
		void Attribute_update_progress(AttributeMap map, AttributeMap old, HWND hwnd);	// progress ����

		// scroll
		void Attribute_update_scroll(AttributeMap map, AttributeMap old, HWND hwnd);	// scroll ����

		// listbox
		void Attribute_update_listbox(AttributeMap map, AttributeMap old, HWND hwnd);	// listbox ����

		// static
		void Attribute_update_static(AttributeMap map, AttributeMap old, HWND hwnd);	// static ����

		// image
		void Attribute_update_image(AttributeMap map, AttributeMap old, HWND hwnd);		// image ����


	public:
		// ѭ������ ��ȡ �������� ���µ� ����Map
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
		std::list<WindowNode>* m_window_hwnd;		// VmWindow �Ĵ�������
		WindowManager *m_manager;					// ���������
		VmMemery m_memery;							// ��̬�ڴ�
		std::list<HWND> m_child_list;				// �Զ��������б�
	};


	/**********************************************
	* VmWindow Class
	* ���������
	* ��������Ast�ڵ��Լ�ͨ�� Map���� ��ʼ������
	* ͨ������ VmLoop �������д���Ļ���ѭ��
	* �������û���ӿ�
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
		bool init();			// ��ʼ������
		bool loop();			// ����ѭ��

		AttributeMap getAttributeById(std::wstring id);							// ͨ��id��ȡ��������
		void setAttributeById(std::wstring id, AttributeMap map);				// ͨ��id���ô�������
		void setWindowAutoRect(std::wstring id, AutoRect type, long interval);	// ���ô����Զ���С

		bool hasMessage();			// �Ƿ���Message�¼�
		void clearMessage();		// ��������Message�¼�
		std::wstring getMessage();	// ���ĵ�һ��Message�¼�

	private:
		bool VmProgram(ASTClass*);					// program Ast�ڵ㴦��
		bool VmBody(ASTClass*);						// body Ast�ڵ㴦��
		bool VmChild(ASTClass*, HWND parent);		// Child Ast�ڵ㴦��

	private:
		std::shared_ptr<ASTClass> m_ast_tree;		// �ڵ���
		std::list<WindowNode> m_window_hwnd;		// �������ݺϼ��б�
		WindowManager m_manager;					// ����ʵ��
		VmLoop m_loop;								// ��̬������
		AutoRectList m_auto_list;					// �Զ����ڴ�С

		bool m_init;
	};

}


#endif
