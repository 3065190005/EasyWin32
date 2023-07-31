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
		std::shared_ptr<ASTClass> Ast();	// 解析入口

	public:
		ASTClass* Program();					// 解析 Program 节点
		ASTClass* Body();						// 解析 Body 节点
		ASTClass* Child();						// 解析 Child 节点
		ASTClass* Attribute(Token);				// 解析 Attribute 节点

		AttributeMap Element();					// 解析 Element 节点
	public:
		AttributeMap GetInitMap(TokenType, std::wstring);	// 获取初始化属性

	private:
		bool CheckToken(TokenType type);				// 测试当前tok是否符合
		bool CheckNextToken(TokenType type);			// 测试下一个tok是否符合
		Token TakeEat(TokenType type);					// 获取并丢掉当前token
		void Throw_err(Token tok);						// 报错处理
	private:
		TokenList m_tokens;						// 待处理token列表
		TokenList::iterator m_current;			// 当前token
		TokenList::iterator m_current_end;		// token结尾
	};

}



#endif // !__EASY_WINDOW_PARSER__

