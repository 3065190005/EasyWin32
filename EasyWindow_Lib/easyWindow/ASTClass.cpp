#include "ASTClass.h"

using namespace easyWindow;

easyWindow::BodyAst::BodyAst(ASTClass* attribute) :
	m_Attribute(nullptr),
	m_Child(nullptr)
{
	m_ast_type = AstType::BodyAst;
	m_Attribute = attribute;
}

easyWindow::BodyAst::~BodyAst()
{
	if (m_Attribute)
	{
		delete m_Attribute;
		m_Attribute = nullptr;
	}

	if (m_Child)
	{
		delete m_Child;
		m_Child = nullptr;
	}
}

ASTClass* easyWindow::BodyAst::GetAttribute()
{
	return m_Attribute;
}

ASTClass* easyWindow::BodyAst::GetChild()
{
	return m_Child;
}

void easyWindow::BodyAst::SetChild(ASTClass* ast)
{
	if (m_Child) 
	{
		delete m_Child;
	}
	m_Child = ast;
}

easyWindow::ProgramAst::ProgramAst(ASTClass* body)
	:
	m_body(nullptr)
{
	m_ast_type = AstType::ProgramAst;
	m_body = body;
}

easyWindow::ProgramAst::~ProgramAst()
{
	if (m_body)
	{
		delete m_body;
		m_body = nullptr;
	}
}

easyWindow::ASTClass* easyWindow::ProgramAst::GetBody()
{
	return m_body;
}

easyWindow::ChildAst::ChildAst(ASTClass* attribute) :
	m_Attribute(nullptr),
	m_NextChild(nullptr)
{
	m_ast_type = AstType::ChildAst;
	m_Attribute = attribute;
}

easyWindow::ChildAst::~ChildAst()
{
	if (m_Attribute)
	{
		delete m_Attribute;
		m_Attribute = nullptr;
	}

	if (m_NextChild)
	{
		delete m_NextChild;
		m_NextChild = nullptr;
	}

	for (auto& i : m_MyChilds)
	{
		delete i;
		i = nullptr;
	}
	m_MyChilds.clear();
}

ASTClass* easyWindow::ChildAst::GetAttribute()
{
	return m_Attribute;
}

void easyWindow::ChildAst::SetNextChild(ASTClass* next)
{
	if (m_NextChild)
	{
		delete m_NextChild;
	}

	m_NextChild = next;
}

ASTClass* easyWindow::ChildAst::GetNextChild()
{
	return m_NextChild;
}

void easyWindow::ChildAst::SetMyChilds(std::list<ASTClass*> childs)
{
	for (auto& i : m_MyChilds)
	{
		delete i;
	}

	m_MyChilds = childs;
}

std::list<ASTClass*> easyWindow::ChildAst::GetMyChilds()
{
	return m_MyChilds;
}

void easyWindow::ChildAst::SetTokBuf(std::wstring buf)
{
	m_token_buf = buf;
}

easyWindow::AttributeAst::AttributeAst()
{
}

easyWindow::AttributeAst::~AttributeAst()
{
}

AttributeMap easyWindow::AttributeAst::GetMap()
{
	return m_map;
}

void easyWindow::AttributeAst::SetMap(AttributeMap map)
{
	m_map = map;
}
