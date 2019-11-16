#pragma once

#include "Hooks.h"
#define M_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811174502841027019385211055596446229489549303819644288109756659334461284756482337867831652712019091456485669234603486104543266482133936072602491412737245870066063155881748815209209628292540917153643678925903600113305305488204665213841469519415116094330572703657595919530921861173819326117931051185480744623799627495673518857527248912279381830119491298336733624406566430860213949463952247371907021798609437027705392171762931767523846748184676694051320005681271452635608277857713427577896091736371787214684409012249534301465495853710507922796892589235420199561121290219608640344181598136297747713099605187072113499999983729780499510597317328160963185950244594553469083026425223082533446850352619311881710100031378387528865875332083814206171776691473035982534904287554687311595628638823537875937519577818577805321712268066130019278766111959092164201989380952572010654858632788659361533818279682303019520353018529689957736225994138912497217752834791315155748572424541506959508295331168617278558890750983817546374649393192550604009277016711390098488240128583616035637076601047101819429555961989467678374494482553797747268471040475346462080466842590694912933136770289891521047521620569660240580

class Fonts
{
public:
	DWORD menu;
	DWORD font_icons;
	DWORD menu_bold;
	DWORD esp;
	DWORD esp_extra;
	DWORD esp_small;
	DWORD indicator;
};

class DrawManager
{
public:
	Fonts fonts;
	void rect(int x, int y, int w, int h, Color color)
	{
		Interfaces::Surface->DrawSetColor(color);
		Interfaces::Surface->DrawFilledRect(x, y, x + w, y + h);
	}

	void outlined_rect(int x, int y, int w, int h, Color color_out, Color color_in)
	{
		Interfaces::Surface->DrawSetColor(color_in);
		Interfaces::Surface->DrawFilledRect(x, y, x + w, y + h);

		Interfaces::Surface->DrawSetColor(color_out);
		Interfaces::Surface->DrawOutlinedRect(x, y, x + w, y + h);
	}

	void outline(int x, int y, int w, int h, Color color)
	{
		Interfaces::Surface->DrawSetColor(color);
		Interfaces::Surface->DrawOutlinedRect(x, y, x + w, y + h);
	}

	void line(int x, int y, int x2, int y2, Color color)
	{
		Interfaces::Surface->DrawSetColor(color);
		Interfaces::Surface->DrawLine(x, y, x2, y2);
	}

	void polyline(int *x, int *y, int count, Color color)
	{
		Interfaces::Surface->DrawSetColor(color);
		Interfaces::Surface->DrawPolyLine(x, y, count);
	}

	void polygon(int count, Vertex_t* Vertexs, Color color)
	{
		static int Texture = Interfaces::Surface->CreateNewTextureID(true);
		unsigned char buffer[4] = { 255, 255, 255, 255 };

		Interfaces::Surface->DrawSetTextureRGBA(Texture, buffer, 1, 1);
		Interfaces::Surface->DrawSetColor(color);
		Interfaces::Surface->DrawSetTexture(Texture);

		Interfaces::Surface->DrawTexturedPolygon(count, Vertexs);
	}

	void polygon_outlined(int count, Vertex_t* Vertexs, Color color, Color colorLine)
	{
		static int x[128];
		static int y[128];

		polygon(count, Vertexs, color);

		for (int i = 0; i < count; i++)
		{
			x[i] = Vertexs[i].m_Position.x;
			y[i] = Vertexs[i].m_Position.y;
		}

		polyline(x, y, count, colorLine);
	}

	void gradient_verticle(int x, int y, int w, int h, Color c1, Color c2)
	{
		rect(x, y, w, h, c1);
		BYTE first = c2.r();
		BYTE second = c2.g();
		BYTE third = c2.b();
		for (int i = 0; i < h; i++)
		{
			float fi = i, fh = h;
			float a = fi / fh;
			DWORD ia = a * 255;
			rect(x, y + i, w, 1, Color(first, second, third, ia));
		}
	}

	void gradient_horizontal(int x, int y, int w, int h, Color c1, Color c2)
	{
		rect(x, y, w, h, c1);
		BYTE first = c2.r();
		BYTE second = c2.g();
		BYTE third = c2.b();
		for (int i = 0; i < w; i++)
		{
			float fi = i, fw = w;
			float a = fi / fw;
			DWORD ia = a * 255;
			rect(x + i, y, 1, h, Color(first, second, third, ia));
		}
	}
	void text(int x, int y, const char* _Input, int font, Color color)
	{
		int apple = 0;
		char Buffer[2048] = { '\0' };
		va_list Args;
		va_start(Args, _Input);
		vsprintf_s(Buffer, _Input, Args);
		va_end(Args);
		size_t Size = strlen(Buffer) + 1;
		wchar_t* WideBuffer = new wchar_t[Size];
		mbstowcs_s(0, WideBuffer, Size, Buffer, Size - 1);

		Interfaces::Surface->DrawSetTextColor(color);
		Interfaces::Surface->DrawSetTextFont(font);
		Interfaces::Surface->DrawSetTextPos(x, y);
		Interfaces::Surface->DrawPrintText(WideBuffer, wcslen(WideBuffer));
	}
	RECT get_text_size(const char* _Input, int font)
	{
		int apple = 0;
		char Buffer[2048] = { '\0' };
		va_list Args;
		va_start(Args, _Input);
		vsprintf_s(Buffer, _Input, Args);
		va_end(Args);
		size_t Size = strlen(Buffer) + 1;
		wchar_t* WideBuffer = new wchar_t[Size];
		mbstowcs_s(0, WideBuffer, Size, Buffer, Size - 1);
		int Width = 0, Height = 0;

		Interfaces::Surface->GetTextSize(font, WideBuffer, Width, Height);

		RECT outcome = { 0, 0, Width, Height };
		return outcome;
	}

	void color_spectrum(int x, int y, int w, int h)
	{
		static int GradientTexture = 0;
		static std::unique_ptr<Color[]> Gradient = nullptr;
		if (!Gradient)
		{
			Gradient = std::make_unique<Color[]>(w * h);

			for (int i = 0; i < w; i++)
			{
				int div = w / 6;
				int phase = i / div;
				float t = (i % div) / (float)div;
				int r, g, b;

				switch (phase)
				{
				case(0):
					r = 255;
					g = 255 * t;
					b = 0;
					break;
				case(1):
					r = 255 * (1.f - t);
					g = 255;
					b = 0;
					break;
				case(2):
					r = 0;
					g = 255;
					b = 255 * t;
					break;
				case(3):
					r = 0;
					g = 255 * (1.f - t);
					b = 255;
					break;
				case(4):
					r = 255 * t;
					g = 0;
					b = 255;
					break;
				case(5):
					r = 255;
					g = 0;
					b = 255 * (1.f - t);
					break;
				}

				for (int k = 0; k < h; k++)
				{
					float sat = k / (float)h;
					int _r = r + sat * (128 - r);
					int _g = g + sat * (128 - g);
					int _b = b + sat * (128 - b);

					*reinterpret_cast<Color*>(Gradient.get() + i + k * w) = Color(_r, _g, _b);
				}
			}

			GradientTexture = Interfaces::Surface->CreateNewTextureID(true);
			Interfaces::Surface->DrawSetTextureRGBA(GradientTexture, (unsigned char*)Gradient.get(), w, h);
		}
		Interfaces::Surface->DrawSetColor(Color(255, 255, 255, 255));
		Interfaces::Surface->DrawSetTexture(GradientTexture);
		Interfaces::Surface->DrawTexturedRect(x, y, x + w, y + h);
	}

	Color color_spectrum_pen(int x, int y, int w, int h, Vector stx)
	{
		int div = w / 6;
		int phase = stx.x / div;
		float t = ((int)stx.x % div) / (float)div;
		int r, g, b;

		switch (phase)
		{
		case(0):
			r = 255;
			g = 255 * t;
			b = 0;
			break;
		case(1):
			r = 255 * (1.f - t);
			g = 255;
			b = 0;
			break;
		case(2):
			r = 0;
			g = 255;
			b = 255 * t;
			break;
		case(3):
			r = 0;
			g = 255 * (1.f - t);
			b = 255;
			break;
		case(4):
			r = 255 * t;
			g = 0;
			b = 255;
			break;
		case(5):
			r = 255;
			g = 0;
			b = 255 * (1.f - t);
			break;
		}

		float sat = stx.y / h;
		return Color(r + sat * (128 - r), g + sat * (128 - g), b + sat * (128 - b), 255);
	}

	void filled_circle(int x, int y, float points, float radius, Color color)
	{
		std::vector<Vertex_t> vertices;
		float step = (float)M_PI * 2.0f / points;

		for (float a = 0; a < (M_PI * 2.0f); a += step)
			vertices.push_back(Vertex_t(Vector2D(radius * cosf(a) + x, radius * sinf(a) + y)));

		polygon((int)points, vertices.data(), color);
	}
};
extern DrawManager draw;