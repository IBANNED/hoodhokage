﻿#include "Controls.h"
#include "RenderManager.h"
#include "Menu.h"
#include "Gui.h"

#define UI_COL_MAIN2		Color(27, 220, 117, 255)
#define UI_COL_SHADOW2		Color(0, 0, 0, 255)

#define white Color(255, 255, 255)
#define mixed Color(90, 90, 90)
#define lighter_gray Color(48, 48, 48)
#define light_gray Color(40, 40, 40)
#define gray Color(28, 28, 28)
#define dark_gray Color(21, 21, 19)
#define darker_gray Color(19, 19, 19)
#define black Color(0, 0, 0)
#pragma region Base Control

void CControl::SetPosition(int x, int y)
{
	m_x = x;
	m_y = y;
}

void CControl::SetSize(int w, int h)
{
	m_iWidth = w;
	m_iHeight = h;
}

void CControl::GetSize(int &w, int &h)
{
	w = m_iWidth;
	h = m_iHeight;
}

bool CControl::Flag(int f)
{
	if (m_Flags & f)
		return true;
	else
		return false;
}

POINT CControl::GetAbsolutePos()
{
	POINT p;
	RECT client = parent->GetClientArea();
	if (parent)
	{
		p.x = m_x + client.left;
		p.y = m_y + client.top + 29;
	}

	return p;
}

void CControl::SetFileId(std::string fid)
{
	FileIdentifier = fid;
}
#pragma endregion Implementations of the Base control functions

#pragma region CheckBox
CCheckBox::CCheckBox()
{
	Checked = false;
	bIsSub = false;

	m_Flags = UIFlags::UI_Clickable | UIFlags::UI_Drawable | UIFlags::UI_SaveFile;
	m_iHeight = 9;

	FileControlType = UIControlTypes::UIC_CheckBox;
}


void CCheckBox::SetState(bool s)
{
	Checked = s;
}

bool CCheckBox::GetState()
{
	return Checked;
}

void CCheckBox::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	Render::GradientV(a.x, a.y, 14, 14, Color(55, 55, 55, 255), Color(30, 30, 30, 255));
	Render::Outline(a.x, a.y, 14, 14, Color(2, 2, 2, 255));

	if (hover)
	{
		Render::Text(a.x + 1, a.y - 1, Color(120, 120, 120, 120), Render::Fonts::MenuSymbols, L"✔");
		Render::Outline(a.x, a.y, 14, 14, Color(120, 120, 120, 255));
	}
	if (Checked)
	{
		Render::Text(a.x + 1, a.y - 1, Color(255, 255, 255, 255), Render::Fonts::MenuSymbols, L"✔");
		Render::Outline(a.x, a.y, 14, 14, Color(2, 2, 2, 255));
	}
}

void CCheckBox::OnUpdate() { m_iHeight = 9; }

void CCheckBox::OnClick()
{

	Checked = !Checked;
}
#pragma endregion Implementations of the Check Box functions

#pragma region Label
CLabel::CLabel()
{
	m_Flags = UIFlags::UI_Drawable;
	FileControlType = UIC_Label;
	Text = "Default";
	FileIdentifier = "Default";
	m_iHeight = 10;
}

void CLabel::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	Render::Text2(a.x, a.y - 1, Text.c_str(), Render::Fonts::Menu, Color(205, 205, 205, 255));
}

void CLabel::SetText(std::string text)
{
	Text = text;
}

void CLabel::OnUpdate() {}
void CLabel::OnClick() {}
#pragma endregion Implementations of the Label functions

#pragma region GroupBox2

CGroupBox2::CGroupBox2()
{
	Items = 1;
	last_y = 0;
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_RenderFirst;
	Text = "Default";
	FileIdentifier = "Default";
	FileControlType = UIControlTypes::UIC_GroupBox;
}

void CGroupBox2::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	RECT txtSize = Render::GetTextSize(Render::Fonts::IconGG, Text.c_str());

	Render::DrawRect(a.x + 2, a.y + 2, m_iWidth - 4, m_iHeight - 4, Color(90, 90, 90, 1));
	Render::Text2(a.x + (m_iWidth / 2) - (txtSize.right / 2), a.y - (txtSize.bottom / 2) - 1, Text.c_str(), Render::Fonts::IconGG, Color(244, 244, 244, 255));
	if (group_tabs.size())
	{
		Render::Line(a.x + 1, a.y + 8, a.x + m_iWidth, a.y + 8, Color(48, 48, 48, 255));
		Render::Line(a.x + 1, a.y + 38, a.x + m_iWidth, a.y + 38, Color(48, 48, 48, 255));
		//	Render::DrawRect(a.x + 1, a.y + 9, m_iWidth - 1, 29, Color(21, 21, 19, 255));
		//	Render::DrawRect(a.x + 1, a.y + 36, m_iWidth - 1, 2, Color(62, 62, 62, 255));

		for (int i = 0; i < group_tabs.size(); i++)
		{
			RECT text_size = Render::GetTextSize(Render::Fonts::IconGG, group_tabs[i].name.c_str());

			int width = m_iWidth - 1;

			int tab_length = (width / group_tabs.size());

			int text_position[] = {
				(a.x + (tab_length * (i + 1)) - (tab_length / 2)),
				a.y + 23 - (text_size.bottom / 2)
			};

			RECT tab_area = {
				(a.x + 1) + (tab_length * i),
				a.y + 9,
				tab_length,
				29
			};

			if (GetAsyncKeyState(VK_LBUTTON))
			{
				if (GUI.IsMouseInRegion(tab_area))
				{
					selected_tab = group_tabs[i].id;
				}
			}

			if (selected_tab == group_tabs[i].id)
			{
				//Render::DrawRect(tab_area.left, tab_area.top, tab_area.right, tab_area.bottom, Color(0, 204, 0, 255));
				Render::DrawRect(tab_area.left, tab_area.top, tab_area.right, tab_area.bottom - 0, Color(39, 39, 39, 255));
				Render::Text2(text_position[0] - (text_size.right / 2), text_position[1], group_tabs[i].name.c_str(), Render::Fonts::IconGG, Color(205, 205, 205, 245));
			}
			else if (selected_tab != group_tabs[i].id)
				Render::Text2(text_position[0] - (text_size.right / 2), text_position[1], group_tabs[i].name.c_str(), Render::Fonts::IconGG, Color(92, 92, 92, 255));
		}
	}


	//	Render::Clear(a.x + 2, a.y + 2, m_iWidth - 4, m_iHeight - 4, Color(90, 90, 90, 255));
	Render::Text(a.x + (m_iWidth / 2) - (txtSize.right / 2), a.y - (txtSize.bottom / 2) - 1, Color(244, 244, 244, 255), Render::Fonts::IconGG, Text.c_str());

	Render::Line(a.x, a.y, a.x + (m_iWidth / 2) - (txtSize.right / 2) - 2, a.y, Color(48, 48, 48, 255));
	Render::Line(a.x - 1, a.y - 1, a.x + (m_iWidth / 2) - (txtSize.right / 2) - 1, a.y - 1, Color(0, 0, 0, 255));

	//Top Right
	Render::Line(a.x + (m_iWidth / 2) + (txtSize.right / 2) + 2, a.y, a.x + m_iWidth, a.y, Color(48, 48, 48, 255));
	Render::Line(a.x + (m_iWidth / 2) + (txtSize.right / 2) + 2, a.y - 1, a.x + m_iWidth + 1, a.y - 1, Color(0, 0, 0, 255));

	//Left
	Render::Line(a.x, a.y, a.x, a.y + m_iHeight, Color(49, 49, 49, 255));
	Render::Line(a.x - 1, a.y, a.x - 1, a.y + m_iHeight, Color(0, 0, 0, 255));

	//Bottom
	Render::Line(a.x, a.y + m_iHeight, a.x + m_iWidth, a.y + m_iHeight, Color(48, 48, 48, 255));
	Render::Line(a.x - 1, a.y + m_iHeight + 1, a.x + m_iWidth + 2, a.y + m_iHeight + 1, Color(0, 0, 0, 255));

	//Right
	Render::Line(a.x + m_iWidth, a.y, a.x + m_iWidth, a.y + m_iHeight + 1, Color(48, 48, 48, 255));
	Render::Line(a.x + m_iWidth + 1, a.y, a.x + m_iWidth + 1, a.y + m_iHeight + 1, Color(0, 0, 0, 255));

}

void CGroupBox2::SetText2(std::string text)
{
	Text = text;
}

void CGroupBox2::PlaceLabledControl2(int g_tab, std::string Label, CTab *Tab, CControl* control)
{
	bool has_tabs = group_tabs.size() ? 1 : 0;

	if (has_tabs) {
		bool has_reset = false;

		for (int i = 0; i < reset_tabs.size(); i++) {
			if (reset_tabs[i] == g_tab)
				has_reset = true;
		}

		if (!has_reset) {
			initialized = false;
			reset_tabs.push_back(g_tab);
		}
	}

	if (!initialized) {
		Items = 0;
		last_y = has_tabs ? m_y + 48 : m_y + 8;
		initialized = true;
	}

	bool add_label_y = true;
	bool is_checkbox = control->FileControlType == UIControlTypes::UIC_CheckBox;
	bool is_label = control->FileControlType == UIControlTypes::UIC_Label;
	bool is_keybind = control->FileControlType == UIControlTypes::UIC_KeyBind;
	bool is_color = control->FileControlType == UIControlTypes::UIC_ColorSelector;

	int x = m_x + 38;
	int y = last_y;
	int control_width, control_height;
	control->GetSize(control_width, control_height);

	CLabel* label = new CLabel;
	label->SetPosition(x, y);
	label->SetText(Label);
	label->parent_group = this;
	label->g_tab = g_tab ? g_tab : 0;
	Tab->RegisterControl(label);

	if (is_checkbox || is_label || is_color) add_label_y = false;

	if (Label != "" && add_label_y && !is_keybind) {
		RECT label_size = Render::GetTextSize(Render::Fonts::Menu, Label.c_str());
		last_y += 14;
		y = last_y;
	}

	if (!is_keybind)
		last_control_height = control_height + 7;

	if (is_keybind || (is_color && Label == "")) {
		y -= last_control_height;
		x = m_x + m_iWidth - 36;
	}
	if (is_color && Label != "")
		x = m_x + m_iWidth - 36;
	if (is_checkbox)
		x -= 24;

	control->SetPosition(x, is_checkbox ? y + 1 : y);
	control->SetSize(m_iWidth - (38 * 2), control_height);
	control->parent_group = this;
	control->g_tab = g_tab ? g_tab : 0;
	Tab->RegisterControl(control);


	if (!is_keybind) {
		if (!is_color || is_color && Label != "")
			last_y += control_height + 7;
	}
}
void CGroupBox2::AddTab2(CGroupTab t)
{
	group_tabs.push_back(t);

	if (selected_tab == 0)
		selected_tab++;
}
void CGroupBox2::OnUpdate() {}
void CGroupBox2::OnClick() {}

#pragma endregion Implementations of the group box functions

#pragma region GroupBox
CGroupBox::CGroupBox()
{
	Items = 1;
	last_y = 0;
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_RenderFirst;
	Text = "Default";
	FileIdentifier = "Default";
	FileControlType = UIControlTypes::UIC_GroupBox;
}


void CGroupBox::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	RECT txtSize = Render::GetTextSize(Render::Fonts::Menu, Text.c_str());

	Render::DrawRect(a.x + 2, a.y + 2, m_iWidth - 4, m_iHeight - 4, Color(90, 90, 90, 1));
	Render::Text2(a.x + (m_iWidth / 2) - (txtSize.right / 2), a.y - (txtSize.bottom / 2) - 1, Text.c_str(), Render::Fonts::Menu, Color(244, 244, 244, 255));
	if (group_tabs.size())
	{
		Render::Line(a.x + 1, a.y + 8, a.x + m_iWidth, a.y + 8, Color(48, 48, 48, 255));
		Render::Line(a.x + 1, a.y + 38, a.x + m_iWidth, a.y + 38, Color(48, 48, 48, 255));
		//	Render::DrawRect(a.x + 1, a.y + 9, m_iWidth - 1, 29, Color(21, 21, 19, 255));
		//	Render::DrawRect(a.x + 1, a.y + 36, m_iWidth - 1, 2, Color(62, 62, 62, 255));

		for (int i = 0; i < group_tabs.size(); i++)
		{
			RECT text_size = Render::GetTextSize(Render::Fonts::Menu, group_tabs[i].name.c_str());

			int width = m_iWidth - 1;

			int tab_length = (width / group_tabs.size());

			int text_position[] = {
				(a.x + (tab_length * (i + 1)) - (tab_length / 2)),
				a.y + 23 - (text_size.bottom / 2)
			};

			RECT tab_area = {
				(a.x + 1) + (tab_length * i),
				a.y + 9,
				tab_length,
				29
			};

			if (GetAsyncKeyState(VK_LBUTTON))
			{
				if (GUI.IsMouseInRegion(tab_area))
				{
					selected_tab = group_tabs[i].id;
				}
			}

			if (selected_tab == group_tabs[i].id)
			{
				//Render::DrawRect(tab_area.left, tab_area.top, tab_area.right, tab_area.bottom, Color(0, 204, 0, 255));
				Render::DrawRect(tab_area.left, tab_area.top, tab_area.right, tab_area.bottom - 0, Color(39, 39, 39, 255));
				Render::Text2(text_position[0] - (text_size.right / 2), text_position[1], group_tabs[i].name.c_str(), Render::Fonts::Menu, Color(205, 205, 205, 245));
			}
			else if (selected_tab != group_tabs[i].id)
				Render::Text2(text_position[0] - (text_size.right / 2), text_position[1], group_tabs[i].name.c_str(), Render::Fonts::Menu, Color(92, 92, 92, 255));
		}
	}


	//	Render::Clear(a.x + 2, a.y + 2, m_iWidth - 4, m_iHeight - 4, Color(90, 90, 90, 255));
	Render::Text(a.x + (m_iWidth / 2) - (txtSize.right / 2), a.y - (txtSize.bottom / 2) - 1, Color(244, 244, 244, 255), Render::Fonts::Menu, Text.c_str());

	Render::Line(a.x, a.y, a.x + (m_iWidth / 2) - (txtSize.right / 2) - 2, a.y, Color(48, 48, 48, 255));
	Render::Line(a.x - 1, a.y - 1, a.x + (m_iWidth / 2) - (txtSize.right / 2) - 1, a.y - 1, Color(0, 0, 0, 255));

	//Top Right
	Render::Line(a.x + (m_iWidth / 2) + (txtSize.right / 2) + 2, a.y, a.x + m_iWidth, a.y, Color(48, 48, 48, 255));
	Render::Line(a.x + (m_iWidth / 2) + (txtSize.right / 2) + 2, a.y - 1, a.x + m_iWidth + 1, a.y - 1, Color(0, 0, 0, 255));

	//Left
	Render::Line(a.x, a.y, a.x, a.y + m_iHeight, Color(49, 49, 49, 255));
	Render::Line(a.x - 1, a.y, a.x - 1, a.y + m_iHeight, Color(0, 0, 0, 255));

	//Bottom
	Render::Line(a.x, a.y + m_iHeight, a.x + m_iWidth, a.y + m_iHeight, Color(48, 48, 48, 255));
	Render::Line(a.x - 1, a.y + m_iHeight + 1, a.x + m_iWidth + 2, a.y + m_iHeight + 1, Color(0, 0, 0, 255));

	//Right
	Render::Line(a.x + m_iWidth, a.y, a.x + m_iWidth, a.y + m_iHeight + 1, Color(48, 48, 48, 255));
	Render::Line(a.x + m_iWidth + 1, a.y, a.x + m_iWidth + 1, a.y + m_iHeight + 1, Color(0, 0, 0, 255));

}

void CGroupBox::SetText(std::string text)
{
	Text = text;
}

void CGroupBox::PlaceLabledControl(int g_tab, std::string Label, CTab *Tab, CControl* control)
{
	bool has_tabs = group_tabs.size() ? 1 : 0;

	if (has_tabs) {
		bool has_reset = false;

		for (int i = 0; i < reset_tabs.size(); i++) {
			if (reset_tabs[i] == g_tab)
				has_reset = true;
		}

		if (!has_reset) {
			initialized = false;
			reset_tabs.push_back(g_tab);
		}
	}

	if (!initialized) {
		Items = 0;
		last_y = has_tabs ? m_y + 48 : m_y + 8;
		initialized = true;
	}

	bool add_label_y = true;
	bool is_checkbox = control->FileControlType == UIControlTypes::UIC_CheckBox;
	bool is_label = control->FileControlType == UIControlTypes::UIC_Label;
	bool is_keybind = control->FileControlType == UIControlTypes::UIC_KeyBind;
	bool is_color = control->FileControlType == UIControlTypes::UIC_ColorSelector;

	int x = m_x + 38;
	int y = last_y;
	int control_width, control_height;
	control->GetSize(control_width, control_height);

	CLabel* label = new CLabel;
	label->SetPosition(x, y);
	label->SetText(Label);
	label->parent_group = this;
	label->g_tab = g_tab ? g_tab : 0;
	Tab->RegisterControl(label);

	if (is_checkbox || is_label || is_color) add_label_y = false;

	if (Label != "" && add_label_y && !is_keybind) {
		RECT label_size = Render::GetTextSize(Render::Fonts::Menu, Label.c_str());
		last_y += 14;
		y = last_y;
	}

	if (!is_keybind)
		last_control_height = control_height + 7;

	if (is_keybind || (is_color && Label == "")) {
		y -= last_control_height;
		x = m_x + m_iWidth - 36;
	}
	if (is_color && Label != "")
		x = m_x + m_iWidth - 36;
	if (is_checkbox)
		x -= 24;

	control->SetPosition(x, is_checkbox ? y + 1 : y);
	control->SetSize(m_iWidth - (38 * 2), control_height);
	control->parent_group = this;
	control->g_tab = g_tab ? g_tab : 0;
	Tab->RegisterControl(control);

	if (!is_keybind) {
		if (!is_color || is_color && Label != "")
			last_y += control_height + 7;
	}
}

void CGroupBox::AddTab(CGroupTab t)
{
	group_tabs.push_back(t);

	if (selected_tab == 0)
		selected_tab++;
}
void CGroupBox::OnUpdate() {}
void CGroupBox::OnClick() {}
#pragma endregion Implementations of the Group Box functions

#pragma region Sliders
CSlider2::CSlider2()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_SaveFile;
	m_iHeight = 10;
	FileControlType = UIControlTypes::UIC_Slider;
}

void CSlider2::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	RECT Region = { a.x, a.y, m_iWidth, 7 };
	Render::GradientV(a.x, a.y, m_iWidth, 7, Color(32, 32, 32, 255), Color(48, 48, 48, 245));

	float Ratio = (Value - Min) / (Max - Min);
	float Location = Ratio * m_iWidth;



	Render::GradientV(a.x, a.y, Location, 7, Color(255, 255, 255, 255), Color(20, 20, 20, 245));

	Render::Outline(a.x, a.y, m_iWidth, 7, Color(2, 2, 2, 255));

	if (GUI.IsMouseInRegion(Region))
	{
		Render::Outline(a.x, a.y, m_iWidth, 7, Color(120, 120, 120, 255));
	}

	char buffer[24];
	sprintf_s(buffer, "%.2f", Value);
	RECT txtSize = Render::GetTextSize(Render::Fonts::Slider, buffer);
	Render::Text(a.x + Location - txtSize.right / 2, a.y + 5, Color(200, 200, 200, 255), Render::Fonts::Slider, buffer);
}
void CSlider2::OnUpdate() {
	POINT a = GetAbsolutePos();
	m_iHeight = 15;

	if (DoDrag)
	{
		if (GUI.GetKeyState(VK_LBUTTON))
		{
			POINT m = GUI.GetMouse();
			float NewX;
			float Ratio;
			NewX = m.x - a.x;//-1
			if (NewX < 0)
				NewX = 0;
			if (NewX > m_iWidth)
				NewX = m_iWidth;
			Ratio = NewX / float(m_iWidth);
			Value = Min + (Max - Min)*Ratio;
		}
		else
		{
			DoDrag = false;
		}
	}
}

void CSlider2::OnClick() {
	POINT a = GetAbsolutePos();
	RECT SliderRegion = { a.x, a.y, m_iWidth, 11 };
	if (GUI.IsMouseInRegion(SliderRegion))
	{
		DoDrag = true;
	}
}

float CSlider2::GetValue()
{
	return Value;
}

void CSlider2::SetValue(float v)
{
	Value = v;
}

void CSlider2::SetBoundaries(float min, float max)
{
	Min = min; Max = max;
}

#pragma endregion Implementations of the Slider functions


#pragma region Sliders
CSlider::CSlider()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_SaveFile;
	m_iHeight = 10;
	FileControlType = UIControlTypes::UIC_Slider;
}

void CSlider::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	RECT Region = { a.x, a.y, m_iWidth, 7 };
	Render::GradientV(a.x, a.y, m_iWidth, 7, Color(32, 32, 32, 255), Color(48, 48, 48, 245));

	float Ratio = (Value - Min) / (Max - Min);
	float Location = Ratio * m_iWidth;



	Render::GradientV(a.x, a.y, Location, 7, Color(255, 255, 255, 255), Color(20, 20, 20, 245));

	Render::Outline(a.x, a.y, m_iWidth, 7, Color(2, 2, 2, 255));

	if (GUI.IsMouseInRegion(Region))
	{
		Render::Outline(a.x, a.y, m_iWidth, 7, Color(120, 120, 120, 255));
	}

	char buffer[24];
	sprintf_s(buffer, "%.f", Value);
	RECT txtSize = Render::GetTextSize(Render::Fonts::Slider, buffer);
	Render::Text(a.x + Location - txtSize.right / 2, a.y + 5, Color(200, 200, 200, 255), Render::Fonts::Slider, buffer);
}
void CSlider::OnUpdate() {
	POINT a = GetAbsolutePos();
	m_iHeight = 15;

	if (DoDrag)
	{
		if (GUI.GetKeyState(VK_LBUTTON))
		{
			POINT m = GUI.GetMouse();
			float NewX;
			float Ratio;
			NewX = m.x - a.x;//-1
			if (NewX < 0)
				NewX = 0;
			if (NewX > m_iWidth)
				NewX = m_iWidth;
			Ratio = NewX / float(m_iWidth);
			Value = Min + (Max - Min)*Ratio;
		}
		else
		{
			DoDrag = false;
		}
	}
}

void CSlider::OnClick() {
	POINT a = GetAbsolutePos();
	RECT SliderRegion = { a.x, a.y, m_iWidth, 11 };
	if (GUI.IsMouseInRegion(SliderRegion))
	{
		DoDrag = true;
	}
}

float CSlider::GetValue()
{
	return Value;
}

void CSlider::SetValue(float v)
{
	Value = v;
}

void CSlider::SetBoundaries(float min, float max)
{
	Min = min; Max = max;
}

#pragma endregion Implementations of the Slider functions

#pragma region CheckBox
CSeperate::CSeperate()
{
	Checked = false;
	//	bIsSub = false;

	m_Flags = UIFlags::UI_Clickable | UIFlags::UI_Drawable | UIFlags::UI_SaveFile;
	m_iHeight = 9;

	FileControlType = UIControlTypes::UIC_CheckBox;
}

void CSeperate::Draw(bool hover)
{
}

void CSeperate::OnUpdate() { m_iHeight = 9; }

void CSeperate::OnClick()
{
}
#pragma endregion Implementations of the Check Box functions


#pragma region KeyBinders

char* KeyStrings[254] = { "[ _ ]", "[M1]", "[M2]", "[BRK]", "[M3]", "[M4]", "[M5]",
"[ _ ]", "[BSPC]", "[TAB]", "[ _ ]", "[ _ ]", "[ _ ]", "[ENTER]", "[ _ ]", "[ _ ]", "[SHI]",
"[CTRL]", "[ALT]","[PAU]","[CAPS]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]",
"[ESC]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[SPACE]","[PGUP]", "[PGDOWN]", "[END]", "[HOME]", "[LEFT]",
"[UP]", "[RIGHT]", "[DOWN]", "[ _ ]", "[PRNT]", "[ _ ]", "[PRTSCR]", "[INS]","[DEL]", "[ _ ]", "[0]", "[1]",
"[2]", "[3]", "[4]", "[5]", "[6]", "[7]", "[8]", "[9]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]",
"[ _ ]", "[A]", "[B]", "[C]", "[D]", "[E]", "[F]", "[G]", "[H]", "[I]", "[J]", "[K]", "[L]", "[M]", "[N]", "[O]", "[P]", "[Q]", "[R]", "[S]", "[T]", "[U]",
"[V]", "[W]", "[X]","[Y]", "[Z]", "[LFTWIN]", "[RGHTWIN]", "[ _ ]", "[ _ ]", "[ _ ]", "[NUM0]", "[NUM1]",
"[NUM2]", "[NUM3]", "[NUM4]", "[NUM5]", "[NUM6]","[NUM7]", "[NUM8]", "[NUM9]", "[*]", "[+]", "[_]", "[-]", "[.]", "[/]", "[F1]", "[F2]", "[F3]",
"[F4]", "[F5]", "[F6]", "[F7]", "[F8]", "[F9]", "[F10]", "[F11]", "[F12]","[F13]", "[F14]", "[F15]", "[F16]", "[F17]", "[F18]", "[F19]", "[F20]", "[F21]",
"[F22]", "[F23]", "[F24]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]","[ _ ]", "[ _ ]", "[ _ ]",
"[NUM LOCK]", "[SCROLL LOCK[", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]",
"[ _ ]", "[ _ ]","[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[LSHFT]", "[RSHFT]", "[LCTRL]",
"[RCTRL]", "[LMENU]", "[RMENU]", "[ _ ]","[ _ ]", "[ _ ]","[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]",
"[ _ ]", "[ _ ]", "[ _ ]", "[NTRK]", "[PTRK]", "[STOP]", "[PLAY]", "[ _ ]", "[ _ ]",
"[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[;]", "[+]", "[,]", "[-]", "[.]", "[/?]", "[~]", "[ _ ]", "[ _ ]",
"[ _ ]", "[ _ ]","[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]",
"[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]","[ _ ]",
"[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[{]", "[\\|]", "[}]", "['\"]", "[ _ ]",
"[ _ ]", "[ _ ]", "[ _ ]","[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]",
"[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]",
"[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]", "[ _ ]",
"[ _ ]", "[ _ ]" };

CKeyBind::CKeyBind()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_SaveFile;
	m_iHeight = 12;
	FileControlType = UIControlTypes::UIC_KeyBind;
}

void CKeyBind::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	if (this->IsGettingKey)
	{
		Render::Text2(a.x, a.y, "[-]", Render::Fonts::Menu, Color(110, 110, 110, 255));
	}
	else
	{
		if (key == -1)
			Render::Text2(a.x, a.y, "[-]", Render::Fonts::Menu, Color(110, 110, 110, 255));
		else
		{
			char* NameOfKey = KeyStrings[key];
			Render::Text2(a.x, a.y, NameOfKey, Render::Fonts::Menu, Color(110, 110, 110, 255));
		}
	}

}

void CKeyBind::OnUpdate() {
	m_iHeight = 13;
	RECT text_area;
	if (key == -1)
		text_area = Render::GetTextz("[-]", Render::Fonts::Menu);
	else
		text_area = Render::GetTextz(text, Render::Fonts::Menu);
	m_iWidth = text_area.right;
	POINT a = GetAbsolutePos();
	if (IsGettingKey)
	{
		for (int i = 0; i < 255; i++)
		{
			if (GUI.GetKeyPress(i))
			{
				if (i == VK_ESCAPE)
				{
					IsGettingKey = false;
					key = -1;
					text = "[-]";
					return;
				}

				key = i;
				IsGettingKey = false;
				text = KeyStrings[i];
				return;
			}
		}
	}
}

void CKeyBind::OnClick() {
	POINT a = GetAbsolutePos();
	if (!IsGettingKey)
		IsGettingKey = true;
}

int CKeyBind::GetKey()
{
	return key;
}

void CKeyBind::SetKey(int k)
{
	key = k;
	text = KeyStrings[k];
}

#pragma endregion Implementations of the KeyBind Control functions

#pragma region Button
CButton::CButton()
{
	m_iWidth = 177;
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable;
	FileControlType == UIControlTypes::UIC_Button;
	Text = "Default";
	CallBack = nullptr;
	FileIdentifier = "Default";
}

void CButton::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	Render::Outline(a.x, a.y, m_iWidth, m_iHeight, Color(2, 2, 2, 0));
	if (Menu::Window.isOpen())
	{
		if (hover)
			Render::GradientV(a.x + 2, a.y + 2, m_iWidth - 4, m_iHeight - 4, Color(47, 47, 47, 255), Color(44, 44, 44, 255));
		else
			Render::GradientV(a.x + 2, a.y + 2, m_iWidth - 4, m_iHeight - 4, Color(44, 44, 44, 255), Color(39, 39, 39, 255));
	}

	RECT TextSize = Render::GetTextSize(Render::Fonts::Menu, Text.c_str());
	int TextX = a.x + (m_iWidth / 2) - (TextSize.left / 2);
	int TextY = a.y + (m_iHeight / 2) - (TextSize.bottom / 2);

	Render::Text(TextX, TextY - 1, Color(150, 150, 150, 255), Render::Fonts::Menu, Text.c_str());
}

void CButton::SetText(std::string text)
{
	Text = text;
}

void CButton::SetCallback(CButton::ButtonCallback_t callback)
{
	CallBack = callback;
}

void CButton::OnUpdate()
{
	m_iHeight = 26;
}

void CButton::OnClick()
{
	if (CallBack)
		CallBack();
}
#pragma endregion Implementations of the Button functions

#pragma region ComboBox
CComboBox::CComboBox()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_Focusable | UIFlags::UI_SaveFile;
	m_iHeight = 16;
	FileControlType = UIControlTypes::UIC_ComboBox;
}

void CComboBox::Draw(bool hover)
{

	POINT a = GetAbsolutePos();
	RECT Region = { a.x, a.y, m_iWidth, 16 };
	Render::GradientV(a.x, a.y, m_iWidth, 16, Color(51, 51, 51, 255), Color(33, 33, 33, 255));
	Render::Outline(a.x, a.y, m_iWidth, 16, Color(0, 0, 0, 255));
	Render::Outline(a.x + 1, a.y + 1, m_iWidth - 2, 16 - 2, Color(48, 48, 48, 255));


	// Hover for the Top Box
	if (GUI.IsMouseInRegion(Region))
	{
		Render::GradientV(a.x, a.y, m_iWidth, 16, Color(51, 51, 51, 255), Color(33, 33, 33, 255));
		Render::Outline(a.x, a.y, m_iWidth, 16, Color(120, 120, 120, 255));
		Render::Outline(a.x + 1, a.y + 1, m_iWidth - 2, 16 - 2, Color(48, 48, 48, 255));
	}

	// If we have some items
	if (Items.size() > 0)
	{
		// The current item
		Render::Text(a.x + 5, a.y + 2, Color(240, 240, 240, 255), Render::Fonts::MenuBold, GetItem().c_str());

		// If the drop down part is open
		if (IsOpen)
		{
			Render::GradientV(a.x, a.y + 17, m_iWidth, Items.size() * 16, Color(51, 51, 51, 255), Color(33, 33, 33, 255));
			Render::Outline(a.x, a.y + 17, m_iWidth, Items.size() * 16, Color(9, 9, 9, 255));

			// Draw the items
			for (int i = 0; i < Items.size(); i++)
			{
				RECT ItemRegion = { a.x, a.y + 17 + i * 16, m_iWidth, 16 };


				if (GUI.IsMouseInRegion(ItemRegion))
				{
					Render::Text(a.x + 5, a.y + 19 + i * 16, Color(255, 0, 0, 255), Render::Fonts::MenuBold, Items[i].c_str());
				}
				else
				{
					Render::Text(a.x + 5, a.y + 19 + i * 16, Color(240, 240, 240, 230), Render::Fonts::MenuBold, Items[i].c_str());
				}
			}
		}
	}
	Vertex_t Verts2[3];
	Verts2[0].m_Position.x = a.x + m_iWidth - 10;
	Verts2[0].m_Position.y = a.y + 8;
	Verts2[1].m_Position.x = a.x + m_iWidth - 5;
	Verts2[1].m_Position.y = a.y + 8;
	Verts2[2].m_Position.x = a.x + m_iWidth - 7.5;
	Verts2[2].m_Position.y = a.y + 11;
	Render::Polygon(3, Verts2, Color(255, 255, 255, 220));
}

void CComboBox::AddItem(std::string text)
{
	Items.push_back(text);
	SelectedIndex = 0;
}

void CComboBox::OnUpdate()
{
	if (IsOpen)
	{
		m_iHeight = 16 + 16 * Items.size();

		if (parent->GetFocus() != this)
			IsOpen = false;
	}
	else
	{
		m_iHeight = 16;
	}

}

void CComboBox::OnClick()
{
	POINT a = GetAbsolutePos();
	RECT Region = { a.x, a.y, m_iWidth, 16 };

	if (IsOpen)
	{
		// If we clicked one of the items(Not in the top bar)
		if (!GUI.IsMouseInRegion(Region))
		{
			// Draw the items
			for (int i = 0; i < Items.size(); i++)
			{
				RECT ItemRegion = { a.x, a.y + 16 + i * 16, m_iWidth, 16 };

				// Hover
				if (GUI.IsMouseInRegion(ItemRegion))
				{
					SelectedIndex = i;
				}
			}
		}

		// Close the drop down
		IsOpen = false;
	}
	else
	{
		IsOpen = true;
	}

}

int CComboBox::GetIndex()
{
	return SelectedIndex;
}

void CComboBox::SetIndex(int index)
{
	SelectedIndex = index;
}

std::string CComboBox::GetItem()
{
	if (SelectedIndex >= 0 && SelectedIndex < Items.size())
	{
		return Items[SelectedIndex];
	}

	return "";
}

void CComboBox::SelectIndex(int idx)
{
	if (idx >= 0 && idx < Items.size())
	{
		SelectedIndex = idx;
	}
}

#pragma endregion Implementations of the ComboBox functions

char* KeyDigits[254] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
"Y", "Z", nullptr, nullptr, nullptr, nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6",
"7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "-", ".", "?", "~", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "{", "|", "}", "\"", nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

CTextField::CTextField()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_SaveFile;
	FileControlType = UIControlTypes::UIC_KeyBind;
}

std::string CTextField::getText()
{
	return text;
}

void CTextField::SetText(std::string stext)
{
	text = stext;
}

void CTextField::Draw(bool hover)
{
	POINT a = GetAbsolutePos();

	Render::Outline(a.x, a.y, m_iWidth, 16, Color(2, 2, 2, 255));
	Render::Clear(a.x + 2, a.y + 2, m_iWidth - 4, 12, Color(50, 50, 50, 255));
	if (hover || IsGettingKey)
		Render::Clear(a.x + 2, a.y + 2, m_iWidth - 4, 12, Color(55, 55, 55, 255));

	const char *cstr = text.c_str();

	Render::Text(a.x + 2, a.y + 2 - 1, Color(244, 244, 244, 255), Render::Fonts::Menu, cstr);
}

void CTextField::OnUpdate()
{
	m_iHeight = 16;
	POINT a = GetAbsolutePos();
	POINT b;
	const char *strg = text.c_str();

	if (IsGettingKey)
	{
		b = GetAbsolutePos();
		for (int i = 0; i < 255; i++)
		{

			if (GUI.GetKeyPress(i))
			{
				if (i == VK_ESCAPE || i == VK_RETURN || i == VK_INSERT)
				{
					IsGettingKey = false;
					return;
				}

				if (i == VK_BACK && strlen(strg) != 0)
				{
					text = text.substr(0, strlen(strg) - 1);
				}

				if (strlen(strg) < 20 && i != NULL && KeyDigits[i] != nullptr)
				{
					if (GetAsyncKeyState(VK_SHIFT))
					{
						text = text + KeyDigits[i];
					}
					else
					{
						text = text + KeyDigits[i];
					}
					return;
				}

				if (strlen(strg) < 20 && i == 32)
				{
					text = text + " ";
					return;
				}
			}
		}
	}
}

void CTextField::OnClick()
{
	POINT a = GetAbsolutePos();
	if (!IsGettingKey)
	{
		IsGettingKey = true;
	}
}

CColorSelector::CColorSelector()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_Focusable | UIFlags::UI_SaveFile;
	m_iHeight = 10;
	FileControlType = UIControlTypes::UIC_ColorSelector;
}

void CColorSelector::Draw(bool hover)
{
	POINT a = GetAbsolutePos();

	Color preview;
	preview.SetColor(color[0], color[1], color[2], color[3]);

	Render::rect(is_open && set_new_pos ? a.x + 194 : a.x, a.y, 16, 10, preview);
	Render::Outline(is_open && set_new_pos ? a.x + 194 : a.x, a.y, 16, 10, Color(2, 2, 2, 255));

	if (is_open && set_new_pos)
	{
		int _x = a.x + 6;
		int _y = a.y + 17;
		int _width = 200;
		int _height = 200;

		Render::Outline(_x - 6, _y - 6, _width + 12, _height + 12, darker_gray);
		Render::OutlinedRect(_x - 5, _y - 5, _width + 10, _height + 10, lighter_gray, light_gray);
		Render::OutlinedRect(_x, _y, _width, _height, lighter_gray, gray);
		_x += 5; _y += 5;
		Render::Color_spectrum(_x, _y, 190, 190);
	}
}

void CColorSelector::OnUpdate() {
	POINT a = GetAbsolutePos();

	if (is_open && !toggle)
	{
		m_x -= 194;
		set_new_pos = true;
		toggle = true;
	}
	else if (!is_open && toggle)
	{
		m_x += 194;
		set_new_pos = false;
		toggle = false;
	}

	if (is_open && set_new_pos && GetAsyncKeyState(VK_LBUTTON))
	{
		int _x = a.x + 11;
		int _y = a.y + 22;
		RECT color_region = { _x, _y, 190, 190 };
		if (GUI.IsMouseInRegion(color_region))
		{
			color[0] = Render::Color_spectrum_pen(_x, _y, 190, 190, Vector(GUI.GetMouse().x - _x, GUI.GetMouse().y - _y, 0)).r();
			color[1] = Render::Color_spectrum_pen(_x, _y, 190, 190, Vector(GUI.GetMouse().x - _x, GUI.GetMouse().y - _y, 0)).g();
			color[2] = Render::Color_spectrum_pen(_x, _y, 190, 190, Vector(GUI.GetMouse().x - _x, GUI.GetMouse().y - _y, 0)).b();
			color[3] = Render::Color_spectrum_pen(_x, _y, 190, 190, Vector(GUI.GetMouse().x - _x, GUI.GetMouse().y - _y, 0)).a();
		}
	}


	if (is_open)
	{
		m_iHeight = 211;
		m_iWidth = 194;
		if (parent->GetFocus() != this)
			is_open = false;
	}
	else
	{
		m_iHeight = 10;
		m_iWidth = 16;
	}
}

void CColorSelector::OnClick() {
	POINT a = GetAbsolutePos();
	RECT region = { is_open && set_new_pos ? a.x + 200 : a.x, a.y, 16, 10 };
	if (GUI.IsMouseInRegion(region)) is_open = !is_open;
}

#pragma endregion Implementation of the Textfield

#define LIST_ITEM_HEIGHT 10
#define LIST_SCROLL_WIDTH 0

#define UI_CURSORSIZE		12
#define UI_CURSORFILL		Color(255,255,255)
#define UI_CURSOROUTLINE	Color(20,20,20,140)

#define UI_WIN_TOPHEIGHT	26
#define UI_WIN_TITLEHEIGHT	32

#define UI_TAB_WIDTH		200
#define UI_TAB_HEIGHT		32

#define UI_WIN_CLOSE_X		20
#define UI_WIN_CLOSE_Y      6

#define UI_CHK_SIZE			16

#define COL_WHITE			Color(240, 240, 240)
#define UI_COL_MAIN			Color(28,136,0, 255)
#define UI_COL_MAINDARK		Color(28,136,0, 255)
#define UI_COL_FADEMAIN		Color(28,136,0, 255)
#define UI_COL_SHADOW		Color(0, 0, 0, 0)
#define UI_COL_CLIENTBACK	Color(74, 74, 74, 255)
#define UI_COL_TABSEPERATOR	Color(229, 229, 229, 255)
#define UI_COL_TABTEXT		Color(145, 145, 145, 255)
#define UI_COL_GROUPOUTLINE Color(222, 222, 222, 255)
#pragma region ListBox
CListBox::CListBox()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable;
	SelectedIndex = 0;
}

void CListBox::Draw(bool hover)
{
	int ItemsToDraw = m_iHeight / LIST_ITEM_HEIGHT;
	POINT a = GetAbsolutePos();

	Render::Outline(a.x, a.y, m_iWidth, m_iHeight + 2, UI_COL_CLIENTBACK);
	// The current item
	//Render::Text(a.x + (m_iWidth / 2) - (TextSize.left / 2), a.y + 2, Color(240, 240, 240, 255), Render::Fonts::Watermark, GetItem().c_str());
	if (Items.size() > 0)
	{
		int drawnItems = 0;
		for (int i = ScrollTop; (i < Items.size() && drawnItems < ItemsToDraw); i++)
		{
			Color textColor = Color(240, 240, 240);
			RECT ItemRegion = { a.x + 1, a.y + 1 + drawnItems * 16, m_iWidth - LIST_SCROLL_WIDTH - 2 , 16 };
			RECT TextSize = Render::GetTextSize(Render::Fonts::Watermark, Items[i].c_str());
			if (i == SelectedIndex)
			{
				textColor = Color(240, 240, 240);
				Render::GradientV(ItemRegion.left, ItemRegion.top + 2, ItemRegion.right, ItemRegion.bottom, Color(29, 29, 29, 255), Color(14, 14, 14, 255));
			}
			else if (GUI.IsMouseInRegion(ItemRegion))
			{
				textColor = Color(240, 240, 240);
			}

			Render::Text(a.x + (m_iWidth / 2) - (TextSize.left / 2), ItemRegion.top + 2, textColor, Render::Fonts::MenuText, Items[i].c_str());
			drawnItems++;
		}

		// Ratio of how many visible to how many are hidden
		float sizeRatio = float(ItemsToDraw) / float(Items.size());
		if (sizeRatio > 1.f) sizeRatio = 1.f;
		float posRatio = float(ScrollTop) / float(Items.size());
		if (posRatio > 1.f) posRatio = 1.f;

		sizeRatio *= m_iHeight;
		posRatio *= m_iHeight;

		Render::Clear(a.x + m_iWidth - LIST_SCROLL_WIDTH, a.y + posRatio, LIST_SCROLL_WIDTH, sizeRatio, UI_COL_TABTEXT);
	}

}

void CListBox::AddItem(std::string text, int value)
{
	Items.push_back(text);
	Values.push_back(value);
}

void CListBox::OnClick()
{
	int ItemsToDraw = m_iHeight / LIST_ITEM_HEIGHT;
	POINT a = GetAbsolutePos();

	// Check the items
	if (Items.size() > 0)
	{
		int drawnItems = 0;
		for (int i = ScrollTop; (i < Items.size() && drawnItems < ItemsToDraw); i++)
		{
			Color textColor = UI_COL_TABTEXT;
			RECT ItemRegion = { a.x + 1, a.y + 1 + drawnItems * 16, m_iWidth - LIST_SCROLL_WIDTH - 2 , 16 };
			if (GUI.IsMouseInRegion(ItemRegion))
			{
				SelectItem(i);
				return;
			}
			drawnItems++;
		}
	}

	// Scrollbar checks are in update so we can just drag

}

void CListBox::OnUpdate()
{
	int ItemsToDraw = m_iHeight / LIST_ITEM_HEIGHT;
	POINT a = GetAbsolutePos();

	// Did we click in the scrollbar??
	RECT Scroll = { a.x + m_iWidth - LIST_SCROLL_WIDTH , a.y + 1, LIST_SCROLL_WIDTH - 2 ,m_iHeight };
	if (GUI.IsMouseInRegion(Scroll) && GUI.GetKeyState(VK_LBUTTON))
	{
		// Ratio of how many visible to how many are hidden
		float ratio = float(ItemsToDraw) / float(Items.size());
		POINT m = GUI.GetMouse();
		m.y -= a.y;

		float sizeRatio = float(ItemsToDraw) / float(Items.size());
		sizeRatio *= m_iHeight;
		float heightDelta = m.y + sizeRatio - m_iHeight;
		if (heightDelta > 0)
			m.y -= heightDelta;

		float mPosRatio = float(m.y) / float(m_iHeight);
		ScrollTop = mPosRatio * Items.size();
		if (ScrollTop < 0)
			ScrollTop = 0;

	}
}

void CListBox::SetHeightInItems(int items)
{
	m_iHeight = items * LIST_ITEM_HEIGHT;
}

std::string CListBox::GetItem()
{
	if (SelectedIndex >= 0 && SelectedIndex < Items.size())
	{
		return Items[SelectedIndex];
	}

	return "Error";
}

#define LIST_ITEM_HEIGHT 16
#define LIST_SCROLL_WIDTH 8

#pragma region ListBox2
CListBox2::CListBox2()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_Focusable | UIFlags::UI_SaveFile;
	SelectedIndex = 0;
	FileControlType = UIControlTypes::UIC_ListBox;
}

void CListBox2::Draw(bool hover)
{
	int ItemsToDraw = m_iHeight / LIST_ITEM_HEIGHT;
	POINT a = GetAbsolutePos();

	Render::rect(a.x + 1, a.y + 1, m_iWidth - 2, m_iHeight - 2, Color(90, 90, 90, 1));

	//Top Left
	Render::Line(a.x, a.y, a.x + m_iWidth - 2, a.y, Color(48, 48, 48, 255));
	Render::Line(a.x - 1, a.y - 1, a.x + (m_iWidth / 2) - 1, a.y - 1, Color(0, 0, 0, 255));

	//Top Right
	Render::Line(a.x + (m_iWidth / 2) + 2, a.y, a.x + m_iWidth, a.y, Color(48, 48, 48, 255));
	Render::Line(a.x + (m_iWidth / 2) + 2, a.y - 1, a.x + m_iWidth + 1, a.y - 1, Color(0, 0, 0, 255));

	//Left
	Render::Line(a.x, a.y, a.x, a.y + m_iHeight, Color(49, 49, 49, 255));
	Render::Line(a.x - 1, a.y, a.x - 1, a.y + m_iHeight, Color(0, 0, 0, 255));

	//Bottom
	Render::Line(a.x, a.y + m_iHeight, a.x + m_iWidth, a.y + m_iHeight, Color(48, 48, 48, 255));
	Render::Line(a.x - 1, a.y + m_iHeight + 1, a.x + m_iWidth + 2, a.y + m_iHeight + 1, Color(0, 0, 0, 255));

	//Right
	Render::Line(a.x + m_iWidth, a.y, a.x + m_iWidth, a.y + m_iHeight + 1, Color(48, 48, 48, 255));
	Render::Line(a.x + m_iWidth + 1, a.y, a.x + m_iWidth + 1, a.y + m_iHeight + 1, Color(0, 0, 0, 255));

	if (Items.size() > 0)
	{
		int drawnItems = 0;
		for (int i = ScrollTop; (i < Items.size() && drawnItems < ItemsToDraw); i++)
		{
			Color textColor = Color(92, 92, 92, 255);
			RECT ItemRegion = { a.x + 1, a.y + 1 + drawnItems * 16, m_iWidth - LIST_SCROLL_WIDTH - 2 , 16 };

			if (i == SelectedIndex)
			{
				textColor = Color(245, 245, 245, 245);



				Render::gradient_verticle(ItemRegion.left, ItemRegion.top, ItemRegion.right, ItemRegion.bottom, Color(200, 0, 0, 255), Color(200, 0, 0, 255));
			}
			else if (GUI.IsMouseInRegion(ItemRegion))
			{
				textColor = Color(245, 245, 245, 245);
				Render::rect(ItemRegion.left, ItemRegion.top, ItemRegion.right, ItemRegion.bottom, Color(92, 92, 92, 255));
			}

			Render::text_yeti(ItemRegion.left + 4, ItemRegion.top + 2, Items[i].c_str(), Render::Fonts::MenuText, textColor);
			drawnItems++;
		}

		// Ratio of how many visible to how many are hidden
		float sizeRatio = float(ItemsToDraw) / float(Items.size());
		if (sizeRatio > 1.f) sizeRatio = 1.f;
		float posRatio = float(ScrollTop) / float(Items.size());
		if (posRatio > 1.f) posRatio = 1.f;

		sizeRatio *= m_iHeight;
		posRatio *= m_iHeight;

		Render::rect(a.x + m_iWidth - LIST_SCROLL_WIDTH, a.y + posRatio, LIST_SCROLL_WIDTH, sizeRatio, Color(52, 52, 52, 255));
	}

}

void CListBox2::AddItem(std::string text, int value)
{
	Items.push_back(text);
	Values.push_back(value);
}

void CListBox2::OnClick()
{
	int ItemsToDraw = m_iHeight / LIST_ITEM_HEIGHT;
	POINT a = GetAbsolutePos();

	// Check the items
	if (Items.size() > 0)
	{
		int drawnItems = 0;
		for (int i = ScrollTop; (i < Items.size() && drawnItems < ItemsToDraw); i++)
		{
			Color textColor = Color(92, 92, 92, 255);
			RECT ItemRegion = { a.x + 1, a.y + 1 + drawnItems * 16, m_iWidth - LIST_SCROLL_WIDTH - 2 , 16 };
			if (GUI.IsMouseInRegion(ItemRegion))
			{
				SelectItem(i);
				return;
			}
			drawnItems++;
		}
	}
}

void CListBox2::OnUpdate()
{
	int ItemsToDraw = m_iHeight / LIST_ITEM_HEIGHT;
	POINT a = GetAbsolutePos();

	// Did we click in the scrollbar??
	RECT Scroll = { a.x + m_iWidth - LIST_SCROLL_WIDTH , a.y + 1, LIST_SCROLL_WIDTH - 2 ,m_iHeight };

	if (GUI.IsMouseInRegion(Scroll) && GetAsyncKeyState(VK_LBUTTON)) dragging = true;
	else if (!GetAsyncKeyState(VK_LBUTTON) && dragging) dragging = false;

	if (dragging)
	{
		// Ratio of how many visible to how many are hidden
		float ratio = float(ItemsToDraw) / float(Items.size());
		POINT m = GUI.GetMouse();
		m.y -= a.y;

		float sizeRatio = float(ItemsToDraw) / float(Items.size());
		sizeRatio *= m_iHeight;
		float heightDelta = m.y + sizeRatio - m_iHeight;
		if (heightDelta > 0)
			m.y -= heightDelta;

		float mPosRatio = float(m.y) / float(m_iHeight);
		ScrollTop = mPosRatio * Items.size();
		if (ScrollTop < 0)
			ScrollTop = 0;
	}
}

void CListBox2::SetHeightInItems(int items)
{
	m_iHeight = items * LIST_ITEM_HEIGHT;
}

std::string CListBox2::GetItem()
{
	if (SelectedIndex >= 0 && SelectedIndex < Items.size())
	{
		return Items[SelectedIndex];
	}

	return "Error";
}

char* KeyDigitsLowercase[254] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x",
"y", "z", nullptr, nullptr, nullptr, nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6",
"7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "-", ".", "/?", "~", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "[", "\\", "]", "'", nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

char* KeyDigitsCapital[254] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
"Y", "Z", nullptr, nullptr, nullptr, nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6",
"7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "-", ".", "?", "~", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "{", "|", "}", "\"", nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

CTextField2::CTextField2()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_SaveFile;
	FileControlType = UIControlTypes::UIC_TextField;
	m_iHeight = 12;
}

std::string CTextField2::getText()
{
	return text;
}

void CTextField2::SetText(std::string stext)
{
	text = stext;
}

void CTextField2::Draw(bool hover)
{
	POINT a = GetAbsolutePos();

	std::string drawn_text = "[";

	const char *cstr = text.c_str();

	drawn_text += cstr;

	if (IsGettingKey)
		drawn_text += "_";

	drawn_text += "]";
	if (drawn_text == "[]")
		drawn_text = "[...]";

	Render::text_yeti(a.x, a.y, drawn_text.c_str(), Render::Fonts::MenuText, Color(244, 244, 244, 255));
}

void CTextField2::OnUpdate()
{
	POINT a = GetAbsolutePos();
	POINT b;
	const char *strg = text.c_str();

	if (IsGettingKey)
	{
		b = GetAbsolutePos();
		for (int i = 0; i < 255; i++)
		{

			if (GUI.GetKeyPress(i))
			{
				if (i == VK_ESCAPE || i == VK_RETURN || i == VK_INSERT)
				{
					IsGettingKey = false;
					return;
				}

				if (i == VK_BACK && strlen(strg) != 0)
				{
					text = text.substr(0, strlen(strg) - 1);
				}

				if (strlen(strg) < 20 && i != NULL && KeyDigitsCapital[i] != nullptr)
				{
					if (GetAsyncKeyState(VK_SHIFT))
					{
						text = text + KeyDigitsCapital[i];
					}
					else
					{
						text = text + KeyDigitsLowercase[i];
					}
					return;
				}

				if (strlen(strg) < 20 && i == 32)
				{
					text = text + " ";
					return;
				}
			}
		}
	}
}

void CTextField2::OnClick()
{
	POINT a = GetAbsolutePos();
	if (!IsGettingKey)
	{
		IsGettingKey = true;
	}
}
