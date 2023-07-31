#include "ApiWindow.h"

namespace easyWindow
{
    ApiWindow::ApiWindow(std::wstring text)
    {
        m_text = text;
    }

    std::shared_ptr<ASTClass> ApiWindow::getAst()
    {
        easyWindow::Lexical lexical(m_text);
        auto tokens = lexical.tokens();

        easyWindow::Parser parser(tokens);
        auto ast_tree = parser.Ast();

        return ast_tree;
    }

}
