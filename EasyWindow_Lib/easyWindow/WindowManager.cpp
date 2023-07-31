#include "WindowManager.h"
#include "Menu.h"
#include <functional>
#include <sstream>

std::map<HWND, easyWindow::WindowManager*> easyWindow::WindowManager::HwndMap;
std::map<HWND, struct easyWindow::WindowManager::fontColor> easyWindow::WindowManager::FontMap;
std::map<HWND, struct easyWindow::WindowManager::ImageInfo> easyWindow::WindowManager::ImageMap;

HWND easyWindow::WindowManager::Window(AttributeMap attribute)
{
    auto hInstance = GetModuleHandle(0);
    static TCHAR szAppName[] = TEXT("EasyWindow");
    HWND         hwnd;
    MSG          msg;
    WNDCLASS     wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = Window_WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));;
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;
  
    
    if (!RegisterClass(&wndclass))
    {
        DWORD error_code = GetLastError();
        if(error_code != 1410)
            return NULL;
    }

    std::wstring title = attribute[L"text"];
    std::wstring x = attribute[L"x"];
    std::wstring y = attribute[L"y"];
    std::wstring w = attribute[L"w"];
    std::wstring h = attribute[L"h"];
    std::wstring style = attribute[L"style"];
    std::wstring fullscreen = attribute[L"fullscreen"];
    std::wstring resize = attribute[L"resize"];
    std::wstring enable = attribute[L"enable"];
    std::wstring maxbox = attribute[L"maxbox"];
    std::wstring minbox = attribute[L"minbox"];
    std::wstring menu = attribute[L"menu"];

    // fullscreen
    LONG fullscreen_long = NULL;
    if (fullscreen == L"true")
        fullscreen_long = WS_MAXIMIZE;

    // resize
    LONG resize_long = NULL;
    if (resize == L"true")
        resize_long = WS_THICKFRAME;


    // maxbox
    LONG maxbox_long = NULL;
    if (maxbox == L"true")
        maxbox_long = WS_MAXIMIZEBOX;

    // minbox
    LONG minbox_long = NULL;
    if (minbox == L"true")
        minbox_long = WS_MINIMIZEBOX;

    hwnd = CreateWindow(szAppName,                  
        title.c_str(),              
        wstringTonumber(style)| WS_BORDER | fullscreen_long | resize_long | maxbox_long | minbox_long,
        wstringTonumber(x),         
        wstringTonumber(y),         
        wstringTonumber(w),        
        wstringTonumber(h),         
        NULL,                       
        NULL,                      
        hInstance,                  
        NULL);                  

    // enable
    EnableWindow(hwnd, (enable == L"true"));

    UnregisterClassW(szAppName, hInstance);

    if (!menu.empty() && hwnd)
    {
        HMENU menu_ptr = menuGet(menu);
        if (menu_ptr != nullptr)
        {
            SetMenu(hwnd, menu_ptr);
        }
    }


    if(hwnd)
        HwndMap.insert({ hwnd,this });

    return hwnd;
}

HWND easyWindow::WindowManager::Button(AttributeMap attribute, HWND parent)
{
    auto hInstance = GetModuleHandle(0);

    std::wstring text = attribute[L"text"];
    std::wstring x = attribute[L"x"];
    std::wstring y = attribute[L"y"];
    std::wstring w = attribute[L"w"];
    std::wstring h = attribute[L"h"];
    std::wstring onclick = attribute[L"onclick"];
    std::wstring style = attribute[L"style"];
    std::wstring type = attribute[L"btntype"];
    std::wstring font_family = attribute[L"fontfamily"];
    std::wstring font_size = attribute[L"fontsize"];
    std::wstring enable = attribute[L"enable"];
    std::wstring isclick = attribute[L"isclick"];
    std::wstring align = attribute[L"align"];
    std::wstring valign = attribute[L"valign"];
    
    LONG btnStyle = NULL;
    if (type == L"1")
        btnStyle = BS_AUTOCHECKBOX;
    if (type == L"2")
        btnStyle = BS_AUTO3STATE;
    if (type == L"3")
        btnStyle = BS_GROUPBOX;
    if (type == L"4")
        btnStyle = BS_PUSHBUTTON;
    if (type == L"5")
        btnStyle = BS_DEFPUSHBUTTON;
    if (type == L"6")
        btnStyle = BS_AUTORADIOBUTTON;

    // align
    LONG align_long = NULL;
    if (align == L"left")
        align_long = BS_LEFT;
    if (align == L"center")
        align_long = BS_CENTER;
    if (align == L"right")
        align_long = BS_RIGHT;

    // valign
    LONG valign_long = NULL;
    if (valign == L"top")
        valign_long = BS_TOP;
    if (valign == L"center")
        valign_long = BS_VCENTER;
    if (valign == L"bottom")
        valign_long = BS_BOTTOM;

    HWND hBtn = CreateWindow(   //按钮创建  
        L"button",
        text.c_str(),
        wstringTonumber(style) | WS_CHILD | align_long| valign_long,
        wstringTonumber(x),
        wstringTonumber(y),
        wstringTonumber(w), 
        wstringTonumber(h),
        parent,
        (HMENU)1,
        hInstance,
        0);

    RECT rect;
    GetWindowRect(hBtn, &rect);

    // -- font

    LONG font_size_num = wstringTonumber(font_size);
    HFONT hfont;
    LOGFONT logfont;
    GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &logfont);
    logfont.lfHeight = -MulDiv(font_size_num, GetDeviceCaps(GetDC(hBtn), LOGPIXELSY), 72);//字体高度为16PX  
    logfont.lfWidth = -MulDiv(font_size_num, GetDeviceCaps(GetDC(hBtn), LOGPIXELSY), 72);//字体宽度为20PX  
    wsprintf(logfont.lfFaceName, TEXT("%s"), font_family.c_str());
    hfont = CreateFontIndirect(&logfont);//字体为宋体  
    SendMessage(hBtn, WM_SETFONT, WPARAM(hfont), TRUE); // Send this to each control

    // -- enable
    bool enable_bool = (enable == L"true");
    Button_Enable(hBtn, enable_bool);

    // isclick 
    bool isclick_bool = (isclick != L"true");
    Button_SetState(hBtn, !isclick_bool);

    if(hBtn)
        HwndMap.insert({ hBtn,this });

    return hBtn;
}

HWND easyWindow::WindowManager::Edit(AttributeMap attribute, HWND parent)
{
    auto hInstance = GetModuleHandle(0);

    std::wstring text = attribute[L"text"];
    std::wstring x = attribute[L"x"];
    std::wstring y = attribute[L"y"];
    std::wstring w = attribute[L"w"];
    std::wstring h = attribute[L"h"];
    std::wstring onclick = attribute[L"onclick"];
    std::wstring style = attribute[L"style"];
    std::wstring font_family = attribute[L"fontfamily"];
    std::wstring font_size = attribute[L"fontsize"];
    std::wstring enable = attribute[L"enable"];
    std::wstring fmtlines = attribute[L"fmtlines"];
    std::wstring banner = attribute[L"banner"];
    std::wstring limit = attribute[L"limit"];
    std::wstring readyonly = attribute[L"readyonly"];

    std::wstring align = attribute[L"align"];
    std::wstring multiline = attribute[L"multiline"];
    std::wstring password = attribute[L"password"];
    std::wstring number = attribute[L"number"];
    std::wstring vscroll = attribute[L"vscroll"];
    std::wstring hscroll = attribute[L"hscroll"];
    std::wstring border = attribute[L"border"];

    // align
    LONG align_long = NULL;
    if (align == L"left")
        align_long = ES_LEFT;
    if (align == L"center")
        align_long = ES_CENTER;
    if (align == L"right")
        align_long = ES_RIGHT;

    // mutiline 
    LONG mutiline_long = NULL;
    if (multiline == L"true")
        mutiline_long = ES_MULTILINE | ES_WANTRETURN;

    LONG password_long = NULL;
    if (password == L"true")
        password_long = ES_PASSWORD;

    LONG number_long = NULL;
    if (number == L"true")
        number_long = ES_NUMBER;

    LONG vscroll_long = NULL;
    if (vscroll == L"true")
        vscroll_long = WS_VSCROLL | ES_AUTOVSCROLL;

    LONG hscroll_long = NULL;
    if (hscroll == L"true")
        hscroll_long = WS_HSCROLL | ES_AUTOHSCROLL;

    // border
    LONG border_long = NULL;
    if (border == L"true")
        border_long = WS_BORDER;

    
    HWND hBtn = CreateWindow(   //按钮edit 
        L"edit",
        text.c_str(),
        wstringTonumber(style) | WS_CHILD | border_long| align_long | mutiline_long | password_long | number_long | vscroll_long | hscroll_long,
        wstringTonumber(x),
        wstringTonumber(y),
        wstringTonumber(w),
        wstringTonumber(h),
        parent,
        (HMENU)wstringTonumber(onclick),
        hInstance,
        0);

    // fontfamily fontsize 
    LONG font_size_num = wstringTonumber(font_size);
    HFONT hfont;
    LOGFONT logfont;
    
    GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &logfont);
    logfont.lfHeight = -MulDiv(font_size_num, GetDeviceCaps(GetDC(hBtn), LOGPIXELSY), 72);
    logfont.lfWeight = -MulDiv(font_size_num, GetDeviceCaps(GetDC(hBtn), LOGPIXELSY), 72);
    wsprintf(logfont.lfFaceName, TEXT("%s"), font_family.c_str());
    hfont = CreateFontIndirect(&logfont);
    SendMessage(hBtn, WM_SETFONT, WPARAM(hfont), TRUE);


    // -- enable
    bool enable_bool = (enable == L"true");
    Edit_Enable(hBtn, enable_bool);

    // fmtlines
    bool fmtlines_bool = (fmtlines == L"true");
    Edit_FmtLines(hBtn, fmtlines_bool);

    // banner
    Edit_SetCueBannerText(hBtn, banner.c_str());

    // limit
    if (!limit.empty())
    {
        long limit_long = wstringTonumber(limit);
        Edit_LimitText(hBtn, limit_long);
    }

    // readyonly
    bool readyonly_bool = (readyonly == L"true");
    Edit_SetReadOnly(hBtn, readyonly_bool);

    if (hBtn)
        HwndMap.insert({ hBtn,this });

    return hBtn;
}

HWND easyWindow::WindowManager::ComboBox(AttributeMap attribute, HWND parent)
{
    auto hInstance = GetModuleHandle(0);

    std::wstring x = attribute[L"x"];
    std::wstring y = attribute[L"y"];
    std::wstring w = attribute[L"w"];
    std::wstring h = attribute[L"h"];
    std::wstring style = attribute[L"style"];

    std::wstring enable = attribute[L"enable"];
    std::wstring list = attribute[L"list"];
    std::wstring current = attribute[L"current"]; 
    std::wstring banner = attribute[L"banner"];
    std::wstring font_family = attribute[L"fontfamily"];
    std::wstring font_size = attribute[L"fontsize"];


    HWND hBtn = CreateWindow(   //按钮edit 
        WC_COMBOBOX,
        TEXT(""),
        wstringTonumber(style) | WS_CHILD | CBS_HASSTRINGS | CBS_DROPDOWN | WS_VSCROLL | CBS_NOINTEGRALHEIGHT,
        wstringTonumber(x),
        wstringTonumber(y),
        wstringTonumber(w),
        wstringTonumber(h),
        parent,
        NULL,
        hInstance,
        0);


    // fontfamily fontsize 
    LONG font_size_num = wstringTonumber(font_size);
    HFONT hfont;
    LOGFONT logfont;

    GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &logfont);
    logfont.lfHeight = -MulDiv(font_size_num, GetDeviceCaps(GetDC(hBtn), LOGPIXELSY), 72);
    logfont.lfWeight = -MulDiv(font_size_num, GetDeviceCaps(GetDC(hBtn), LOGPIXELSY), 72);
    wsprintf(logfont.lfFaceName, TEXT("%s"), font_family.c_str());
    hfont = CreateFontIndirect(&logfont);
    SendMessage(hBtn, WM_SETFONT, WPARAM(hfont), TRUE);
    
    // list
    ComboBox_ResetContent(hBtn);
    auto lists = splitwString(list, L'\n');
    for (auto& i : lists) {
        if(!i.empty())
            ComboBox_AddString(hBtn, i.c_str());
    }

    // enable
    bool enable_bool = (enable == L"true");
    ComboBox_Enable(hBtn, enable_bool);

    // current
    if (!current.empty())
        ComboBox_SelectString(hBtn, -1, current.c_str());
    
    // banner
    ComboBox_SetCueBannerText(hBtn, banner.c_str());

    if (hBtn)
        HwndMap.insert({ hBtn,this });

    return hBtn;
}

HWND easyWindow::WindowManager::Layer(AttributeMap attribute, HWND parent)
{
    auto hInstance = GetModuleHandle(0);
    static TCHAR szAppName[] = TEXT("Layer");
    HWND         hwnd;
    MSG          msg;
    WNDCLASS     wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = Layer_WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));;
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;


    if (!RegisterClass(&wndclass))
    {
        DWORD error_code = GetLastError();
        if (error_code != 1410)
            return NULL;
    }

    std::wstring x = attribute[L"x"];
    std::wstring y = attribute[L"y"];
    std::wstring w = attribute[L"w"];
    std::wstring h = attribute[L"h"];
    std::wstring style = attribute[L"style"];

    std::wstring frame = attribute[L"frame"];
    std::wstring resize = attribute[L"resize"];
    std::wstring enable = attribute[L"enable"];
    std::wstring border = attribute[L"border"];
    std::wstring acceptfile = attribute[L"acceptfile"];
    std::wstring color = attribute[L"color"];



    // border
    LONG border_long = NULL;
    if (border == L"true")
        border_long = WS_BORDER;


    // resize
    LONG resize_long = NULL;
    if (resize == L"true")
        resize_long = WS_THICKFRAME;


    // frame
    LONG frame_long = NULL;
    if (frame == L"true")
        frame_long = WS_DLGFRAME;


    // acceptfile
    LONG acceptfile_long = NULL;
    if (acceptfile == L"true")
        acceptfile_long = WS_EX_ACCEPTFILES;

    // color 占用menu
    DWORD color_int = 0;
    if (color.size() == 6)
    {
        std::wstringstream ss;
        ss << std::hex << color;
        ss >> color_int;
    }


    hwnd = CreateWindowEx(acceptfile_long,
        szAppName,
        L"",
        wstringTonumber(style) | WS_CHILD | resize_long | border_long | frame_long,
        wstringTonumber(x),
        wstringTonumber(y),
        wstringTonumber(w),
        wstringTonumber(h),
        parent,
        (HMENU)color_int,
        hInstance,
        NULL);

    UnregisterClassW(szAppName, hInstance);


    // enable
    EnableWindow(hwnd, (enable == L"true"));


    if (hwnd)
        HwndMap.insert({ hwnd,this });

    return hwnd;
}

HWND easyWindow::WindowManager::Progress(AttributeMap attribute, HWND parent)
{
    auto hInstance = GetModuleHandle(0);

    std::wstring x = attribute[L"x"];
    std::wstring y = attribute[L"y"];
    std::wstring w = attribute[L"w"];
    std::wstring h = attribute[L"h"];
    std::wstring style = attribute[L"style"];

    std::wstring frame = attribute[L"frame"];
    std::wstring resize = attribute[L"resize"];
    std::wstring state = attribute[L"state"];

    std::wstring step = attribute[L"step"];
    std::wstring max = attribute[L"max"];
    std::wstring min = attribute[L"min"];
    std::wstring pos = attribute[L"pos"];

    // resize
    LONG resize_long = NULL;
    if (resize == L"true")
        resize_long = WS_THICKFRAME;


    // frame
    LONG frame_long = NULL;
    if (frame == L"true")
        frame_long = WS_DLGFRAME;

    HWND hBtn = CreateWindow(
        PROGRESS_CLASS,
        L"",
        wstringTonumber(style) | WS_CHILD | resize_long | frame_long,
        wstringTonumber(x),
        wstringTonumber(y),
        wstringTonumber(w),
        wstringTonumber(h),
        parent,
        NULL,
        hInstance,
        NULL);

    // max min
    DWORD  max_long = wstringTonumber(max);
    DWORD  min_long = wstringTonumber(min);

    SendMessage(hBtn, PBM_SETRANGE, 0, MAKELPARAM(min_long, max_long));


    // pos
    long pos_long = wstringTonumber(pos);
    pos_long = SendMessage(hBtn, PBM_SETPOS, pos_long, NULL);


    // step
    long step_long = wstringTonumber(step);
    SendMessageW(hBtn, PBM_SETSTEP, step_long, NULL);


    SendMessageW(hBtn, PBM_STEPIT, 0, 0);


    if (hBtn)
        HwndMap.insert({ hBtn,this });

    return hBtn;
}

HWND easyWindow::WindowManager::Scroll(AttributeMap attribute, HWND parent)
{
    auto hInstance = GetModuleHandle(0);
    static TCHAR szAppName[] = TEXT("scroll_s");
    HWND         hwnd;
    MSG          msg;
    WNDCLASS     wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = Scroll_WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));;
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;


    if (!RegisterClass(&wndclass))
    {
        DWORD error_code = GetLastError();
        if (error_code != 1410)
            return NULL;
    }

    std::wstring x = attribute[L"x"];
    std::wstring y = attribute[L"y"];
    std::wstring w = attribute[L"w"];
    std::wstring h = attribute[L"h"];
    std::wstring style = attribute[L"style"];

    std::wstring vmin = attribute[L"vmin"];
    std::wstring vmax = attribute[L"vmax"];
    std::wstring vpos = attribute[L"vpos"];
    std::wstring vert = attribute[L"vert"];
    std::wstring hmin = attribute[L"hmin"];
    std::wstring hmax = attribute[L"hmax"];
    std::wstring hpos = attribute[L"hpos"];
    std::wstring horz = attribute[L"horz"];


    // vert
    LONG vert_long = NULL;
    if (vert == L"true")
        vert_long = WS_VSCROLL;

    // horz
    LONG horz_long = NULL;
    if (horz == L"true")
        horz_long = WS_HSCROLL;

    HWND hBtn = CreateWindow(
        L"scroll_s",
        L"",
        wstringTonumber(style) | WS_CHILD| vert_long | horz_long,
        wstringTonumber(x),
        wstringTonumber(y),
        wstringTonumber(w),
        wstringTonumber(h),
        parent,
        NULL,
        hInstance,
        NULL);


    // max min pos
    SCROLLINFO scinfo;

    scinfo.cbSize = sizeof(SCROLLINFO);
    scinfo.fMask = SIF_ALL;
    scinfo.nPage = 0;

    scinfo.nMin = wstringTonumber(vmin);
    scinfo.nMax = wstringTonumber(vmax);
    scinfo.nPos = wstringTonumber(vpos);
    SetScrollInfo(hBtn, SB_VERT, &scinfo,TRUE);

    scinfo.nMin = wstringTonumber(hmin);
    scinfo.nMax = wstringTonumber(hmax);
    scinfo.nPos = wstringTonumber(hpos);
    SetScrollInfo(hBtn, SB_HORZ, &scinfo, TRUE);

    UnregisterClassW(szAppName, hInstance);

    if (hBtn)
        HwndMap.insert({ hBtn,this });

    return hBtn;
}

HWND easyWindow::WindowManager::ListBox(AttributeMap attribute, HWND parent)
{
    auto hInstance = GetModuleHandle(0);

    std::wstring x = attribute[L"x"];
    std::wstring y = attribute[L"y"];
    std::wstring w = attribute[L"w"];
    std::wstring h = attribute[L"h"];
    std::wstring style = attribute[L"style"];

    std::wstring enable = attribute[L"enable"];
    std::wstring list = attribute[L"list"];
    std::wstring current = attribute[L"current"];
    std::wstring font_family = attribute[L"fontfamily"];
    std::wstring font_size = attribute[L"fontsize"];
    std::wstring border = attribute[L"border"];

    
    // border
    LONG border_long = NULL;
    if (border == L"true")
        border_long = WS_BORDER;


    HWND hBtn = CreateWindow(   //按钮listbox 
        WC_LISTBOX,
        NULL,
        wstringTonumber(style) | WS_CHILD | LBS_HASSTRINGS| border_long|WS_VSCROLL|WS_HSCROLL| LBS_MULTIPLESEL,
        wstringTonumber(x),
        wstringTonumber(y),
        wstringTonumber(w),
        wstringTonumber(h),
        parent,
        NULL,
        hInstance,
        0);



    // fontfamily fontsize 
    LONG font_size_num = wstringTonumber(font_size);
    HFONT hfont;
    LOGFONT logfont;

    GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &logfont);
    logfont.lfHeight = -MulDiv(font_size_num, GetDeviceCaps(GetDC(hBtn), LOGPIXELSY), 72);
    logfont.lfWeight = -MulDiv(font_size_num, GetDeviceCaps(GetDC(hBtn), LOGPIXELSY), 72);
    wsprintf(logfont.lfFaceName, TEXT("%s"), font_family.c_str());
    hfont = CreateFontIndirect(&logfont);
    SendMessage(hBtn, WM_SETFONT, WPARAM(hfont), TRUE);

    // list
    auto lists = splitwString(list, L'\n');
    // 清空字符串
    SendMessage(hBtn, LB_RESETCONTENT, NULL, NULL);
    for (auto& i : lists) {
        if (!i.empty())
            SendMessage(hBtn, LB_ADDSTRING, NULL, (LPARAM)i.c_str());
    }

    // enable
    bool enable_bool = (enable == L"true");
    EnableWindow(hBtn, enable_bool);

    // current
    // 取消所有选择
    SendMessage(hBtn, LB_SETSEL, FALSE, 0);
    lists = splitwString(current, L'\n');
    for(auto & i : lists)
    {
        if (!i.empty())
        {
            long find_index = SendMessage(hBtn, LB_FINDSTRINGEXACT, -1, (LPARAM)i.c_str());
            if (find_index != LB_ERR)
            {
                DWORD select = 0;
                select |= find_index;
                select <<= 16;
                select |= find_index;
                SendMessage(hBtn, LB_SELITEMRANGE, TRUE, select);
            }
        }
            
    }



    if (hBtn)
        HwndMap.insert({ hBtn,this });

    return hBtn;
}

HWND easyWindow::WindowManager::Static(AttributeMap attribute, HWND parent)
{
    auto hInstance = GetModuleHandle(0);

    std::wstring text = attribute[L"text"];
    std::wstring x = attribute[L"x"];
    std::wstring y = attribute[L"y"];
    std::wstring w = attribute[L"w"];
    std::wstring h = attribute[L"h"];
    std::wstring style = attribute[L"style"];

    std::wstring enable = attribute[L"enable"];
    std::wstring font_family = attribute[L"fontfamily"];
    std::wstring font_size = attribute[L"fontsize"];

    std::wstring fontcolor = attribute[L"fontcolor"];
    std::wstring bgcolor = attribute[L"bgcolor"];
    std::wstring bgvisible = attribute[L"bgvisible"];


    HWND hBtn = CreateWindow(   //按钮 static
        L"STATIC",
        text.c_str(),
        wstringTonumber(style) | WS_CHILD | SS_LEFT,
        wstringTonumber(x),
        wstringTonumber(y),
        wstringTonumber(w),
        wstringTonumber(h),
        parent,
        NULL,
        hInstance,
        0);

    // fontfamily fontsize 
    LONG font_size_num = wstringTonumber(font_size);
    HFONT hfont;
    LOGFONT logfont;

    GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &logfont);
    logfont.lfHeight = -MulDiv(font_size_num, GetDeviceCaps(GetDC(hBtn), LOGPIXELSY), 72);
    logfont.lfWeight = -MulDiv(font_size_num, GetDeviceCaps(GetDC(hBtn), LOGPIXELSY), 72);
    wsprintf(logfont.lfFaceName, TEXT("%s"), font_family.c_str());
    hfont = CreateFontIndirect(&logfont);
    SendMessage(hBtn, WM_SETFONT, WPARAM(hfont), TRUE);


    // -- enable
    bool enable_bool = (enable == L"true");
    EnableWindow(hBtn, enable_bool);

    if (hBtn)
    {
        // font visible 
        FontMap[hBtn].bgvisible = bgvisible;

        // font color
        FontMap[hBtn].fontcolor = fontcolor;
    
        // bg color
        FontMap[hBtn].bgcolor = bgcolor;
    }

    if (hBtn)
        HwndMap.insert({ hBtn,this });

    return hBtn;
}

HWND easyWindow::WindowManager::Image(AttributeMap attribute, HWND parent)
{
    auto hInstance = GetModuleHandle(0);

    std::wstring path = attribute[L"path"];
    std::wstring x = attribute[L"x"];
    std::wstring y = attribute[L"y"];
    std::wstring w = attribute[L"w"];
    std::wstring h = attribute[L"h"];
    std::wstring style = attribute[L"style"];
    std::wstring enable = attribute[L"enable"];

    static TCHAR szAppName[] = TEXT("Image_c");
    HWND         hwnd;
    MSG          msg;
    WNDCLASS     wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = Image_WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));;
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;


    if (!RegisterClass(&wndclass))
    {
        DWORD error_code = GetLastError();
        if (error_code != 1410)
            return NULL;
    }

    /* The class is registered, let's create the program*/
    HWND hBtn = CreateWindow(
        szAppName,         /* Classname */
        L"",       /* Title Text */
        wstringTonumber(style),
        wstringTonumber(x),
        wstringTonumber(y),
        wstringTonumber(w),
        wstringTonumber(h),
        parent,        /* The window is a child-window to desktop */
        NULL,                /* No menu */
        hInstance,       /* Program Instance handler */
        NULL                 /* No Window Creation data */
    );

    // enable
    EnableWindow(hBtn, (enable == L"true"));
    
    if (hBtn)
    {
        ImageInfo info;
        info.path = path;
        ImageMap.insert({ hBtn,info });
    }

    if (hBtn)
        HwndMap.insert({ hBtn,this });

    return hBtn;
}

void easyWindow::WindowManager::RemoveHwndMap(HWND hwnd)
{
    auto finder = HwndMap.find(hwnd);
    if (finder != HwndMap.end()) {
        DestroyWindow(hwnd);
        HwndMap.erase(finder);
    }
    
    auto fontder = FontMap.find(hwnd);
    if (fontder != FontMap.end())
    {
        FontMap.erase(fontder);
    }

    auto imageder = ImageMap.find(hwnd);
    if (imageder != ImageMap.end())
    {
        ImageMap.erase(imageder);
    }
}

LRESULT easyWindow::WindowManager::Window_WndProc(HWND hwnd, UINT uint, WPARAM wparam, LPARAM lparam)
{
    switch (uint)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
        }
        break;
    case WM_COMMAND:
        {
            // button callback
            if (HIWORD(wparam) == BN_CLICKED)
            {
                int code = LOWORD(wparam);
                HwndMap[hwnd]->Message.push({ (HWND)lparam, numberTowstring(code)});
            }
        }
        break;
    case WM_CTLCOLORSTATIC:
    {
        fontColor colorinfo;
        auto finder = FontMap.find((HWND)lparam);
        if (finder != FontMap.end())
        {
            colorinfo = finder->second;


            // bg color
            std::wstring bgcolor = colorinfo.bgcolor;
            DWORD bgcolor_long = 0;
            if (bgcolor.size() == 6)
            {
                std::wstringstream ss;
                ss << std::hex << bgcolor;
                ss >> bgcolor_long;
            }
            SetBkColor((HDC)wparam, bgcolor_long);


            // -- fontcolor
            std::wstring fontcolor = colorinfo.fontcolor;
            DWORD fontcolor_long = 0;
            if (fontcolor.size() == 6)
            {
                std::wstringstream ss;
                ss << std::hex << fontcolor;
                ss >> fontcolor_long;
            }
            SetTextColor((HDC)wparam, fontcolor_long);

            // bg visible
            if (colorinfo.bgvisible == L"false")
                SetBkMode((HDC)wparam, TRANSPARENT);
            else
                SetBkMode((HDC)wparam, OPAQUE);


            return (INT_PTR)GetStockObject((NULL_BRUSH));
        }
    }
    break;
    default:
        return DefWindowProc(hwnd, uint, wparam, lparam);
    }

    return NULL;
}

LRESULT easyWindow::WindowManager::Layer_WndProc(HWND hwnd, UINT uint, WPARAM wparam, LPARAM lparam)
{
    static std::map<HWND, DWORD> color_list;
    switch (uint)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        {
            DWORD color_int = 0;
            if (color_list.find(hwnd) != color_list.end())
                color_int = color_list[hwnd];
            else
                color_int = (DWORD)GetMenu(hwnd);

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rc;
            GetClientRect(hwnd, &rc);
            HBRUSH hNewBrush = ::CreateSolidBrush(color_int);

            if (hNewBrush)
            {
                ::FillRect(hdc, &rc, hNewBrush);
                ::DeleteObject(hNewBrush);
            }

            EndPaint(hwnd, &ps);
        }
        break;
    case WM_COMMAND:
    {
        // button callback
        if (HIWORD(wparam) == BN_CLICKED)
        {
            int code = LOWORD(wparam);
            HwndMap[hwnd]->Message.push({ (HWND)lparam, numberTowstring(code) });
        }
    }
    break;
    case 101:
        color_list[hwnd] = (DWORD)wparam;
        break;
    case WM_DROPFILES:
        {
            HDROP hDropInfo = (HDROP)wparam;
            UINT  nFileCount = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
            TCHAR szFileName[_MAX_PATH] = TEXT("");
            DWORD dwAttribute;

            // 获取拖拽进来文件和文件夹
            for (UINT i = 0; i < nFileCount; i++)
            {
                ::DragQueryFile(hDropInfo, i, szFileName, _MAX_PATH-1);
                dwAttribute = ::GetFileAttributes(szFileName);
                HwndMap[hwnd]->wStringMessage.push_back(szFileName);
            }
            ::DragFinish(hDropInfo);
        }
        break;
    case WM_CTLCOLORSTATIC:
    {
        fontColor colorinfo;
        auto finder = FontMap.find((HWND)lparam);
        if (finder != FontMap.end())
        {
            colorinfo = finder->second;


            // bg color
            std::wstring bgcolor = colorinfo.bgcolor;
            DWORD bgcolor_long = 0;
            if (bgcolor.size() == 6)
            {
                std::wstringstream ss;
                ss << std::hex << bgcolor;
                ss >> bgcolor_long;
            }
            SetBkColor((HDC)wparam, bgcolor_long);


            // -- fontcolor
            std::wstring fontcolor = colorinfo.fontcolor;
            DWORD fontcolor_long = 0;
            if (fontcolor.size() == 6)
            {
                std::wstringstream ss;
                ss << std::hex << fontcolor;
                ss >> fontcolor_long;
            }
            SetTextColor((HDC)wparam, fontcolor_long);

            // bg visible
            if (colorinfo.bgvisible == L"false")
                SetBkMode((HDC)wparam, TRANSPARENT);
            else
                SetBkMode((HDC)wparam, OPAQUE);


            return (INT_PTR)GetStockObject((NULL_BRUSH));
        }
    }
    break;
    default:
        return DefWindowProc(hwnd, uint, wparam, lparam);
    }

    return NULL;
}

LRESULT easyWindow::WindowManager::Scroll_WndProc(HWND hwnd, UINT uint, WPARAM wparam, LPARAM lparam)
{
    switch (uint)
    {
    case WM_CREATE:
        SetWindowRgn(hwnd, NULL, TRUE);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_VSCROLL:
        {
            switch (LOWORD(wparam))
            {
            case SB_THUMBTRACK:
                long pos = HIWORD(wparam);
                SetScrollPos(hwnd, SB_VERT, pos, TRUE);
            }
        }
        break;
    case WM_HSCROLL:
        {
            switch (LOWORD(wparam))
            {
            case SB_THUMBTRACK:
                long pos = HIWORD(wparam);
                SetScrollPos(hwnd, SB_HORZ, pos, TRUE);
            }
        }
        break;
    default:
        return DefWindowProc(hwnd, uint, wparam, lparam);
    }

    return NULL;
}

LRESULT easyWindow::WindowManager::Image_WndProc(HWND hwnd, UINT uint, WPARAM wparam, LPARAM lparam)
{
    switch (uint)                  /* handle the messages */
    {
    case WM_DESTROY:
        PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
        break;
    case WM_PAINT:
    {
        std::wstring image_path;

        auto finder = ImageMap.find(hwnd);
        if (finder == ImageMap.end())
            return NULL;

        image_path = finder->second.path;

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        Gdiplus::Graphics graphics(hdc);
        Gdiplus::Image image(image_path.c_str());
        RECT rect;
        GetWindowRect(hwnd, &rect);
        graphics.DrawImage(&image, 0, 0, rect.right, rect.bottom);
        EndPaint(hwnd, &ps);
    }
    break;
    case 114514:
    {
        std::wstring path;
        path = (wchar_t*)wparam;
        ImageMap[hwnd].path = path;
    }
    break;
    default:
        return DefWindowProc(hwnd, uint, wparam, lparam);
    }
    return NULL;
}

long easyWindow::WindowManager::wstringTonumber(std::wstring str)
{
    long double ret = 0;
    std::wstringstream ss;
    ss << str;
    ss >> ret;
    return ret;
}

std::wstring easyWindow::WindowManager::numberTowstring(long number)
{
    std::wstring ret;
    std::wstringstream ss;
    ss << number;
    ss >> ret;
    return ret;
}

std::list<std::wstring> easyWindow::WindowManager::splitwString(std::wstring str, wchar_t split)
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

HMENU easyWindow::WindowManager::menuGet(std::wstring menuStr)
{
    
    HMENU ret = nullptr;

    easyWindow::Menu menu(menuStr);
    ret = menu.get();

    return ret;
}
