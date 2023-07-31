#include "VmWindow.h"
#include "Menu.h"
#include <objbase.h>

#include <sstream>
#include <algorithm>

namespace easyWindow
{
    easyWindow::VmWindow::VmWindow(std::shared_ptr<ASTClass> ast)
    {
        m_ast_tree = ast;
        m_loop.SetWindowHwndList(&m_window_hwnd);
        m_loop.SetWindowManager(&m_manager);
    }

    easyWindow::VmWindow::~VmWindow()
    {
        m_init = false;
        for (auto& i : m_window_hwnd)
        {
            m_manager.RemoveHwndMap(i.self);
        }
        m_window_hwnd.clear();
    }

    bool easyWindow::VmWindow::init()
    {
        if (m_init)
            return true;

        AstType type = m_ast_tree->getAstType();
        if (type == AstType::ProgramAst)
        {
            auto ast = m_ast_tree.get();
            bool result = VmProgram(ast);
            if (result) {
                loop();
                m_init = true;
            }
            else
            {
                for (auto& i : m_window_hwnd)
                {
                    m_manager.RemoveHwndMap(i.self);
                }
                m_window_hwnd.clear();
            }

            return result;
        }

        return false;
    }

    bool easyWindow::VmWindow::loop()
    {
        bool ret = false;
        for (auto& i : m_window_hwnd)
        {

            std::wstring id = i.attribute[L"id"];
            for (auto& j : m_auto_list)
            {
                if (id == std::get<0>(j))
                {
                    m_loop.updateAutoRect(i.self, std::get<1>(j), std::get<2>(j));
                }
            }

            int show = SW_HIDE;
            if (i.attribute[L"visible"] == L"true")
                show = SW_SHOW;

            ShowWindow(i.self, show);

            MSG msg;
            if (GetMessage(&msg, NULL, 0, 0))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                ret = true;
            }
            else
            {
                ret = false;
                break;
            }

            // 动态更新属性
            m_loop.loopUpdateAttribute(i);
        }

        return ret;
    }

    AttributeMap easyWindow::VmWindow::getAttributeById(std::wstring id)
    {
        AttributeMap ret;
        if (id.empty())
            return ret;

        for (auto& i : m_window_hwnd)
        {
            if (i.attribute.find(L"id") != i.attribute.end())
            {
                if (i.attribute[L"id"] == id)
                {
                    ret = i.attribute;
                    break;
                }
            }
        }
        return ret;
    }

    void VmWindow::setAttributeById(std::wstring id, AttributeMap map)
    {
        if (id.empty())
            return;

        for (auto& i : m_window_hwnd)
        {
            if (i.attribute.find(L"id") != i.attribute.end())
            {
                if (i.attribute[L"id"] == id)
                {
                    AttributeMap sources = i.attribute;

                    for (auto& i : map)
                    {
                        std::wstring name = i.first;
                        std::transform(name.begin(), name.end(), name.begin(), towlower);
                        sources[name] = i.second;
                    }

                    m_loop.updateWithAttribute(sources, i.attribute, i.self);
                    SetWindowPos(i.self, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
                    i.attribute = sources;
                    return;
                }
            }
        }
    }

    void VmWindow::setWindowAutoRect(std::wstring id, AutoRect type, long interval)
    {
        if (interval < 0)
            interval = 0;

        if(type == AutoRect::none)
        {
            for (auto it = m_auto_list.begin(); it != m_auto_list.end(); it++)
            {
                m_auto_list.erase(it);
                return;
            }
        }
        else
        {
            AutoType element;
            std::get<0>(element) = id;
            std::get<1>(element) = type;
            std::get<2>(element) = interval;
            
            m_auto_list.push_back(element);
        }
    }

    bool VmWindow::hasMessage()
    {
        bool message = !m_manager.Message.empty();
        bool string = !m_manager.wStringMessage.empty();
        
        return message || string;
    }

    std::wstring VmWindow::getMessage()
    {
        if (m_manager.Message.empty() && m_manager.wStringMessage.empty()) {
            throw("no more Message Code");
            return L"";
        }

        std::wstring code;
        if (!m_manager.Message.empty())
        {
            auto hwnd = m_manager.Message.front();
            m_manager.Message.pop();
            code = hwnd.second;
            for (auto& i : m_window_hwnd)
            {
                if (i.self == hwnd.first)
                {
                    code = i.attribute[L"onclick"];
                }
            }
        }

        else if (!m_manager.wStringMessage.empty())
        {
            code = m_manager.wStringMessage.front();
            m_manager.wStringMessage.pop_front();
        }


        return code;
    }

    void VmWindow::clearMessage()
    {
        while (!m_manager.Message.empty())
            m_manager.Message.pop();
    }

    bool easyWindow::VmWindow::VmProgram(ASTClass* program)
    {
        ProgramAst* ast = (ProgramAst*)program;

        auto body = ast->GetBody();
        if (body)
            return VmBody(body);
        return true;
    }

    bool easyWindow::VmWindow::VmBody(ASTClass* body)
    {
        BodyAst* ast = (BodyAst*)body;
        ChildAst* child = (ChildAst*)ast->GetChild();
        AttributeAst* attribute = (AttributeAst*)ast->GetAttribute();
        AttributeMap map = attribute->GetMap();

        WindowNode node;
        HWND win_hwnd = m_manager.Window(map);
        node.attribute = map;
        node.parent = NULL;
        node.self = win_hwnd;
        node.name = L"window";
        node.type = TokenType::KEY_WINDOW;

        if (win_hwnd == NULL)
            return false;

        m_window_hwnd.push_back(node);

        if (child)
            return VmChild(child, win_hwnd);

        return true;
    }

    bool easyWindow::VmWindow::VmChild(ASTClass* child, HWND parent)
    {
        ChildAst* ast = (ChildAst*)child;
        auto MyChild = ast->GetMyChilds();
        ChildAst* next_child = (ChildAst*)ast->GetNextChild();
        AttributeAst* attribute = (AttributeAst*)ast->GetAttribute();
        AttributeMap map = attribute->GetMap();

        // Self
        WindowNode node;
        HWND win_hwnd = NULL;

        std::wstring childBuf = ast->getTokenBuf();
        if (childBuf == L"button")
            win_hwnd = m_manager.Button(map, parent);
        if (childBuf == L"edit")
            win_hwnd = m_manager.Edit(map, parent);
        if(childBuf == L"combobox")
            win_hwnd = m_manager.ComboBox(map, parent);
        if(childBuf == L"layer")
            win_hwnd = m_manager.Layer(map, parent);
        if (childBuf == L"progress")
            win_hwnd = m_manager.Progress(map, parent);
        if (childBuf == L"scroll")
            win_hwnd = m_manager.Scroll(map, parent);
        if (childBuf == L"listbox")
            win_hwnd = m_manager.ListBox(map, parent);
        if (childBuf == L"static")
            win_hwnd = m_manager.Static(map, parent);
        if (childBuf == L"image")
            win_hwnd = m_manager.Image(map, parent);

        node.attribute = map;
        node.parent = parent;
        node.self = win_hwnd;
        node.name = childBuf;
        node.type = TokenType::KEY_CHILD;

        if (win_hwnd == NULL)
            return false;

        m_window_hwnd.push_back(node);

        // MyChild
        for (auto& i : MyChild)
            if (!VmChild(i, win_hwnd))
                return false;

        if (next_child)
            return VmChild(next_child, parent);

        return true;
    }


    bool VmMemery::Realloc(size_t lens)
    {
        if (m_memery || m_memlens < lens)
        {
            if (m_memery)
                delete m_memery;
            m_memery = new wchar_t[lens];
        }

        if (m_memery)
            return true;

        return false;
    }

    // -------- VmLoop

    VmLoop::VmLoop()
    {
    }
    VmLoop::~VmLoop()
    {
    }

    BOOL VmLoop::enumChildFinder(HWND hwndChild, LPARAM lParam)
    {
        ((VmLoop*)lParam)->m_child_list.push_back(hwndChild);
        return TRUE;
    }

    long double VmLoop::wStringToNumber(std::wstring str)
    {
        long double ret = 0;
        std::wstringstream ss;
        ss << str;
        ss >> ret;
        return ret;
    }

    std::wstring VmLoop::NumberTowString(long number)
    {
        std::wstring ret;
        std::wstringstream ss;
        ss << number;
        ss >> ret;
        return ret;
    }

    void VmLoop::SetWindowHwndList(std::list<WindowNode>* list)
    {
        m_window_hwnd = list;
    }

    void VmLoop::SetWindowManager(WindowManager* manager)
    {
        m_manager = manager;
    }

    std::list<std::wstring> VmLoop::splitwString(std::wstring str, wchar_t split)
    {
        std::list<std::wstring> res;
        if (str == L"")		return res;
        //在字符串末尾也加入分隔符，方便截取最后一段
        std::wstring strs = str + split;
        size_t pos = strs.find(split);

        // 若找不到内容则字符串搜索函数返回 npos
        while (pos != strs.npos)
        {
            std::wstring temp = strs.substr(0, pos);
            res.push_back(temp);
            //去掉已分割的字符串,在剩下的字符串中进行分割
            strs = strs.substr(pos + 1, strs.size());
            pos = strs.find(split);
        }

        return res;
    }
    void VmLoop::updateAutoRect(HWND hwnd, VmClassStruct::enumAutoRect type,long interval)
    {
        RECT rect;
        GetClientRect(hwnd, &rect);
        EnumChildWindows(hwnd, VmLoop::enumChildFinder, (LPARAM)this);

        // 非递归获取子窗口
        for (auto i = m_child_list.begin() ; i != m_child_list.end() ; i ++)
        {
            if (GetParent(*i) != hwnd)
            {
                m_child_list.erase(i);
                i = m_child_list.begin();
            }
        }

        // 开始排序
        size_t child_count = m_child_list.size();
        if (child_count == 0)
            return;
        
        if (type == VmClassStruct::enumAutoRect::horz)
        {
            rect.right -= ((child_count + 1) * interval);
            long offset = rect.right /= child_count;
            long posx = interval;
            for (auto& child : m_child_list)
            {
                SetWindowPos(child, HWND_TOP, posx, 0, offset, rect.bottom, SWP_NOZORDER | SWP_FRAMECHANGED);
                posx += offset + interval;
            }
        }
        else if (type == VmClassStruct::enumAutoRect::vert)
        {
            rect.bottom -= ((child_count + 1) * interval);
            long offset = rect.bottom /= child_count;
            long posy = interval;
            for (auto& child : m_child_list)
            {
                SetWindowPos(child, HWND_TOP, 0, posy, rect.right, offset, SWP_NOZORDER | SWP_FRAMECHANGED);
                posy += offset + interval;
            }
        }

        m_child_list.clear();
    }
    void VmLoop::updateWithAttribute(AttributeMap map, AttributeMap old, HWND hwnd)
    {

        wchar_t name_c[1024]{ 0 };
        GetClassName(hwnd, name_c, 1024);
        std::wstring name(name_c);

        if (name == L"Edit")
            Attribute_update_edit(map, hwnd);
        else if (name == L"Button")
            Attribute_update_button(map, hwnd);
        else if (name == L"ComboBox")
            Attribute_update_combobox(map, hwnd);
        else if (name == L"Layer")
            Attribute_update_layer(map, hwnd);
        else if (name == L"msctls_progress32")
            Attribute_update_progress(map, old, hwnd);
        else if (name == L"scroll_s")
            Attribute_update_scroll(map, old, hwnd);
        else if (name == WC_LISTBOX)
            Attribute_update_listbox(map, old, hwnd);
        else if (name == WC_STATIC)
            Attribute_update_static(map, old, hwnd);
        else if (name == L"Image_c")
            Attribute_update_image(map, old, hwnd);
        else
            Attribute_update_window(map, hwnd);
    }

    void VmLoop::Attribute_update_window(AttributeMap map, HWND hwnd)
    {
        // visible
        std::wstring visible = map[L"visible"];
        ShowWindow(hwnd, SW_HIDE);
        if (visible == L"true")
            ShowWindow(hwnd, SW_SHOW);

        // x y w h 
        std::wstring x = map[L"x"];
        std::wstring y = map[L"y"];
        std::wstring w = map[L"w"];
        std::wstring h = map[L"h"];
        MoveWindow(hwnd, wStringToNumber(x), wStringToNumber(y), wStringToNumber(w), wStringToNumber(h), TRUE);

        // text
        std::wstring text = map[L"text"];
        SetWindowText(hwnd, text.c_str());

        // style
        std::wstring style = map[L"style"];
        SetWindowLong(hwnd, GWL_STYLE, wStringToNumber(style));

        // fullscreen
        std::wstring fullscreen = map[L"fullscreen"];
        if (fullscreen == L"true")
        {
            SendMessage(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, NULL);
        }
        else
            SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, NULL);

        // enable
        std::wstring enable = map[L"enable"];
        EnableWindow(hwnd, enable == L"true");

        // resize
        std::wstring resize = map[L"resize"];
        long resize_long = GetWindowLong(hwnd, GWL_STYLE);
        resize_long &= ~WS_THICKFRAME;
        if (resize == L"true")
            resize_long |= WS_THICKFRAME;
        SetWindowLong(hwnd, GWL_STYLE, resize_long);

        // maxbox
        std::wstring maxbox = map[L"maxbox"];
        long maxbox_long = GetWindowLong(hwnd, GWL_STYLE);
        maxbox_long &= ~WS_MAXIMIZEBOX;
        if (maxbox == L"true")
            maxbox_long |= WS_MAXIMIZEBOX;
        SetWindowLong(hwnd, GWL_STYLE, maxbox_long);

        // minbox
        std::wstring minbox = map[L"minbox"];
        long minbox_long = GetWindowLong(hwnd, GWL_STYLE);
        minbox_long &= ~WS_MINIMIZEBOX;
        if (minbox == L"true")
            minbox_long |= WS_MINIMIZEBOX;
        SetWindowLong(hwnd, GWL_STYLE, minbox_long);


        // menu
        std::wstring menu = map[L"menu"];
        if (!menu.empty() && hwnd)
        {
            Menu menu_cls(menu);
            HMENU menu_ptr = menu_cls.get();
            if (menu_ptr != nullptr)
            {
                SetMenu(hwnd, menu_ptr);
            }
        }

    }

    void VmLoop::Attribute_update_button(AttributeMap map, HWND hwnd)
    {
        for (auto& i : *m_window_hwnd)
        {
            if (i.self == hwnd)
            {
                m_manager->RemoveHwndMap(hwnd);
                i.self = m_manager->Edit(map, i.parent);
                break;
            }
        }
    }

    void VmLoop::Attribute_update_edit(AttributeMap map, HWND hwnd)
    {
        for (auto& i : *m_window_hwnd)
        {
            if (i.self == hwnd)
            {
                m_manager->RemoveHwndMap(hwnd);
                i.self = m_manager->Edit(map, i.parent);
                break;
            }
        }
    }

    void VmLoop::Attribute_update_combobox(AttributeMap map, HWND hwnd)
    {
        // visible
        std::wstring visible = map[L"visible"];
        ShowWindow(hwnd, SW_HIDE);
        if (visible == L"true")
            ShowWindow(hwnd, SW_SHOW);

        // x y w h 
        std::wstring x = map[L"x"];
        std::wstring y = map[L"y"];
        std::wstring w = map[L"w"];
        std::wstring h = map[L"h"];
        MoveWindow(hwnd, wStringToNumber(x), wStringToNumber(y), wStringToNumber(w), wStringToNumber(h), TRUE);

        // style
        std::wstring style = map[L"style"];
        SetWindowLong(hwnd, GWL_STYLE, wStringToNumber(style));

        // list
        ComboBox_ResetContent(hwnd);
        std::wstring list = map[L"list"];
        auto lists = splitwString(list, L'\n');
        for (auto& i : lists) {
            if (!i.empty())
                ComboBox_AddString(hwnd, i.c_str());
        }

        // enable
        std::wstring enable = map[L"enable"];
        bool enable_bool = (enable == L"true");
        ComboBox_Enable(hwnd, enable_bool);

        // current
        std::wstring current = map[L"current"];
        if (!current.empty())
            ComboBox_SelectString(hwnd, -1, current.c_str());

        // banner
        std::wstring banner = map[L"banner"];
        ComboBox_SetCueBannerText(hwnd, banner.c_str());

        // fontfamily fontsize 
        std::wstring font_family = map[L"fontfamily"];
        std::wstring font_size = map[L"fontsize"];

        LONG font_size_num = wStringToNumber(font_size);
        HFONT hfont;
        LOGFONT logfont;

        GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &logfont);
        logfont.lfHeight = -MulDiv(font_size_num, GetDeviceCaps(GetDC(hwnd), LOGPIXELSY), 72);
        logfont.lfWeight = -MulDiv(font_size_num, GetDeviceCaps(GetDC(hwnd), LOGPIXELSY), 72);
        wsprintf(logfont.lfFaceName, TEXT("%s"), font_family.c_str());
        hfont = CreateFontIndirect(&logfont);
        SendMessage(hwnd, WM_SETFONT, WPARAM(hfont), TRUE);
    }

    void VmLoop::Attribute_update_layer(AttributeMap map, HWND hwnd)
    {
        // visible
        std::wstring visible = map[L"visible"];
        ShowWindow(hwnd, SW_HIDE);
        if (visible == L"true")
            ShowWindow(hwnd, SW_SHOW);

        // x y w h 
        std::wstring x = map[L"x"];
        std::wstring y = map[L"y"];
        std::wstring w = map[L"w"];
        std::wstring h = map[L"h"];
        MoveWindow(hwnd, wStringToNumber(x), wStringToNumber(y), wStringToNumber(w), wStringToNumber(h), TRUE);

        // style
        std::wstring style = map[L"style"];
        SetWindowLong(hwnd, GWL_STYLE, wStringToNumber(style));

        // enable
        std::wstring enable = map[L"enable"];
        EnableWindow(hwnd, enable == L"true");

        // resize
        std::wstring resize = map[L"resize"];
        long resize_long = GetWindowLong(hwnd, GWL_STYLE);
        resize_long &= ~WS_THICKFRAME;
        if (resize == L"true")
            resize_long |= WS_THICKFRAME;
        SetWindowLong(hwnd, GWL_STYLE, resize_long);

        // frame
        std::wstring frame = map[L"frame"];
        long frame_long = GetWindowLong(hwnd, GWL_STYLE);
        frame_long &= ~WS_DLGFRAME;
        if (frame == L"true")
            frame_long |= WS_DLGFRAME;
        SetWindowLong(hwnd, GWL_STYLE, frame_long);


        // border
        std::wstring border = map[L"border"];
        long border_long = GetWindowLong(hwnd, GWL_STYLE);
        border_long &= ~WS_BORDER;
        if (border == L"true")
            border_long |= WS_BORDER;
        SetWindowLong(hwnd, GWL_STYLE, border_long);


        // acceptfile
        std::wstring acceptfile = map[L"acceptfile"];
        long acceptfile_long = GetWindowLong(hwnd, GWL_EXSTYLE);
        acceptfile_long &= ~WS_EX_ACCEPTFILES;
        if (acceptfile == L"true")
            acceptfile_long |= WS_EX_ACCEPTFILES;
        SetWindowLong(hwnd, GWL_EXSTYLE, acceptfile_long);

        // color
        std::wstring color = map[L"color"];
        DWORD color_int = 0;
        if (color.size() == 6)
        {
            std::wstringstream ss;
            ss << std::hex << color;
            ss >> color_int;

            if (color_int == 0)
                color_int = 0;

            SendMessage(hwnd, 101, color_int, NULL);
        }
        
        SendMessage(hwnd, WM_PAINT, NULL, NULL);
    }

    void VmLoop::Attribute_update_progress(AttributeMap map,AttributeMap old, HWND hwnd)
    {
        // visible
        if (map[L"visible"] != old[L"visible"])
        {
            std::wstring visible = map[L"visible"];
            ShowWindow(hwnd, SW_HIDE);
            if (visible == L"true")
                ShowWindow(hwnd, SW_SHOW);
        }


        // x y w h 
        std::wstring x = map[L"x"];
        std::wstring y = map[L"y"];
        std::wstring w = map[L"w"];
        std::wstring h = map[L"h"];
        MoveWindow(hwnd, wStringToNumber(x), wStringToNumber(y), wStringToNumber(w), wStringToNumber(h), TRUE);

        // style
        if (map[L"style"] != old[L"style"]) {
            std::wstring style = map[L"style"];
            SetWindowLong(hwnd, GWL_STYLE, wStringToNumber(style));
        }

        // resize
        if (map[L"resize"] != old[L"resize"]) {
            std::wstring resize = map[L"resize"];
            long resize_long = GetWindowLong(hwnd, GWL_STYLE);
            resize_long &= ~WS_THICKFRAME;
            if (resize == L"true")
                resize_long |= WS_THICKFRAME;
            SetWindowLong(hwnd, GWL_STYLE, resize_long);
        }

        // frame
        if (map[L"frame"] != old[L"frame"]) {
            std::wstring frame = map[L"frame"];
            long frame_long = GetWindowLong(hwnd, GWL_STYLE);
            frame_long &= ~WS_DLGFRAME;
            if (frame == L"true")
                frame_long |= WS_DLGFRAME;
            SetWindowLong(hwnd, GWL_STYLE, frame_long);
        }

        // max min

        if (map[L"max"] != old[L"max"] || map[L"min"] != old[L"min"]) {
            std::wstring max = map[L"max"];
            std::wstring min = map[L"min"];
            DWORD  max_long = wStringToNumber(max);
            DWORD  min_long = wStringToNumber(min);

            SendMessage(hwnd, PBM_SETRANGE, 0, MAKELPARAM(min_long, max_long));
        }

        // pos
        if (map[L"pos"] != old[L"pos"]) {
            std::wstring pos = map[L"pos"];
            long pos_long = wStringToNumber(pos);

            SendMessage(hwnd, PBM_SETPOS, pos_long, NULL);
        }


        // step
        if (map[L"step"] != old[L"step"]) {
            std::wstring step = map[L"step"];
            long step_long = wStringToNumber(step);
            SendMessageW(hwnd, PBM_SETSTEP, step_long, NULL);
        }
        
        if (map[L"update"] == L"true") {
            // update all attribute
            SendMessageW(hwnd, PBM_STEPIT, 0, 0);
        }
    }

    void VmLoop::Attribute_update_scroll(AttributeMap map, AttributeMap old, HWND hwnd)
    {
        // visible
        if (map[L"visible"] != old[L"visible"])
        {
            std::wstring visible = map[L"visible"];
            ShowWindow(hwnd, SW_HIDE);
            if (visible == L"true")
                ShowWindow(hwnd, SW_SHOW);
        }


        // x y w h 
        std::wstring x = map[L"x"];
        std::wstring y = map[L"y"];
        std::wstring w = map[L"w"];
        std::wstring h = map[L"h"];
        MoveWindow(hwnd, wStringToNumber(x), wStringToNumber(y), wStringToNumber(w), wStringToNumber(h), TRUE);

        // style
        if (map[L"style"] != old[L"style"]) {
            std::wstring style = map[L"style"];
            SetWindowLong(hwnd, GWL_STYLE, wStringToNumber(style));
        }


        // max min pos
        std::wstring vmin = map[L"vmin"];
        std::wstring vmax = map[L"vmax"];
        std::wstring vpos = map[L"vpos"];
        std::wstring hmin = map[L"hmin"];
        std::wstring hmax = map[L"hmax"];
        std::wstring hpos = map[L"hpos"];

        SCROLLINFO scinfo;

        scinfo.cbSize = sizeof(SCROLLINFO);
        scinfo.fMask = SIF_ALL;
        scinfo.nPage = 0;

        scinfo.nMin = wStringToNumber(vmin);
        scinfo.nMax = wStringToNumber(vmax);
        scinfo.nPos = wStringToNumber(vpos);
        SetScrollInfo(hwnd, SB_VERT, &scinfo, TRUE);

        scinfo.nMin = wStringToNumber(hmin);
        scinfo.nMax = wStringToNumber(hmax);
        scinfo.nPos = wStringToNumber(hpos);
        SetScrollInfo(hwnd, SB_HORZ, &scinfo, TRUE);


        // vert
        std::wstring vert = map[L"vert"];
        long vert_long = GetWindowLong(hwnd, GWL_STYLE);
        vert_long &= ~WS_VSCROLL;
        if (vert == L"true")
            vert_long |= WS_VSCROLL;
        SetWindowLong(hwnd, GWL_STYLE, vert_long);

        // horz
        std::wstring horz = map[L"horz"];
        long horz_long = GetWindowLong(hwnd, GWL_STYLE);
        horz_long &= ~WS_HSCROLL;
        if (horz == L"true")
            horz_long |= WS_HSCROLL;
        SetWindowLong(hwnd, GWL_STYLE, horz_long);
    }

    void VmLoop::Attribute_update_listbox(AttributeMap map, AttributeMap old, HWND hwnd)
    {
        // visible
        std::wstring visible = map[L"visible"];
        ShowWindow(hwnd, SW_HIDE);
        if (visible == L"true")
            ShowWindow(hwnd, SW_SHOW);

        // x y w h 
        std::wstring x = map[L"x"];
        std::wstring y = map[L"y"];
        std::wstring w = map[L"w"];
        std::wstring h = map[L"h"];
        MoveWindow(hwnd, wStringToNumber(x), wStringToNumber(y), wStringToNumber(w), wStringToNumber(h), TRUE);

        // style
        std::wstring style = map[L"style"];
        SetWindowLong(hwnd, GWL_STYLE, wStringToNumber(style));

        // border
        std::wstring border = map[L"border"];
        long border_long = GetWindowLong(hwnd, GWL_STYLE);
        border_long &= ~WS_BORDER;
        if (border == L"true")
            border_long |= WS_BORDER;
        SetWindowLong(hwnd, GWL_STYLE, border_long);

        // list
        std::wstring list = map[L"list"];
        // list
        auto lists = splitwString(list, L'\n');
        // 清空字符串
        SendMessage(hwnd, LB_RESETCONTENT, NULL, NULL);
        for (auto& i : lists) {
            if (!i.empty())
                SendMessage(hwnd, LB_ADDSTRING, NULL, (LPARAM)i.c_str());
        }

        // enable
        std::wstring enable = map[L"enable"];
        bool enable_bool = (enable == L"true");
        EnableWindow(hwnd, enable_bool);

        // current
        std::wstring current = map[L"current"];
        // 取消所有选择
        SendMessage(hwnd, LB_SETSEL, FALSE, 0);
        lists = splitwString(current, L'\n');
        for (auto& i : lists)
        {
            if (!i.empty())
            {
                long find_index = SendMessage(hwnd, LB_FINDSTRINGEXACT, -1, (LPARAM)i.c_str());
                if (find_index != LB_ERR)
                {
                    DWORD select = 0;
                    select |= find_index;
                    select <<= 16;
                    select |= find_index;
                    SendMessage(hwnd, LB_SELITEMRANGE, TRUE, select);
                }
            }
        }

        // fontfamily fontsize 
        std::wstring font_family = map[L"fontfamily"];
        std::wstring font_size = map[L"fontsize"];

        LONG font_size_num = wStringToNumber(font_size);
        HFONT hfont;
        LOGFONT logfont;

        GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &logfont);
        logfont.lfHeight = -MulDiv(font_size_num, GetDeviceCaps(GetDC(hwnd), LOGPIXELSY), 72);
        logfont.lfWeight = -MulDiv(font_size_num, GetDeviceCaps(GetDC(hwnd), LOGPIXELSY), 72);
        wsprintf(logfont.lfFaceName, TEXT("%s"), font_family.c_str());
        hfont = CreateFontIndirect(&logfont);
        SendMessage(hwnd, WM_SETFONT, WPARAM(hfont), TRUE);
    }

    void VmLoop::Attribute_update_static(AttributeMap map, AttributeMap old, HWND hwnd)
    {
        // x y w h 
        std::wstring x = map[L"x"];
        std::wstring y = map[L"y"];
        std::wstring w = map[L"w"];
        std::wstring h = map[L"h"];
        MoveWindow(hwnd, wStringToNumber(x), wStringToNumber(y), wStringToNumber(w), wStringToNumber(h), TRUE);

        // style
        std::wstring style = map[L"style"];
        SetWindowLong(hwnd, GWL_STYLE, wStringToNumber(style));

        // enable
        std::wstring enable = map[L"enable"];
        bool enable_bool = (enable == L"true");
        Static_Enable(hwnd, enable_bool);


        // fontfamily fontsize 
        std::wstring font_family = map[L"fontfamily"];
        std::wstring font_size = map[L"fontsize"];

        LONG font_size_num = wStringToNumber(font_size);
        HFONT hfont;
        LOGFONT logfont;

        GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &logfont);
        logfont.lfHeight = -MulDiv(font_size_num, GetDeviceCaps(GetDC(hwnd), LOGPIXELSY), 72);
        logfont.lfWeight = -MulDiv(font_size_num, GetDeviceCaps(GetDC(hwnd), LOGPIXELSY), 72);
        wsprintf(logfont.lfFaceName, TEXT("%s"), font_family.c_str());
        hfont = CreateFontIndirect(&logfont);
        SendMessage(hwnd, WM_SETFONT, WPARAM(hfont), TRUE);


        // -- bgcolor
        std::wstring bgcolor = map[L"bgcolor"];
        m_manager->FontMap[hwnd].bgcolor = bgcolor;

        
        // -- fontcolor
        std::wstring fontcolor = map[L"fontcolor"];
        m_manager->FontMap[hwnd].fontcolor = fontcolor;

        // -- bgvisible
        std::wstring bgvisible = map[L"bgvisible"];
        m_manager->FontMap[hwnd].bgvisible = bgvisible;

        // -- text
        std::wstring text = map[L"text"];
        Static_SetText(hwnd, text.c_str());

        SendMessage(hwnd, WM_PAINT, NULL, NULL);

        // visible
        std::wstring visible = map[L"visible"];
        ShowWindow(hwnd, SW_HIDE);
        if (visible == L"true")
            ShowWindow(hwnd, SW_SHOW);
    }

    void VmLoop::Attribute_update_image(AttributeMap map, AttributeMap old, HWND hwnd)
    {
        // x y w h 
        std::wstring x = map[L"x"];
        std::wstring y = map[L"y"];
        std::wstring w = map[L"w"];
        std::wstring h = map[L"h"];
        MoveWindow(hwnd, wStringToNumber(x), wStringToNumber(y), wStringToNumber(w), wStringToNumber(h), TRUE);

        // style
        std::wstring style = map[L"style"];
        SetWindowLong(hwnd, GWL_STYLE, wStringToNumber(style));

        // enable
        std::wstring enable = map[L"enable"];
        bool enable_bool = (enable == L"true");
        Static_Enable(hwnd, enable_bool);

        // -- path
        std::wstring path = map[L"path"];
        SendMessage(hwnd, 114514, (WPARAM)path.c_str(), NULL);
        SendMessage(hwnd, WM_PAINT, NULL, NULL);

        // visible
        std::wstring visible = map[L"visible"];
        ShowWindow(hwnd, SW_HIDE);
        if (visible == L"true")
            ShowWindow(hwnd, SW_SHOW);
    }

    void VmLoop::loopUpdateAttribute(WindowNode& node)
    {
        wchar_t className[4096];
        GetClassName(node.self, className, 4096);
        std::wstring name = className;


        if (name == L"Edit") { // edit 属性动态更新
            Edit_loop_attribute(node);
        }
        else if (name == L"Button") {   // button 属性动态更新
            Botton_loop_attribute(node);
        }
        else if (name == L"ComboBox"){ // ComboBox 属性动态更新
            Combobox_loop_attribute(node);
        }
        else if (name == L"Layer") { // Layer 属性动态更新
            Layer_loop_attribute(node);
        }
        else if (name == L"msctls_progress32") { // progress 属性动态更新
            Progress_loop_attribute(node);
        }
        else if (name == L"scroll_s") { // scroll 属性动态更新
            ScrollBar_loop_attribute(node);
        }
        else if (name == WC_LISTBOX) {  // listbox 属性动态更新
            ListBox_loop_attribute(node);
        }
        else if (name == WC_STATIC) {    // static 属性动态更新
            Static_loop_attribute(node);
        }
        else if (name == L"Image_c") {  // image 属性动态更新
            Image_loop_attribute(node);
        }
        else {  // window 属性动态更新
            Window_loop_attribute(node);
        }
    }

    void VmLoop::Window_loop_attribute(WindowNode& node)
    {
        // visible 
        node.attribute[L"visible"] = L"false";
        BOOL visible = IsWindowVisible(node.self);
        if (visible)
            node.attribute[L"visible"] = L"true";


        // text
        long lens = GetWindowTextLength(node.self) + 1;
        if (lens > m_memery.getMemLens())
            m_memery.Realloc(lens);
        GetWindowText(node.self, m_memery.getMemery(), lens);
        node.attribute[L"text"] = m_memery.getMemery();


        // xywh
        RECT rect;
        GetWindowRect(node.self, &rect);
        node.attribute[L"x"] = NumberTowString(rect.left);
        node.attribute[L"y"] = NumberTowString(rect.top);
        node.attribute[L"w"] = NumberTowString(rect.right - rect.left);
        node.attribute[L"h"] = NumberTowString(rect.bottom - rect.top);


        // style
        LONG style = GetWindowLong(node.self, GWL_STYLE);
        node.attribute[L"style"] = NumberTowString(style);


        // screenw screenh
        int cxClient = GetSystemMetrics(SM_CXSCREEN);
        int cyClient = GetSystemMetrics(SM_CYSCREEN);
        node.attribute[L"screenx"] = NumberTowString(cxClient);
        node.attribute[L"screeny"] = NumberTowString(cyClient);

        // fullscreen
        node.attribute[L"fullscreen"] = L"false";
        if (IsZoomed(node.self))
            node.attribute[L"fullscreen"] = L"true";

        // enable
        // none

        // resize
        // none

        // maxbox
        // none

        // minxbox
        // none

        // enable
        // none

        // menu
        // none
    }

    void VmLoop::Edit_loop_attribute(WindowNode& node)
    {
        // visible 
        node.attribute[L"visible"] = L"false";
        BOOL visible = IsWindowVisible(node.self);
        if (visible)
            node.attribute[L"visible"] = L"true";


        // text
        long lens = GetWindowTextLength(node.self) + 1;
        if (lens > m_memery.getMemLens())
            m_memery.Realloc(lens);
        GetWindowText(node.self, m_memery.getMemery(), lens);
        node.attribute[L"text"] = m_memery.getMemery();


        // xywh
        RECT rect;
        GetClientRect(node.self, &rect);
        MapWindowPoints(node.self, GetParent(node.self), (LPPOINT)&rect, 2);

        node.attribute[L"x"] = NumberTowString(rect.left);
        node.attribute[L"y"] = NumberTowString(rect.top);
        node.attribute[L"w"] = NumberTowString(rect.right);
        node.attribute[L"h"] = NumberTowString(rect.bottom);


        // style
        LONG style = GetWindowLong(node.self, GWL_STYLE);
        node.attribute[L"style"] = NumberTowString(style);


        // banner
        // none

        // fontfamily fontsize
        // none

        // enable
        // none

        // limit
        // none

        // readyonly
        // none

        // multiline
        // none

        // password
        // none

        // number
        // none

        // vscroll
        // none

        // hscroll
        // none
    }

    void VmLoop::Botton_loop_attribute(WindowNode& node)
    {
        // visible 
        node.attribute[L"visible"] = L"false";
        BOOL visible = IsWindowVisible(node.self);
        if (visible)
            node.attribute[L"visible"] = L"true";


        // text
        long lens = GetWindowTextLength(node.self) + 1;
        if (lens > m_memery.getMemLens())
            m_memery.Realloc(lens);
        GetWindowText(node.self, m_memery.getMemery(), lens);
        node.attribute[L"text"] = m_memery.getMemery();


        // xywh
        RECT rect;
        GetClientRect(node.self, &rect);
        MapWindowPoints(node.self, GetParent(node.self), (LPPOINT)&rect, 2);

        node.attribute[L"x"] = NumberTowString(rect.left);
        node.attribute[L"y"] = NumberTowString(rect.top);
        node.attribute[L"w"] = NumberTowString(rect.right);
        node.attribute[L"h"] = NumberTowString(rect.bottom);

        // onclick
        // none


        // style
        LONG style = GetWindowLong(node.self, GWL_STYLE);
        node.attribute[L"style"] = NumberTowString(style);


        // btntype
        // none

        // fontfamilysize
        // none

        // enable
        // none

        // isclick
        bool isclick = Button_GetState(node.self);
        if (!isclick)
            node.attribute[L"isclick"] = L"true";
        else
            node.attribute[L"isclick"] = L"false";

        // align
        // none

        // vcenter
        // none
    }
    void VmLoop::Combobox_loop_attribute(WindowNode& node)
    {
        // visible 
        node.attribute[L"visible"] = L"false";
        BOOL visible = IsWindowVisible(node.self);
        if (visible)
            node.attribute[L"visible"] = L"true";


        // xywh
        RECT rect;
        GetClientRect(node.self, &rect);
        MapWindowPoints(node.self, GetParent(node.self), (LPPOINT)&rect, 2);

        node.attribute[L"x"] = NumberTowString(rect.left);
        node.attribute[L"y"] = NumberTowString(rect.top);
        node.attribute[L"w"] = NumberTowString(rect.right);
        node.attribute[L"h"] = NumberTowString(rect.bottom);


        // style
        LONG style = GetWindowLong(node.self, GWL_STYLE);
        node.attribute[L"style"] = NumberTowString(style);


        // list
        // none

        // current
        long lens = ComboBox_GetTextLength(node.self) + 1;
        if (lens > m_memery.getMemLens())
            m_memery.Realloc(lens);
        ComboBox_GetText(node.self, m_memery.getMemery(), lens);
        node.attribute[L"current"] = m_memery.getMemery();

        // hide
        // none

        // banner
        // none
        
        // fontfamily
        // none
        
        // fontsize
        // none

    }
    void VmLoop::Layer_loop_attribute(WindowNode& node)
    {
        // visible 
        node.attribute[L"visible"] = L"false";
        BOOL visible = IsWindowVisible(node.self);
        if (visible)
            node.attribute[L"visible"] = L"true";


        // xywh
        RECT rect;
        GetClientRect(node.self, &rect);
        MapWindowPoints(node.self, GetParent(node.self), (LPPOINT)&rect, 2);

        node.attribute[L"x"] = NumberTowString(rect.left);
        node.attribute[L"y"] = NumberTowString(rect.top);
        node.attribute[L"w"] = NumberTowString(rect.right - rect.left);
        node.attribute[L"h"] = NumberTowString(rect.bottom - rect.top);


        // style
        LONG style = GetWindowLong(node.self, GWL_STYLE);
        node.attribute[L"style"] = NumberTowString(style);
            

        // enable
        // none

        // resize
        // none

        // border
        // none

        // frame
        // none

        // acceptfile
        // none

        // color
        // none
    }
    void VmLoop::Progress_loop_attribute(WindowNode& node)
    {
        // visible 
        node.attribute[L"visible"] = L"false";
        BOOL visible = IsWindowVisible(node.self);
        if (visible)
            node.attribute[L"visible"] = L"true";


        // xywh
        RECT rect;
        GetClientRect(node.self, &rect);
        MapWindowPoints(node.self, GetParent(node.self), (LPPOINT)&rect, 2);
        node.attribute[L"x"] = NumberTowString(rect.left);
        node.attribute[L"y"] = NumberTowString(rect.top);
        node.attribute[L"w"] = NumberTowString(rect.right - rect.left);
        node.attribute[L"h"] = NumberTowString(rect.bottom - rect.top);


        // style
        LONG style = GetWindowLong(node.self, GWL_STYLE);
        node.attribute[L"style"] = NumberTowString(style);

        // max min
        PBRANGE range;
        SendMessage(node.self, PBM_GETRANGE, NULL, (LPARAM)&range);
        node.attribute[L"max"] = NumberTowString(range.iHigh);
        node.attribute[L"min"] = NumberTowString(range.iLow);

        // pos
        UINT pos;
        pos = SendMessage(node.self, PBM_GETPOS, NULL, NULL);
        node.attribute[L"pos"] = NumberTowString(pos);

        // step PBM_GETSTEP
        UINT step;
        step = SendMessage(node.self, PBM_GETSTEP, NULL, NULL);
        node.attribute[L"step"] = NumberTowString(step);


        // frame
        // none

        // resize
        // none

        // update 
        node.attribute[L"update"] = L"false";
    }
    void VmLoop::ScrollBar_loop_attribute(WindowNode& node)
    {
        // visible 
        node.attribute[L"visible"] = L"false";
        BOOL visible = IsWindowVisible(node.self);
        if (visible)
            node.attribute[L"visible"] = L"true";


        // xywh
        RECT rect;
        GetClientRect(node.self, &rect);
        MapWindowPoints(node.self, GetParent(node.self), (LPPOINT)&rect, 2);
        node.attribute[L"x"] = NumberTowString(rect.left);
        node.attribute[L"y"] = NumberTowString(rect.top);
        node.attribute[L"w"] = NumberTowString(rect.right - rect.left);
        node.attribute[L"h"] = NumberTowString(rect.bottom - rect.top);


        // style
        LONG style = GetWindowLong(node.self, GWL_STYLE);
        node.attribute[L"style"] = NumberTowString(style);

        SCROLLINFO si;
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_ALL;
        GetScrollInfo(node.self, SB_HORZ, &si);
        node.attribute[L"hpos"] = NumberTowString(si.nPos);

        GetScrollInfo(node.self, SB_VERT, &si);
        node.attribute[L"vpos"] = NumberTowString(si.nPos);

    }

    void VmLoop::ListBox_loop_attribute(WindowNode& node)
    {
        // visible 
        node.attribute[L"visible"] = L"false";
        BOOL visible = IsWindowVisible(node.self);
        if (visible)
            node.attribute[L"visible"] = L"true";


        // xywh
        RECT rect;
        GetClientRect(node.self, &rect);
        MapWindowPoints(node.self, GetParent(node.self), (LPPOINT)&rect, 2);

        node.attribute[L"x"] = NumberTowString(rect.left);
        node.attribute[L"y"] = NumberTowString(rect.top);
        node.attribute[L"w"] = NumberTowString(rect.right);
        node.attribute[L"h"] = NumberTowString(rect.bottom);


        // style
        LONG style = GetWindowLong(node.self, GWL_STYLE);
        node.attribute[L"style"] = NumberTowString(style);

        // current
        std::list<std::wstring> list_wstring;
        long list_count = SendMessage(node.self, LB_GETCOUNT, NULL ,NULL);  // 获取项目最大索引
        for (long i = 0; i < list_count; i++)
        {
            long select = SendMessage(node.self, LB_GETSEL, i, NULL);       // 获取当前索引的选择状态
            if (select != LB_ERR && select == 1)        // 选择状态未出错 且 选择状态不等于未选中(0) 1=选中
            {
                long meme_lens = SendMessage(node.self, LB_GETTEXTLEN, i, NULL);    // 获得被选择字符串长度
                if (meme_lens > m_memery.getMemLens())
                    m_memery.Realloc(meme_lens);
                SendMessage(node.self, LB_GETTEXT, i, (LPARAM)m_memery.getMemery());    // 获取当前索引字符串
                list_wstring.push_back(m_memery.getMemery());
            }
        }
        auto mem_lens = 0;
        std::wstring mem_wstr;
        for (auto& i : list_wstring)
        {
            mem_lens += i.size();
            mem_wstr += i + L"\n";
        }
        node.attribute[L"current"] = mem_wstr;


        // fontfamily
        // none

        // fontsize
        // none
    }

    void VmLoop::Static_loop_attribute(WindowNode& node)
    {
        // visible 
        node.attribute[L"visible"] = L"false";
        BOOL visible = IsWindowVisible(node.self);
        if (visible)
            node.attribute[L"visible"] = L"true";


        // xywh
        RECT rect;
        GetClientRect(node.self, &rect);
        MapWindowPoints(node.self, GetParent(node.self), (LPPOINT)&rect, 2);

        node.attribute[L"x"] = NumberTowString(rect.left);
        node.attribute[L"y"] = NumberTowString(rect.top);
        node.attribute[L"w"] = NumberTowString(rect.right);
        node.attribute[L"h"] = NumberTowString(rect.bottom);


        // style
        LONG style = GetWindowLong(node.self, GWL_STYLE);
        node.attribute[L"style"] = NumberTowString(style);

        // text
        long lens = GetWindowTextLength(node.self) + 1;
        if (lens > m_memery.getMemLens())
            m_memery.Realloc(lens);
        GetWindowText(node.self, m_memery.getMemery(), lens);
        node.attribute[L"text"] = m_memery.getMemery();

        // fontcolor 
        // none

         
        // bgcolor
        // none

        // bgvisible
        // none
    }
    void VmLoop::Image_loop_attribute(WindowNode& node)
    {
        // visible 
        node.attribute[L"visible"] = L"false";
        BOOL visible = IsWindowVisible(node.self);
        if (visible)
            node.attribute[L"visible"] = L"true";


        // xywh
        RECT rect;
        GetClientRect(node.self, &rect);
        MapWindowPoints(node.self, GetParent(node.self), (LPPOINT)&rect, 2);

        node.attribute[L"x"] = NumberTowString(rect.left);
        node.attribute[L"y"] = NumberTowString(rect.top);
        node.attribute[L"w"] = NumberTowString(rect.right);
        node.attribute[L"h"] = NumberTowString(rect.bottom);


        // style
        LONG style = GetWindowLong(node.self, GWL_STYLE);
        node.attribute[L"style"] = NumberTowString(style);

        // enable
        // none

        // path 
        // none
    }
}