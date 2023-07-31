#include "Parser.h"

#include <sstream>

namespace easyWindow
{

#include "InitMap.h";

	Parser::Parser(Lexical::TokenList toks)
	{
		m_tokens		= toks;
		m_current		= m_tokens.begin();
		m_current_end	= m_tokens.end();
	}

	Parser::~Parser()
	{
	}

	std::shared_ptr<ASTClass> Parser::Ast()
	{
		ASTClass* astList = nullptr;
		astList = Program();

		std::shared_ptr<ASTClass> ret(astList);
		return ret;
	}

	ASTClass* Parser::Program()
	{
		ASTClass* body_ast = Body();
		ProgramAst* ret = new ProgramAst(body_ast);
		TakeEat(TokenType::KEY_EOF);
		return ret;
	}

	ASTClass* Parser::Body()
	{

		TakeEat(TokenType::LESS);
		auto tok = TakeEat(TokenType::KEY_WINDOW);
		TakeEat(TokenType::GREATE);
		ASTClass* attribute = Attribute(tok);
		ASTClass* child = nullptr;

		// childs
		if (!(CheckToken(TokenType::LESS) && CheckNextToken(TokenType::SLASH)))
		{
			child = Child();
		}

		TakeEat(TokenType::LESS);
		TakeEat(TokenType::SLASH);
		TakeEat(TokenType::KEY_WINDOW);
		TakeEat(TokenType::GREATE);

		BodyAst* ret = new BodyAst(attribute);
		if (child)
			ret->SetChild(child);

		return ret;
	}

	ASTClass* Parser::Child()
	{
		TakeEat(TokenType::LESS);
		auto token = TakeEat(TokenType::KEY_CHILD);
		TakeEat(TokenType::GREATE);

		ASTClass* attribute = Attribute(token);
		std::list<ASTClass*> childs;
		ASTClass* next_child = nullptr;

		// childs
		while ((!(CheckToken(TokenType::LESS) && CheckNextToken(TokenType::SLASH))))
		{
			childs.push_back(Child());
		} 

		TakeEat(TokenType::LESS);
		TakeEat(TokenType::SLASH);
		auto end_tok = TakeEat(TokenType::KEY_CHILD);
		if (end_tok.m_buf != token.m_buf) 
		{
			Throw_err(token);
			return nullptr;
		}

		// next child
		TakeEat(TokenType::GREATE);
		if ((!(CheckToken(TokenType::LESS) && CheckNextToken(TokenType::SLASH))))
		{
			next_child = Child();
		}

		ChildAst* ret = new ChildAst(attribute);
		ret->SetMyChilds(childs);
		ret->SetNextChild(next_child);
		ret->SetTokBuf(token.m_buf);
		return ret;
	}

	ASTClass* Parser::Attribute(Token key_type)
	{
		AttributeMap map = GetInitMap(key_type.m_type,key_type.m_buf);
		TakeEat(TokenType::LBRACKET);
		if (!CheckToken(TokenType::RBRACKET))
		{
			auto newmap = Element();
			for (auto i : newmap)
			{
				map[i.first] = i.second;
			}
		}
		TakeEat(TokenType::RBRACKET);

		AttributeAst* ret = new AttributeAst();
		ret->SetMap(map);
		return ret;
	}

	AttributeMap Parser::Element()
	{
		AttributeMap ret;
		auto var_tok = TakeEat(TokenType::VAR_ID);
		TakeEat(TokenType::EQUAL);
		auto value_tok = TakeEat(TokenType::String_const);

		if (CheckToken(TokenType::COMMA))
		{
			TakeEat(TokenType::COMMA);
			AttributeMap next_map = Element();
			for (auto& i : next_map)
			{
				ret[i.first] = i.second;
			}
		}

		ret[var_tok.m_buf] = value_tok.m_buf;
		return ret;
	}

	AttributeMap Parser::GetInitMap(TokenType key_type, std::wstring name)
	{
		AttributeMap ret;
		if (key_type == TokenType::KEY_WINDOW)
			ret = InitMap::attributeNode[L"window"];
		else if (key_type == TokenType::KEY_CHILD)
			ret = InitMap::attributeNode[name];

		return ret;
	}

	bool Parser::CheckToken(TokenType type)
	{
		if (m_current != m_current_end)
		{
			Token ret = *m_current;
			if (ret.m_type == type)
				return true;
		}

		return false;
	}

	bool Parser::CheckNextToken(TokenType type)
	{
		if (m_current != m_current_end)
		{
			auto next_iter = m_current;
			next_iter++;
			if (next_iter != m_current_end)
			{
				Token ret = *next_iter;
				if (ret.m_type == type)
					return true;
			}
		}

		return false;
	}

	Parser::Token Parser::TakeEat(TokenType type)
	{
		static Token ret;
		bool result = false;
		if (m_current != m_current_end)
		{
			ret = *m_current;
			if (ret.m_type == type)
				result = true;
			m_current++;
		}

		if (!result)
			Throw_err(ret);

		return ret;
	}

	void Parser::Throw_err(Token tok)
	{
		std::stringstream ss;
		ss << "Parser : unknow Token at row : ";
		ss << tok.m_row;
		ss << " column : ";
		ss << tok.m_column;

		std::string buf;
		ss >> buf;
		throw(buf);
	}
}
