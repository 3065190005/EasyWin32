#pragma once
#ifndef __EASY_WINDOW_ASTCLASS__
#define __EASY_WINDOW_ASTCLASS__

#include "Lexical.h"
#include <map>
#include <list>

namespace easyWindow
{
	// 基础节点
	class ASTClass
	{
	public:
		enum class AstType {
			None = 0,
			ProgramAst,			// 程序开始节点
			BodyAst,			// 主体节点
			ChildAst,			// 窗口节点
			AttributeAst,		// 属性节点
			ElementAst			// 属性值
		};
	public:
		using TokenType = Lexical::TokenType;
		
	public:
		ASTClass():
		m_token_row(0),
		m_token_column(0),
		m_token_type(TokenType::NONE),
		m_token_buf(L""),
		m_ast_type(AstType::None)
			{};
		virtual ~ASTClass() {};

	public:
		virtual AstType getAstType() { return m_ast_type; }

		virtual TokenType getTokenType() { return m_token_type; };
		virtual size_t getTokenRow() { return m_token_row; }
		virtual size_t getTokenColumn() { return m_token_column; }
		virtual std::wstring getTokenBuf() { return m_token_buf; }

	protected:
		AstType m_ast_type;

	protected:
		size_t m_token_row;
		size_t m_token_column;
		TokenType m_token_type;
		std::wstring m_token_buf;
		
	};


	// Program 节点
	class ProgramAst : public ASTClass 
	{
	public:
		ProgramAst(ASTClass* body);
		~ProgramAst();

		ASTClass* GetBody();

	private:
		ASTClass* m_body;
	};



	// Body 节点
	class BodyAst : public ASTClass
	{
	public:
		BodyAst(ASTClass* attribute);
		~BodyAst();

		ASTClass* GetAttribute();
		
		ASTClass* GetChild();
		void SetChild(ASTClass*);

	private:
		ASTClass* m_Attribute;
		ASTClass* m_Child;
	};

	// Child 节点
	class ChildAst : public ASTClass
	{
	public:
		ChildAst(ASTClass* attribute);
		~ChildAst();

		ASTClass* GetAttribute();

		void SetNextChild(ASTClass* next);
		ASTClass* GetNextChild();

		void SetMyChilds(std::list<ASTClass*> childs);
		std::list<ASTClass*> GetMyChilds();
		
		void SetTokBuf(std::wstring);

	private:
		ASTClass* m_Attribute;
		ASTClass* m_NextChild;
		std::list<ASTClass*> m_MyChilds;
	};


	using AttributeMap = std::map<std::wstring, std::wstring>;

	// Attribute 节点
	class AttributeAst : public ASTClass
	{
	public:
		AttributeAst();
		~AttributeAst();

		AttributeMap GetMap();
		void SetMap(AttributeMap);

	private:
		AttributeMap m_map;
	};
}

#endif

