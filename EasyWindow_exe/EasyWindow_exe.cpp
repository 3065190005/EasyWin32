// EasyWindow_exe.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#pragma comment(lib,"EasyWindow_Lib.lib")


#include "easyWindow/ApiWindow.h"
#include "easyWindow/VmWindow.h"
#include "easyWindow/Menu.h"

#include <fstream>
#include <sstream>
#include <iostream>

int main()
{
    std::wstring buf;
    buf = LR"(
<window>[id="0"]
       <button>[onclick="update",x="300"]
       </button>
       <layer>[id="1",color="000000"]
            <edit>[w="200",h="100",id="1",border="true"]
            </edit>
        </layer>
</window>
    )";

    easyWindow::ApiWindow api_window(buf);
    easyWindow::VmWindow vm_window(api_window.getAst());
    vm_window.init();

    while (vm_window.loop())
    {
    }

    return 0;
}