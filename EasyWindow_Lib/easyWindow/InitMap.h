#pragma once
#ifndef __EASY_WINDOW_INITMEMERY__
#define __EASY_WINDOW_INITMEMERY__

namespace InitMap
{
	using AttributeMap = easyWindow::AttributeMap;

	AttributeMap window_map =
	{
		{L"id",L""},
		{L"screenx",L""},
		{L"screeny",L""},
		{L"fullscreen",L"false"},
		{L"resize",L"true"},
		{L"enable",L"true"},
		{L"maxbox",L"true"},
		{L"minbox",L"true"},

		{L"visible",L"true"},
		{L"x",L"0"},
		{L"y",L"0"},
		{L"w",L"1024"},
		{L"h",L"768"},
		{L"text",L""},
		{L"style",L"13107200"},
	};


	AttributeMap button_map =
	{
		{L"id",L""},
		{L"onclick",L"0"},

		{L"visible",L"true"},
		{L"x",L"0"},
		{L"y",L"0"},
		{L"w",L"80"},
		{L"h",L"90"},
		{L"text",L"Button"},
		{L"style",L"1342177280"},

		{L"btntype",L"5"},
		{L"fontfamily",L"Segoe UI"},
		{L"fontsize",L"10"},
		{L"enable",L"true"},
		{L"isclick",L"false"},
		{L"align",L"left"},
		{L"valign",L"top"}
	};

	AttributeMap edit_map =
	{
		{L"id",L""},

		{L"visible",L"true"},
		{L"x",L"0"},
		{L"y",L"0"},
		{L"w",L"300"},
		{L"h",L"300"},
		{L"text",L""},
		{L"style",L"1342177280"},
		{L"fontfamily",L"Segoe UI"},
		{L"fontsize",L"10"},
		{L"enable",L"true"},
		{L"fmtlines",L"true"},
		{L"banner",L""},
		{L"limit",L"0"},
		{L"readyonly",L"false"},

		{L"align",L"left"},
		{L"multiline",L"false"},
		{L"password",L"false"},
		{L"number",L"false"},
		{L"vscroll",L"false"},
		{L"hscroll",L"false"},
		{L"border",L"true"},

	};

	AttributeMap combobox_map =
	{
		{L"id",L""},
		{L"visible",L"true"},
		{L"x",L"0"},
		{L"y",L"0"},
		{L"w",L"80"},
		{L"h",L"90"},
		{L"style",L"1073741824"},

		{L"fontfamily",L"Segoe UI"},
		{L"fontsize",L"10"},
		{L"enable",L"true"},
		{L"list",L""},
		{L"current",L""},
		{L"banner",L""},

	};


	AttributeMap layer_map =
	{
		{L"id",L""},

		{L"visible",L"true"},
		{L"x",L"10"},
		{L"y",L"10"},
		{L"w",L"300"},
		{L"h",L"300"},
		{L"style",L"1073741824"},

		{L"enable",L"true"},
		{L"border",L"false"},
		{L"frame",L"false"},
		{L"resize",L"false"},

		{L"color",L"FFFFFF"},
		{L"acceptfile",L"true"},
	};


	AttributeMap progress_map =
	{
		{L"id",L""},

		{L"visible",L"true"},
		{L"x",L"0"},
		{L"y",L"0"},
		{L"w",L"100"},
		{L"h",L"50"},
		{L"style",L"1073741824"},

		{L"max",L"100"},
		{L"min",L"0"},
		{L"pos",L"1"},
		{L"resize",L"false"},
		{L"frame",L"false"},
		{L"step",L"1"},
		{L"update",L"false"},
	};


	AttributeMap scroll_map =
	{
		{L"id",L""},

		{L"visible",L"true"},
		{L"x",L"0"},
		{L"y",L"0"},
		{L"w",L"100"},
		{L"h",L"100"},
		{L"style",L"1073741824"},

		{L"vmin",L"0"},
		{L"vmax",L"100"},
		{L"vpos",L"0"},
		{L"vert",L"true"},
		{L"hmin",L"0"},
		{L"hmax",L"100"},
		{L"hpos",L"0"},
		{L"horz",L"true"},
	};


	AttributeMap listbox_map =
	{
		{L"id",L""},
		{L"visible",L"true"},
		{L"x",L"10"},
		{L"y",L"10"},
		{L"w",L"200"},
		{L"h",L"100"},
		{L"style",L"1073741824"},

		{L"fontfamily",L"Segoe UI"},
		{L"fontsize",L"10"},
		{L"enable",L"true"},
		{L"list",L""},
		{L"current",L""},
		{L"border",L"true"},

	};


	AttributeMap static_map =
	{
		{L"id",L""},
		{L"visible",L"true"},
		{L"x",L"10"},
		{L"y",L"10"},
		{L"w",L"10"},
		{L"h",L"10"},
		{L"style",L"1073741824"},
		{L"text",L"Static"},

		{L"fontfamily",L"Segoe UI"},
		{L"fontsize",L"13"},
		{L"enable",L"true"},
		{L"fontcolor",L"000000"},
		{L"bgcolor",L"FFFFFF"},
		{L"bgvisible",L"true"},
		

	};


	AttributeMap image_map =
	{
		{L"id",L""},
		{L"visible",L"true"},
		{L"x",L"10"},
		{L"y",L"10"},
		{L"w",L"100"},
		{L"h",L"100"},
		{L"style",L"1073741824"},

		{L"enable",L"true"},
		{L"path",L""},


	};

	using AttributeNode = std::map<std::wstring, AttributeMap>;

	AttributeNode attributeNode =
	{
		{L"window",window_map},
		{L"button",button_map},
		{L"edit",edit_map},
		{L"combobox",combobox_map},
		{L"layer",layer_map},
		{L"progress",progress_map},
		{L"scroll",scroll_map},
		{L"listbox",listbox_map},
		{L"static",static_map},
		{L"image",image_map},
	};
}


#endif