#pragma once
#ifndef __EASY_WINDOW_PARSER__
#define __EASY_WINDOW_PARSER__

#include "Lexical.h"
#include "ASTClass.h"

#include <memory>

namespace easyWindow 
{

	class Parser final
	{
	public:
		using TokenList = Lexical::TokenList;
		using Token = Lexical::Token;
		using TokenType = Lexical::TokenType;

	public:
		explicit Parser(Lexical::TokenList toks);
		~Parser();

	public:
		std::shared_ptr<ASTClass> Ast();	// �������

	public:
		ASTClass* Program();					// ���� Program �ڵ�
		ASTClass* Body();						// ���� Body �ڵ�
		ASTClass* Child();						// ���� Child �ڵ�
		ASTClass* Attribute(Token);				// ���� Attribute �ڵ�

		AttributeMap Element();					// ���� Element �ڵ�
	public:
		AttributeMap GetInitMap(TokenType, std::wstring);	// ��ȡ��ʼ������

	private:
		bool CheckToken(TokenType type);				// ���Ե�ǰtok�Ƿ����
		bool CheckNextToken(TokenType type);			// ������һ��tok�Ƿ����
		Token TakeEat(TokenType type);					// ��ȡ��������ǰtoken
		void Throw_err(Token tok);						// ������
	private:
		TokenList m_tokens;						// ������token�б�
		TokenList::iterator m_current;			// ��ǰtoken
		TokenList::iterator m_current_end;		// token��β
	};

}



#endif // !__EASY_WINDOW_PARSER__

