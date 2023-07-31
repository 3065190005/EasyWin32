#pragma once
#ifndef __EASY_WINDOW_LEXICAL__
#define __EASY_WINDOW_LEXICAL__

#include <string>
#include <list>

namespace easyWindow {

	class Lexical final
	{
	public:
		class Token
		{
		public:
			Token() :
				m_type(Type::NONE),
				m_row(0),
				m_column(0),
				m_buf{L""} 
			{};
			~Token() {};
		public:
			enum class Type {
				NONE = 0,
				LESS,			// <
				GREATE,			// >
				SLASH,			// /
				LBRACKET,		// [
				RBRACKET,		// ]
				EQUAL,			// =
				COMMA,			// ,

				KEY_WINDOW,		// key_window
				
				KEY_CHILD,		// key_input

				VAR_ID,			// var_name
				String_const,	// string

				KEY_EOF			// EOF
			};

		public:
			Type m_type;		// token 类型
			long m_row;			// token 行
			long m_column;		// token 列
			std::wstring m_buf; // token 值
		};

	public:
		explicit Lexical(std::wstring buf);
		~Lexical();

		std::list<Token> tokens();		// 解析所有token
	private:
		void Skip();					// 跳过所有额外符号 空格 \t
		bool getSymbol(Token& tok);		// 获取指定符号 = [ ] < > /
		bool getKeyWord(Token& tok);	// 获取关键字 window button 
		bool getVariable(Token& tok);	// 获取变量 

	private:
		wchar_t check_char();			// 检测当前字符
		wchar_t check_next_char();		// 检测下一个字符
		void take_char();			// 跳过当前字符

		void throw_err();				// 跳过当前字符

	private:
		// 工具函数
		std::wstring replace(std::wstring txt, std::wstring sub, std::wstring ne, int count);		// 替换字符
		std::wstring getAsciiName();			// 获取 英文单词
		std::wstring getStringConst();			// 获取 双引号字符串
		std::wstring getAsciiNumber();			// 获取 字母

	private:
		std::wstring m_buf;					// 内容buf
		std::list<Token> m_tokens;			// token
		std::wstring::iterator m_iter;		// 当前位置
		std::wstring::iterator m_enditer;	// 结束位置

		size_t m_row;				// 当前行
		size_t m_column;			// 当前列

		bool m_init;				// 是否初始化

		std::list<std::wstring> m_child_name; // ChildKeyWorld
	
	public:
		using TokenList = std::list<Lexical::Token>;
		using TokenType = Token::Type;
	};
};

#endif

