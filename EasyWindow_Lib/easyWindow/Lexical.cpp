#include "Lexical.h"

#include <sstream>
#include <algorithm>

namespace easyWindow 
{

	Lexical::Lexical(std::wstring buf)
	{
		m_buf = buf;
		
		m_buf = replace(m_buf, L"\r", L"\n", -1);
		m_buf = replace(m_buf, L"\n\n", L"\n", -1);

		m_iter = m_buf.begin();
		m_enditer = m_buf.end();

		m_row = 1;
		m_column = 1;

		m_init = false;

		m_child_name =
		{
			L"button",
			L"edit",
			L"combobox",
			L"layer",
			L"progress",
			L"scroll",
			L"listbox",
			L"static",
			L"image",
		};
	}

	Lexical::~Lexical()
	{
	}

	std::list<Lexical::Token> easyWindow::Lexical::tokens()
	{
		if (!m_init)
		{
			m_tokens.clear();
			std::list<Token> ret;

			while (true)
			{
				Token tok;
				Skip();
				if (getSymbol(tok)) {
					ret.push_back(tok);
					continue;
				}
				if(getKeyWord(tok)) {
					ret.push_back(tok);
					continue;
				}
				if(getVariable(tok)) {
					ret.push_back(tok);
					continue;
				}

				if (m_iter == m_enditer) {
					tok.m_type = TokenType::KEY_EOF;
					tok.m_row = m_row + 1;
					tok.m_column = 1;
					ret.push_back(tok);
					m_init = true;
					break;
				}

				throw_err();
				break;
			}

			m_tokens = ret;
		}
		
		return m_tokens;
	}

	void Lexical::Skip()
	{
		while (true)
		{
			wchar_t wchar = check_char();
			if (wchar == L'\t') {
				take_char();
				m_column++;
			}
			else if (wchar == L' ') {
				take_char();
				m_column++;
			}
			else if (wchar == L'\n')
			{
				take_char();
				m_row++;
				m_column = 1;
			}
			else
			{
				break;
			}
		}
	}

	bool Lexical::getSymbol(Token& tok)
	{
		bool ret = false;
		tok.m_type = TokenType::NONE;
		wchar_t wchar = check_char();

		if (wchar == L'[')
			tok.m_type = TokenType::LBRACKET;
		if (wchar == L']')
			tok.m_type = TokenType::RBRACKET;
		if (wchar == L'<')
			tok.m_type = TokenType::LESS;
		if (wchar == L'>')
			tok.m_type = TokenType::GREATE;
		if (wchar == L'=')
			tok.m_type = TokenType::EQUAL;
		if (wchar == L'/')
			tok.m_type = TokenType::SLASH;
		if (wchar == L',')
			tok.m_type = TokenType::COMMA;

		if (tok.m_type != TokenType::NONE) {
			
			tok.m_column = m_column;
			tok.m_row = m_row;

			m_column++;

			take_char();
			ret = true;
		}

		return ret;

	}
	bool Lexical::getKeyWord(Token& tok)
	{
		bool ret = false;
		std::wstring::iterator roll_back = m_iter;

		tok.m_type = TokenType::NONE;
		std::wstring word = getAsciiName();
		std::transform(word.begin(), word.end(), word.begin(), ::towlower);

		if (word == L"window")
			tok.m_type = TokenType::KEY_WINDOW;
		else
		{
			for (auto& i : m_child_name)
			{
				if (i == word)
				{
					tok.m_type = TokenType::KEY_CHILD;
				}
			}
		}


		if (tok.m_type != TokenType::NONE)
		{
			tok.m_column = m_column;
			tok.m_row = m_row;
			tok.m_buf = word;

			m_column += word.length();

			ret = true;
		}
		else
		{
			m_iter = roll_back;
		}

		return ret;
	}
	bool Lexical::getVariable(Token& tok)
	{
		bool ret = false;

		tok.m_type = TokenType::NONE;
		std::wstring word = getAsciiName();
		if (!word.empty()) {
			std::transform(word.begin(), word.end(), word.begin(), ::towlower);

			tok.m_type = TokenType::VAR_ID;
			tok.m_buf = word;

			tok.m_column = m_column;
			tok.m_row = m_row;

			m_column += word.length();
			ret = true;
		}
		else
		{
			word = getStringConst();
			if (!word.empty())
			{
				tok.m_type = TokenType::String_const;
				tok.m_buf = word;

				tok.m_column = m_column;
				tok.m_row = m_row;

				m_column += word.length();
				ret = true;
			}
		}

		return ret;
	}
	wchar_t Lexical::check_char()
	{
		wchar_t ret = L'\0';
		if (m_iter != m_enditer)
			ret = *m_iter;

		return ret;
	}
	wchar_t Lexical::check_next_char()
	{
		wchar_t ret = L'\0';
		if (m_iter != m_enditer && m_iter + 1 != m_enditer)
			ret = *(m_iter+1);

		return ret;
	}
	void Lexical::take_char()
	{
		if (m_iter != m_enditer) 
		{
			m_iter++;
		}

		return;
	}

	void Lexical::throw_err()
	{
		std::string buf;
		std::stringstream ss;
		ss << "Lexical : unknow token at Row: ";
		ss << m_row;
		ss << " Column: ";
		ss << m_column;
		ss >> buf;
		throw(buf);
	}

	std::wstring Lexical::replace(std::wstring txt, std::wstring sub, std::wstring ne, int count)
	{
		size_t sublens = sub.length();
		int ncount = 0;
		while (count < 0 || count >= 0 && ncount < count)
		{
			size_t step = txt.find(sub);
			if (step != txt.npos) {
				txt.replace(step, sublens, ne);
				ncount++;
				continue;
			}

			break;
		}

		return txt;
	}
	std::wstring Lexical::getAsciiName()
	{
		std::wstring ret;
		wchar_t wchar = check_char();
		while ((wchar >= L'a' && wchar <= 'z') || 
			(wchar >= L'A' && wchar <= L'Z') ||
			wchar == L'_') 
		{
			ret += wchar;
			take_char();
			wchar = check_char();
		}

		return ret;
	}
	std::wstring Lexical::getStringConst()
	{
		std::wstring ret;
		wchar_t wchar = check_char();

		if (wchar == L'"') 
		{
			take_char();
			while (true)
			{
				std::wstring buf;

				// ÅÐ¶Ï½áÊø
				wchar = check_char();
				if (wchar == L'"')
				{
					take_char();
					break;
				}

				// ÅÐ¶Ï µ¥´Ê
				buf = getAsciiName();
				if (!buf.empty()) 
				{
					ret += buf;
					continue;
				}

				// ÅÐ¶Ï Êý×Ö
				buf = getAsciiNumber();
				if (!buf.empty())
				{
					ret += buf;
					continue;
				}

				// ÅÐ¶Ï ¸ñÊ½»¯·ûºÅ
				wchar_t last_char = check_char();
				wchar_t next_char = check_next_char();
				wchar_t double_wchar = L'\0';
				if (last_char == L'\\' && next_char == L'a')
					double_wchar = L'\a';
				else if (last_char == L'\\' && next_char == L'b')
					double_wchar = L'\b';
				else if (last_char == L'\\' && next_char == L'f')
					double_wchar = L'\f';
				else if (last_char == L'\\' && next_char == L'r')
					double_wchar = L'\r';
				else if (last_char == L'\\' && next_char == L'n')
					double_wchar = L'\n';
				else if (last_char == L'\\' && next_char == L't')
					double_wchar = L'\t';
				else if (last_char == L'\\' && next_char == L'v')
					double_wchar = L'\v';
				else if (last_char == L'\\' && next_char == L'\\')
					double_wchar = L'\\';
				else if (last_char == L'\\' && next_char != L'\0')
					double_wchar = next_char;

				if (double_wchar != L'\0')
				{
					ret += double_wchar;
					take_char();
					take_char();
					continue;
				}
				else if (last_char == L'\\' && (next_char == L'\0' || next_char == L' '))
				{
					throw_err();
					return ret;
				}

				ret += wchar;
				take_char();
			}
		}

		return ret;
	}
	std::wstring Lexical::getAsciiNumber()
	{
		std::wstring ret;
		wchar_t wchar = check_char();
		while ((wchar >= L'0' && wchar <= '9'))
		{
			ret += wchar;
			take_char();
			wchar = check_char();
		}

		return ret;
	}
}