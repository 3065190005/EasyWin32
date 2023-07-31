#include "menu.h"
#include "Lexical.h"
#include "cJSON.h"

namespace easyWindow
{
	Menu::Menu(std::wstring buf)
	{
		m_buf = buf;
	}
	Menu::~Menu()
	{
	}

	HMENU Menu::get()
	{
		HMENU ret = nullptr;

		char cstr[8096] = { 0 };
		sprintf_s(cstr, 4096, "%S", m_buf.c_str());

		std::list<std::wstring> menuList;
		std::list<std::string> menuNames;
		std::map<std::string,menuData> menuDatas;

		cJSON* json = cJSON_Parse(cstr);
		if (json)
		{
			ret = CreateMenu();

			// 获取所有菜单名
			cJSON* list = cJSON_GetObjectItem(json, "list");
			int list_size = cJSON_GetArraySize(list);
			for (auto i = 0; i < list_size; i++)
			{
				cJSON* name = cJSON_GetArrayItem(list, i);
				std::string name_str = cJSON_GetStringValue(name);
				menuNames.push_back(name_str);
			}

			// 通过菜单名初始化所有菜单结构体
			for (auto& i : menuNames)
			{
				cJSON* data = cJSON_GetObjectItem(json, i.c_str());
				if (data)
				{
					cJSON* name_json = cJSON_GetObjectItem(data, "name");
					cJSON* id_json = cJSON_GetObjectItem(data, "id");
					cJSON* childs_json = cJSON_GetObjectItem(data, "childs");

					std::string name = cJSON_GetStringValue(name_json);
					long id = cJSON_GetNumberValue(id_json);
					std::list<std::string> childs;
					
					cJSON* child_name_json = NULL;
					cJSON_ArrayForEach(child_name_json, childs_json)
					{
						std::string child_name = cJSON_GetStringValue(child_name_json);
						childs.push_back(child_name);
						menuDatas[child_name].isChild = true;
						menuDatas[child_name].parents.push_back(child_name);
					}

					menuDatas[i].id = id;
					menuDatas[i].name = name;
					menuDatas[i].childs = childs;
					if(!menuDatas[i].childs.empty())
						menuDatas[i].menu = CreateMenu();

				}
			}

			

			// 通过菜单结构体创建菜单

			for (auto& i : menuDatas)
			{
				auto name = i.first;
				auto data = i.second;

				if (data.childs.empty())
				{
					if (!data.parents.empty())
					{
						for (auto& parent : data.parents)
						{
							AppendMenuA(menuDatas[parent].menu, MF_STRING, data.id, data.name.c_str());
							DestroyMenu(data.menu);
						}
					}
				}
				else
				{
					for (auto& j : data.childs)
					{
						auto finder = menuDatas.find(j);
						if (finder != menuDatas.end())
						{
							AppendMenuA(data.menu, MF_POPUP, (LONG)finder->second.menu, finder->second.name.c_str());
						}
					}
				}

				if (data.isChild == false)
				{
					if(!data.childs.empty())
						AppendMenuA(ret, MF_POPUP, (LONG)data.menu, data.name.c_str());
					else
						AppendMenuA(ret, MF_STRING, data.id, data.name.c_str());
				}
			}

			if (menuDatas.empty())
				DestroyMenu(ret);

			cJSON_Delete(json);
		}

		return ret;
	}
}