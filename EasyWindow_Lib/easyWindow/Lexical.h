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
			Type m_type;		// token ����
			long m_row;			// token ��
			long m_column;		// token ��
			std::wstring m_buf; // token ֵ
		};

	public:
		explicit Lexical(std::wstring buf);
		~Lexical();

		std::list<Token> tokens();		// ��������token
	private:
		void Skip();					// �������ж������ �ո� \t
		bool getSymbol(Token& tok);		// ��ȡָ������ = [ ] < > /
		bool getKeyWord(Token& tok);	// ��ȡ�ؼ��� window button 
		bool getVariable(Token& tok);	// ��ȡ���� 

	private:
		wchar_t check_char();			// ��⵱ǰ�ַ�
		wchar_t check_next_char();		// �����һ���ַ�
		void take_char();			// ������ǰ�ַ�

		void throw_err();				// ������ǰ�ַ�

	private:
		// ���ߺ���
		std::wstring replace(std::wstring txt, std::wstring sub, std::wstring ne, int count);		// �滻�ַ�
		std::wstring getAsciiName();			// ��ȡ Ӣ�ĵ���
		std::wstring getStringConst();			// ��ȡ ˫�����ַ���
		std::wstring getAsciiNumber();			// ��ȡ ��ĸ

	private:
		std::wstring m_buf;					// ����buf
		std::list<Token> m_tokens;			// token
		std::wstring::iterator m_iter;		// ��ǰλ��
		std::wstring::iterator m_enditer;	// ����λ��

		size_t m_row;				// ��ǰ��
		size_t m_column;			// ��ǰ��

		bool m_init;				// �Ƿ��ʼ��

		std::list<std::wstring> m_child_name; // ChildKeyWorld
	
	public:
		using TokenList = std::list<Lexical::Token>;
		using TokenType = Token::Type;
	};
};

#endif

