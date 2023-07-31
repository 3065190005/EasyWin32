#include "framework.h"


#ifdef _DEBUG

#include "easyWindow/ApiWindow.h"
#include "easyWindow/VmWindow.h"
#include "easyWindow/Menu.h"

#include <fstream>
#include <sstream>
#include <iostream>


std::wstring menuString = LR"(
{
	"list":[
		"apple","banana","orange","watermelon"
	],
    "apple":{
        "name":"tools",
        "id":1,
        "childs":["banana","orange"]
    },
    "banana":{
        "name":"help",
        "id":2,
        "childs":[]
    },
    "orange":{
        "name":"save",
        "id":3,
        "childs":["banana"]
    },
    "watermelon":{
        "name":"else",
        "id":4,
        "childs":[]
    }
}



)";

void func()
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
        
        if (vm_window.hasMessage())
        {
            easyWindow::AttributeMap map;
            std::wstring msg = vm_window.getMessage();
            if (msg == L"update")
            {
                map[L"menu"] = menuString;
                vm_window.setAttributeById(L"0", map);
                map[L"border"] = L"false";
                vm_window.setAttributeById(L"1", map);
            }

            std::wcout << msg << std::endl;
        }
    }
}


int main() {
    func();
    return 0;
}

#endif
