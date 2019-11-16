/*
- hoodhokage by ibanned -
*/

#pragma once

#include "Interfaces.h"

#include "Vector2D.h"

void Quad();


namespace Render
{
	void Initialise();

	// Normal Drawing functions
	void Clear(int x, int y, int w, int h, Color color);
	void Outline(int x, int y, int w, int h, Color color);
	void DrawOutlinedRect(int x, int y, int w, int h, Color col);
	void Line(int x, int y, int x2, int y2, Color color);
	void DrawLine(int x0, int y0, int x1, int y1, Color col);
	void PolyLine(int *x, int *y, int count, Color color);
	bool TransformScreen(const Vector & in, Vector & out);
	void rect(int x, int y, int w, int h, Color color);
	void Text2(int x, int y, const char * _Input, int font, Color color);
	RECT GetTextz(const char * _Input, int font);
	void DrawRect(int x, int y, int w, int h, Color col);
	void DrawRectRainbow(int x, int y, int w, int h, float flSpeed, float &flRainbow);
	void Polygon(int count, Vertex_t* Vertexs, Color color);
	void PolygonOutline(int count, Vertex_t* Vertexs, Color color, Color colorLine);
	void DrawFilledCircle(Vector2D center, Color color, float radius, float points);
	void PolyLine(int count, Vertex_t* Vertexs, Color colorLine);
	void DrawOutlinedRect3(int x, int y, int w, int h, Color col);
	void OutlinedRect(int x, int y, int w, int h, Color color_out, Color color_in);
	Color Color_spectrum_pen(int x, int y, int w, int h, Vector stx);
	void Color_spectrum(int x, int y, int w, int h);
	void Rect2(int x, int y, int w, int h, int r, int g, int b, int a);

	// Gradient Functions
	void GradientV(int x, int y, int w, int h, Color c1, Color c2);
	void Gradientp(Color c1, Color c2);
	void GradientH(int x, int y, int w, int h, Color c1, Color c2);
	void gradient_verticle(int x, int y, int w, int h, Color c1, Color c2);
	void text_yeti(int x, int y, const char * _Input, int font, Color color);
	void GradientSideways(int x, int y, int w, int h, Color color1, Color color2, int variation);
	void GradientTop(int x, int y, int w, int h, Color c1, Color c2, Color c3);
	void text_yeti(int x, int y, const char* _Input, int font, Color color);

	// Text functions
	namespace Fonts
	{
		extern DWORD Default;
		extern DWORD Menu;
		extern DWORD MenuBold;
		extern DWORD MenuBold2;
		extern DWORD ESP;
		extern DWORD WEPESP;
		extern DWORD TITLE;
		extern DWORD Watermark;
		extern DWORD Tabs;
		extern DWORD Tabs2;
		extern DWORD MenuSymbols;
		extern DWORD Slider;
		extern DWORD complexfont;
		extern DWORD MenuText;
		extern DWORD LBY;
		extern DWORD Menutabs;
		extern DWORD LBY2;
		extern DWORD MenuTabs2;
		extern DWORD LBY3;
		extern DWORD WaterMark;
		extern DWORD wallbang;
		extern DWORD IconGG;
		extern DWORD supremacy;
	};

	void Text(int x, int y, Color color, DWORD font, const char* text);
	void Textf(int x, int y, Color color, DWORD font, const char* fmt, ...);
	void Text(int x, int y, Color color, DWORD font, const wchar_t* text);
	RECT GetTextSize(DWORD font, const char* text);

	// Other rendering functions
	bool WorldToScreen(Vector &in, Vector &out);
	void DrawCircle(float x, float y, float r, float s, Color color);
	void FilledCircle(Vector2D position, float points, float radius, Color color);
	void TexturedPolygon(int n, Vertex_t * vertice, Color col);
	RECT GetViewport();
};

namespace D
{
	extern void DrawOutlinedCircle(int x, int y, int r, Color col);
};


