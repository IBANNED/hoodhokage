/*
- hoodhokage by ibanned -
*/

#include "ESP.h"
#include "Interfaces.h"
#include "RenderManager.h"
#include "EdgyLagComp.h"
#include "MemePredict.h"
#include "SDK.h"
#include "MiscClasses.h"
#include "Entities.h"
void CEsp::Init()
{
	BombCarrier = nullptr;
}

void CEsp::Move(CUserCmd *pCmd,bool &bSendPacket) 
{

}

void CEsp::BacktrackingCross(IClientEntity* pEntity)
{
	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		player_info_t pinfo;
		if (pEntity && pEntity != hackManager.pLocal() && !pEntity->IsDormant())
		{
			if (Interfaces::Engine->GetPlayerInfo(i, &pinfo) && pEntity->IsAlive())
			{
				if (Menu::Window.RageBotTab.AimbotBacktrack2.GetState())
				{
					if (hackManager.pLocal()->IsAlive())
					{
						for (int t = 0; t < 12; ++t)
						{
							Vector screenbacktrack[64][12];

							if (headPositions[i][t].simtime && headPositions[i][t].simtime + 1 > hackManager.pLocal()->GetSimulationTime())
							{
								if (Render::WorldToScreen(headPositions[i][t].hitboxPos, screenbacktrack[i][t]))
								{

									Interfaces::Surface->DrawSetColor(Color(255, 255, 255, 255));
									Interfaces::Surface->DrawOutlinedRect(screenbacktrack[i][t].x, screenbacktrack[i][t].y, screenbacktrack[i][t].x + 2, screenbacktrack[i][t].y + 2);

								}
							}
						}
					}
					else
					{
						memset(&headPositions[0][0], 0, sizeof(headPositions));
					}
				}
				else if (Menu::Window.LegitBotTab.AimbotBacktrack.GetState())
				{
					if (hackManager.pLocal()->IsAlive())
					{
						for (int t = 0; t < 12; ++t)
						{
							Vector screenbacktrack[64][12];

							if (headPositions[i][t].simtime && headPositions[i][t].simtime + 1 > hackManager.pLocal()->GetSimulationTime())
							{
								if (Render::WorldToScreen(headPositions[i][t].hitboxPos, screenbacktrack[i][t]))
								{

									Interfaces::Surface->DrawSetColor(Color(255, 255, 255, 255));
									Interfaces::Surface->DrawOutlinedRect(screenbacktrack[i][t].x, screenbacktrack[i][t].y, screenbacktrack[i][t].x + 2, screenbacktrack[i][t].y + 2);

								}
							}
						}
					}
					else
					{
						memset(&headPositions[0][0], 0, sizeof(headPositions));
					}
				}
			}
		}
	}
}

// Main ESP Drawing loop
void CEsp::Draw()
{
	IClientEntity *pLocal = hackManager.pLocal();

	if (Menu::Window.MiscTab.OtherSpectators.GetState())
	{
		SpecList();
	}

	if (Menu::Window.VisualsTab.RemoveScope.GetState())
		RemoveScope();

	// Loop through all active entitys
	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		// Get the entity
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		player_info_t pinfo;

		// The entity isn't some laggy peice of shit or something
		if (pEntity &&  pEntity != pLocal && !pEntity->IsDormant())
		{
			// Radar
			if (Menu::Window.LegitBotTab.OtherRadar.GetState())
			{
				DWORD m_bSpotted = NetVar.GetNetVar(0x839EB159);
				*(char*)((DWORD)(pEntity)+m_bSpotted) = 1;
			}

			// Is it a player?!
			if (/*Menu::Window.VisualsTab.FiltersPlayers.GetState() && */Interfaces::Engine->GetPlayerInfo(i, &pinfo) && pEntity->IsAlive())
			{
				DrawPlayer(pEntity, pinfo);
			}
			ClientClass* cClass = (ClientClass*)pEntity->GetClientClass();

			// Dropped weapons
			if (Menu::Window.VisualsTab.FiltersWeapons.GetState() && cClass->m_ClassID != (int)CSGOClassID::CBaseWeaponWorldModel && ((strstr(cClass->m_pNetworkName, "Weapon") || cClass->m_ClassID == (int)CSGOClassID::CDEagle || cClass->m_ClassID == (int)CSGOClassID::CAK47)))
			{
				DrawDrop(pEntity, cClass);
			}

			// If entity is the bomb
			if (cClass->m_ClassID == (int)CSGOClassID::CPlantedC4)
				DrawBombPlanted(pEntity, cClass);

			if (cClass->m_ClassID == (int)CSGOClassID::CC4)
				DrawBomb(pEntity, cClass);

			CUserCmd* cmdlist = *(CUserCmd**)((DWORD)Interfaces::pInput + 0xEC);
			CUserCmd* pCmd = cmdlist;

			if (Menu::Window.VisualsTab.Grenades.GetState())
				DrawDrop(pEntity, cClass);

		}

	}

	// Anti Flash
	if (Menu::Window.VisualsTab.OtherNoFlash.GetState())
	{
		DWORD m_flFlashMaxAlpha = NetVar.GetNetVar(0xFE79FB98);
		*(float*)((DWORD)pLocal + m_flFlashMaxAlpha) = 0;
	}
	DWORD m_flFlashMaxAlpha = NetVar.GetNetVar(0xFE79FB98);
	if (!Menu::Window.VisualsTab.OtherNoFlash.GetState())
		*(float*)((DWORD)pLocal + m_flFlashMaxAlpha) = 255;
}

void CEsp::SpecList()
{
	IClientEntity *pLocal = hackManager.pLocal();

	RECT scrn = Render::GetViewport();
	int ayy = 0;

	// Loop through all active entitys
	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		// Get the entity
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		player_info_t pinfo;

		// The entity isn't some laggy peice of shit or something
		if (pEntity &&  pEntity != pLocal)
		{
			if (Interfaces::Engine->GetPlayerInfo(i, &pinfo) && !pEntity->IsAlive() && !pEntity->IsDormant())
			{
				HANDLE obs = pEntity->GetObserverTargetHandle();

				if (obs)
				{
					IClientEntity *pTarget = Interfaces::EntList->GetClientEntityFromHandle(obs);
					player_info_t pinfo2;
					if (pTarget)
					{
						if (Interfaces::Engine->GetPlayerInfo(pTarget->GetIndex(), &pinfo2))
						{
							char buf[255]; sprintf_s(buf, "%s -> %s", pinfo.name, pinfo2.name);
							RECT TextSize = Render::GetTextSize(Render::Fonts::ESP, buf);
							Render::Clear(scrn.right - 260, (scrn.bottom / 2) + (16 * ayy), 260, 16, Color(0, 0, 0, 0));
							Render::Text(scrn.right - TextSize.right - 4, (scrn.bottom / 2) + (16 * ayy), pTarget->GetIndex() == pLocal->GetIndex() ? Color(255, 20, 20, 255) : Color(230, 230, 230, 255), Render::Fonts::ESP, buf);
							ayy++;
						}
					}
				}
			}
		}
	}
}

//  Yeah m8
void CEsp::DrawPlayer(IClientEntity* pEntity, player_info_t pinfo)
{
	ESPBox Box;
	Color Color;
	IGameEvent* event;
	Vector max = pEntity->GetCollideable()->OBBMaxs(); //
	Vector pos, pos3D;
	Vector top, top3D;
	pos3D = pEntity->GetOrigin();
	top3D = pos3D + Vector(0, 0, max.z);

	if (!Render::WorldToScreen(pos3D, pos) || !Render::WorldToScreen(top3D, top))
		return;

	if (Menu::Window.VisualsTab.FiltersEnemiesOnly.GetState() && (pEntity->GetTeamNum() == hackManager.pLocal()->GetTeamNum()))
		return;

	if (GetBox(pEntity, Box))
	{
		Color = GetPlayerColor(pEntity);
		}

		if (Menu::Window.VisualsTab.OptionsBox.GetState())
			DrawBox(Box, Color);

		if (Menu::Window.VisualsTab.OptionsName.GetState())
			DrawName(pinfo, Box);

		switch (Menu::Window.VisualsTab.OptionsVitals.GetIndex())
		{
		case 0: break;
		case 1: DrawHealth2(pEntity, Box); break;
		case 2: DrawVitals(pEntity, Box); break;
		}

		if (Menu::Window.VisualsTab.OptionsInfo.GetState() || Menu::Window.VisualsTab.OptionsWeapon.GetState())
			DrawInfo(pEntity, Box);

		if (Menu::Window.VisualsTab.OptionsAimSpot.GetState())
			DrawCross(pEntity);

		if (Menu::Window.VisualsTab.OptionsSkeleton.GetState())
			DrawSkeleton(pEntity);

		if (Menu::Window.VisualsTab.BacktrackingLol.GetState())
			BacktrackingCross(pEntity);
}

// Returns false on failure nigga don't fail me
bool CEsp::GetBox(IClientEntity* pEntity, CEsp::ESPBox &result)
{
	// Variables
	Vector  vOrigin, min, max, sMin, sMax, sOrigin,
		flb, brt, blb, frt, frb, brb, blt, flt;
	float left, top, right, bottom;

	// Get the locations
	vOrigin = pEntity->GetOrigin();
	min = pEntity->collisionProperty()->GetMins() + vOrigin;
	max = pEntity->collisionProperty()->GetMaxs() + vOrigin;

	// Points of a 3d bounding box
	Vector points[] = 
	{
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z) 
	};

	// Get screen positions
	if (!Render::WorldToScreen(points[3], flb) || !Render::WorldToScreen(points[5], brt)
		|| !Render::WorldToScreen(points[0], blb) || !Render::WorldToScreen(points[4], frt)
		|| !Render::WorldToScreen(points[2], frb) || !Render::WorldToScreen(points[1], brb)
		|| !Render::WorldToScreen(points[6], blt) || !Render::WorldToScreen(points[7], flt))
		return false;

	// Put them in an array (maybe start them off in one later for speed?)
	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	// Init this shit
	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;

	// Find the bounding corners for our box
	for (int i = 1; i < 8; i++)
	{
		if (left > arr[i].x)
			left = arr[i].x;
		if (bottom < arr[i].y)
			bottom = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (top > arr[i].y)
			top = arr[i].y;
	}

	// Width / height
	result.x = left;
	result.y = top;
	result.w = right - left;
	result.h = bottom - top;

	return true;
}

// Get an entities color depending on team and vis ect
Color CEsp::GetPlayerColor(IClientEntity* pEntity)
{
	int TeamNum = pEntity->GetTeamNum();
	bool IsVis = GameUtils::IsVisible(hackManager.pLocal(), pEntity, (int)CSGOHitboxID::HITBOX_HEAD);

	Color color;

	int BoxvisR = Menu::Window.VisualsTab.BoxRVis.GetValue();
	int BoxvisG = Menu::Window.VisualsTab.BoxGVis.GetValue();
	int BoxvisB = Menu::Window.VisualsTab.BoxBVis.GetValue();
	int OpacidadeVis = Menu::Window.VisualsTab.OpacidadeVis.GetValue();

	int BoxR = Menu::Window.VisualsTab.BoxR.GetValue();
	int BoxG = Menu::Window.VisualsTab.BoxG.GetValue();
	int BoxB = Menu::Window.VisualsTab.BoxB.GetValue();
	int Opacidade = Menu::Window.VisualsTab.Opacidade.GetValue();

	if (TeamNum == TEAM_CS_T)
	{


		if (IsVis)
		{
			color = Color(BoxvisR, BoxvisG, BoxvisB, OpacidadeVis);
		}
		else
		{
			color = Color(BoxR, BoxG, BoxB, Opacidade);
		}
	}
	else
	{

		if (IsVis)
		{
			color = Color(BoxR, BoxG, BoxB, Opacidade);
		}
		else
		{
			color = Color(BoxvisR, BoxvisG, BoxvisB, OpacidadeVis);
		}
	}


	return color;
}

// 2D  Esp box
void CEsp::DrawBox(CEsp::ESPBox size, Color color)
{
	int VertLine = (((float)size.w) * (0.20f));
	int HorzLine = (((float)size.h) * (0.20f));

	// Full Box
	Render::Outline(size.x, size.y, size.w, size.h, color);
	Render::Outline(size.x - 1, size.y - 1, size.w + 2, size.h + 2, Color(10, 10, 10, 50));
	Render::Outline(size.x + 1, size.y + 1, size.w - 2, size.h - 2, Color(10, 10, 10, 50));
}

void CEsp::RemoveScope()
{
	IClientEntity *pLocal = hackManager.pLocal();
	
	if(pLocal->IsScoped())
	{
		int width = 0;
		int height = 0;
		Interfaces::Engine->GetScreenSize(width, height);
		Render::Outline((width / 2), 0, (width / 2) + 1, height, Color(0, 0, 0, 255));
		Render::Outline(0, (height / 2), width, (height / 2) + 1, Color(0, 0, 0, 255));
	}
	else
	{

		
	}

}

// Unicode Conversions
static wchar_t* CharToWideChar(const char* text)
{
	size_t size = strlen(text) + 1;
	wchar_t* wa = new wchar_t[size];
	mbstowcs_s(NULL, wa, size/4, text, size);
	return wa;
}

// Player name
void CEsp::DrawName(player_info_t pinfo, CEsp::ESPBox size)
{
	if (strlen(pinfo.name) > 16)
	{
		pinfo.name[12] = 0;
		strcat(pinfo.name, "...");
		RECT nameSize = Render::GetTextSize(Render::Fonts::supremacy, pinfo.name);
		Render::Text(size.x + (size.w / 2) - (nameSize.right / 2), size.y - 16, Color(255, 255, 255, 255), Render::Fonts::supremacy, pinfo.name);
	}
	else
	{
		RECT nameSize = Render::GetTextSize(Render::Fonts::supremacy, pinfo.name);
		Render::Text(size.x + (size.w / 2) - (nameSize.right / 2), size.y - 16, Color(255, 255, 255, 255), Render::Fonts::supremacy, pinfo.name);
	}
}

// Draw a health bar. For Tf2 when a bar is bigger than max health a second bar is displayed
void CEsp::DrawPenis(ESPBox box, IClientEntity* pEntity)
{
	float health = pEntity->GetHealth();

	if (health > 100)
		health = 100;

	int colors[2] = { 255 - (health * 2.55), health * 2.55 };

	Interfaces::Surface->DrawSetColor(10, 10, 10, 150);
	if (Menu::Window.VisualsTab.OptionsVitals.GetIndex())
	{
		Interfaces::Surface->DrawOutlinedRect(box.x - 5, box.y - 1, box.x - 1, box.y + box.h + 1);

		int height = health * box.h / 100;

		if (health > 0)
		{
			Interfaces::Surface->DrawSetColor(colors[0], colors[1], 0, 255);
			Interfaces::Surface->DrawFilledRect(box.x - 4,
				box.y + box.h - height,
				box.x - 2,
				box.y + box.h);
		}
	}
}

CEsp::ESPBox CEsp::GetBOXX(IClientEntity* pEntity)
{
	ESPBox result;
	// Variables
	Vector  vOrigin, min, max, sMin, sMax, sOrigin,
		flb, brt, blb, frt, frb, brb, blt, flt;
	float left, top, right, bottom;

	// Get the locations
	vOrigin = pEntity->GetOrigin();
	min = pEntity->collisionProperty()->GetMins() + vOrigin;
	max = pEntity->collisionProperty()->GetMaxs() + vOrigin;

	// Points of a 3d bounding box
	Vector points[] = { Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z) };

	// Get screen positions
	if (!Render::WorldToScreen(points[3], flb) || !Render::WorldToScreen(points[5], brt)
		|| !Render::WorldToScreen(points[0], blb) || !Render::WorldToScreen(points[4], frt)
		|| !Render::WorldToScreen(points[2], frb) || !Render::WorldToScreen(points[1], brb)
		|| !Render::WorldToScreen(points[6], blt) || !Render::WorldToScreen(points[7], flt))
		return result;

	// Put them in an array (maybe start them off in one later for speed?)
	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	// Init this shit
	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;

	// Find the bounding corners for our box
	for (int i = 1; i < 8; i++)
	{
		if (left > arr[i].x)
			left = arr[i].x;
		if (bottom < arr[i].y)
			bottom = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (top > arr[i].y)
			top = arr[i].y;
	}

	// Width / height
	result.x = left;
	result.y = top;
	result.w = right - left;
	result.h = bottom - top;

	return result;
}
void CEsp::DrawHealth2(IClientEntity* pEntity, CEsp::ESPBox size)
{
	int HPEnemy = 100;
	HPEnemy = pEntity->GetHealth();
	char nameBuffer[512];
	sprintf_s(nameBuffer, "%d", HPEnemy);
	float h = (size.h);
	float offset = (h / 4.f) + 5;
	float w = h / 64.f;
	float health = pEntity->GetHealth();
	UINT hp = h - (UINT)((h * health) / 100);

	ESPBox BOX = GetBOXX(pEntity);
	int Line_Size = (BOX.y - BOX.h) / 8;
	int Line_Size2 = (BOX.y + BOX.h) / 7;
	int Health = pEntity->GetHealth();
	if (Health > 100)
		Health = 100;
	int healthG = Health * 2.55;
	int healthR = 255 - healthG;
	int heightoffset = 5;
	Interfaces::Surface->DrawSetColor(10, 10, 10, 1);
	Interfaces::Surface->DrawOutlinedRect(BOX.x - 7, BOX.y + 5 - 1 - heightoffset, BOX.x - 3, BOX.y + BOX.h + 4 + 4 - heightoffset);
	Render::DrawRect((size.x - 5), size.y - 1, 4, h + 4, Color(0, 0, 0, 180));
	int hpBarH = Health * BOX.h / 100;

	if (Health > 0)
	{
		Interfaces::Surface->DrawSetColor(170, 255, 0, 255);
		Interfaces::Surface->DrawFilledRect(BOX.x - 4, BOX.y + 5 + BOX.h - hpBarH - heightoffset, BOX.x - 2, BOX.y + BOX.h + 7 - heightoffset);
	}

	if (Health < 100)
	{
		Render::Text(size.x - 10, size.y + hp - heightoffset, Color(255, 255, 255, 255), Render::Fonts::wallbang, nameBuffer);

	}
}

#define Surface Interfaces::Surface

void DrawOutlinedRect(int x, int y, int w, int h, Color col)
{
	Surface->DrawSetColor(col);
	Surface->DrawOutlinedRect(x, y, x + w, y + h);
}

void DrawOutlinedCircle(int x, int y, int r, Color col)
{
	Surface->DrawSetColor(col);
	Surface->DrawOutlinedCircle(x, y, r, 1);
}

void DrawLine(int x0, int y0, int x1, int y1, Color col)
{
	Surface->DrawSetColor(col);
	Surface->DrawLine(x0, y0, x1, y1);
}
void CEsp::DrawVitals(IClientEntity* pEntity, CEsp::ESPBox size)
{
	ESPBox HealthBar = size;
	HealthBar.y += (HealthBar.h + 6);
	HealthBar.h = 4;

	float HealthValue = pEntity->GetHealth();
	float HealthPerc = HealthValue / 100.f;
	float flBoxes = std::ceil(HealthValue / 10.f);
	float Width = (size.w * HealthPerc);
	HealthBar.w = Width;
	float h = (size.h);
	float offset = (h / 4.f) + 5;
	float w = h / 64.f;
	float health = pEntity->GetHealth();
	float flMultiplier = 12 / 360.f; flMultiplier *= flBoxes - 1;
	Color ColHealth = Color::FromHSB(flMultiplier, 1, 1);

	UINT hp = h - (UINT)((h * health) / 100); // Percentage

	int Red = 255 - (health*2.55);
	int Green = health*2.55;
	DrawOutlinedRect((size.x - 6), size.y - 1, 3, h + 2, Color(10, 10, 10, 150));
	DrawOutlinedRect(size.x - 5, size.y - 1, 1, (h / 10) * flBoxes + 1, ColHealth);

	for (int i = 0; i < 10; i++)
	{
		DrawLine((size.x - 6), size.y + i * (h / 10) - 1, size.x - 3, size.y + i * (h / 10), Color::Black());
	}
}


void CEsp::Barrel(CEsp::ESPBox size, Color color, IClientEntity* pEntity)
{
	float HealthValue = pEntity->GetHealth();
	float HealthPerc = HealthValue / 100.f;
	float flBoxes = std::ceil(HealthValue / 10.f);
	float Width = (size.w * HealthPerc);
	float h = (size.h);
	float offset = (h / 4.f) + 5;
	float w = h / 64.f;
	float health = pEntity->GetHealth();
	float flMultiplier = 12 / 360.f; flMultiplier *= flBoxes - 1;
	Color ColHealth = Color::FromHSB(flMultiplier, 1, 1);

	Vector src3D, src;
	src3D = pEntity->GetOrigin() - Vector(0, 0, 0);

	if (!Render::WorldToScreen(src3D, src))
		return;

	int ScreenWidth, ScreenHeight;
	Interfaces::Engine->GetScreenSize(ScreenWidth, ScreenHeight);

	int x = (int)(ScreenWidth * 0.5f);
	int y = 0;


	y = ScreenHeight;

	Render::Line((int)(src.x), (int)(src.y), x, y, ColHealth);
}

// Cleans the internal class name up to something human readable and nice
std::string CleanItemName(std::string name)
{
	std::string Name = name;
	// Tidy up the weapon Name
	if (Name[0] == 'C')
		Name.erase(Name.begin());

	// Remove the word Weapon
	auto startOfWeap = Name.find("Weapon");
	if (startOfWeap != std::string::npos)
		Name.erase(Name.begin() + startOfWeap, Name.begin() + startOfWeap + 6);

	return Name;
}

// Anything else: weapons, class state? idk
void CEsp::DrawInfo(IClientEntity* pEntity, CEsp::ESPBox size)
{
	int baimact;

	std::vector<std::string> Info;

	IClientEntity* pWeapon = Interfaces::EntList->GetClientEntityFromHandle((HANDLE)pEntity->GetActiveWeaponHandle());
	if (Menu::Window.VisualsTab.OptionsWeapon.GetState() && pWeapon)
	{
		ClientClass* cClass = (ClientClass*)pWeapon->GetClientClass();
		if (cClass)
		{
			std::string meme = CleanItemName(cClass->m_pNetworkName);
			RECT nameSize = Render::GetTextSize(Render::Fonts::supremacy, meme.c_str());
			if (Menu::Window.VisualsTab.OptionsWeapon.GetState()) {
				Render::Text(size.x + (size.w / 2) - (nameSize.right / 2), size.y + size.h + 3,
					Color(255, 255, 255, 255), Render::Fonts::supremacy, meme.c_str());
			}
		}
	}

	// Bomb Carrier
	if (Menu::Window.VisualsTab.OptionsInfo.GetState() && pEntity == BombCarrier)
	{
		Info.push_back("Bomb");
	}

	// Is Scoped
	if (Menu::Window.VisualsTab.IsScoped.GetState() && pEntity->IsScoped())
	{
		Info.push_back("Zoom");
	}

	if (Menu::Window.RageBotTab.baimall.GetState())
	{
		Info.push_back("Baim");
	}

	static RECT Size = Render::GetTextSize(Render::Fonts::Default, "Hi");
	int i = 0;
	for (auto Text : Info)
	{
		Render::Text(size.x + size.w + 3, size.y + (i*(Size.bottom)), Color(255, 255, 255, 255), Render::Fonts::MenuBold2, Text.c_str());
		i++;
	}
}

// Little cross on their heads
void CEsp::DrawCross(IClientEntity* pEntity)
{
	Vector cross = pEntity->GetHeadPos(), screen;
	static int Scale = 2;
	if (Render::WorldToScreen(cross, screen))
	{
		Render::Clear(screen.x - Scale, screen.y - (Scale * 2), (Scale * 2), (Scale * 4), Color(20, 20, 20, 160));
		Render::Clear(screen.x - (Scale * 2), screen.y - Scale, (Scale * 4), (Scale * 2), Color(20, 20, 20, 160));
		Render::Clear(screen.x - Scale - 1, screen.y - (Scale * 2) - 1, (Scale * 2) - 2, (Scale * 4) - 2, Color(250, 250, 250, 160));
		Render::Clear(screen.x - (Scale * 2) - 1, screen.y - Scale - 1, (Scale * 4) - 2, (Scale * 2) - 2, Color(250, 250, 250, 160));
	}
}

// Draws a dropped CS:GO Item
void CEsp::DrawDrop(IClientEntity* pEntity, ClientClass* cClass)
{
	Vector Box;
	CBaseCombatWeapon* Weapon = (CBaseCombatWeapon*)pEntity;
	IClientEntity* plr = Interfaces::EntList->GetClientEntityFromHandle((HANDLE)Weapon->GetOwnerHandle());
}

// Draw the planted bomb and timer
void CEsp::DrawBombPlanted(IClientEntity* pEntity, ClientClass* cClass)
{
	BombCarrier = nullptr;

	Vector vOrig; Vector vScreen;
	vOrig = pEntity->GetOrigin();
	CCSBomb* Bomb = (CCSBomb*)pEntity;

	if (Render::TransformScreen(vOrig, vScreen))
	{


		float flBlow = Bomb->GetC4BlowTime();
		float TimeRemaining = flBlow - (Interfaces::Globals->interval_per_tick * hackManager.pLocal()->GetTickBase());
		float TimeRemaining2;
		bool exploded = true;
		if (TimeRemaining < 0)
		{
			!exploded;

			TimeRemaining2 = 0;
		}
		else
		{
			exploded = true;
			TimeRemaining2 = TimeRemaining;
		}
		char buffer[64];
		if (exploded)
		{
			sprintf_s(buffer, "Bomb: %.1f", TimeRemaining2);
		}
		else
		{
			sprintf_s(buffer, "Bomb Undefusable", TimeRemaining2);
		}
		Render::Text(vScreen.x, vScreen.y, Color(0, 255, 50, 255), Render::Fonts::ESP, buffer);
	}
}

// Draw the bomb if it's dropped, or store the player who's carrying 
void CEsp::DrawBomb(IClientEntity* pEntity, ClientClass* cClass)
{
	// Null it out incase bomb has been dropped or planted
	BombCarrier = nullptr;
	CBaseCombatWeapon *BombWeapon = (CBaseCombatWeapon *)pEntity;
	Vector vOrig; Vector vScreen;
	vOrig = pEntity->GetOrigin();
	bool adopted = true;
	HANDLE parent = BombWeapon->GetOwnerHandle();
	if (parent || (vOrig.x == 0 && vOrig.y == 0 && vOrig.z == 0))
	{
		IClientEntity* pParentEnt = (Interfaces::EntList->GetClientEntityFromHandle(parent));
		if (pParentEnt && pParentEnt->IsAlive())
		{
			BombCarrier = pParentEnt;
			adopted = false;
		}
	}

	if (adopted)
	{
		if (Render::WorldToScreen(vOrig, vScreen))
		{
			Render::Text(vScreen.x, vScreen.y, Color(255, 255, 255, 255), Render::Fonts::MenuBold2, "Bomb");
		}
	}
}

void DrawBoneArray(int* boneNumbers, int amount, IClientEntity* pEntity, Color color)
{
	Vector LastBoneScreen;
	for (int i = 0; i < amount; i++)
	{
		Vector Bone = pEntity->GetBonePos(boneNumbers[i]);
		Vector BoneScreen;

		if (Render::WorldToScreen(Bone, BoneScreen))
		{
			if (i>0)
			{
				Render::Line(LastBoneScreen.x, LastBoneScreen.y, BoneScreen.x, BoneScreen.y, color);
			}
		}
		LastBoneScreen = BoneScreen;
	}
}

void CEsp::DrawSkeleton(IClientEntity* pEntity)
{
	studiohdr_t* pStudioHdr = Interfaces::ModelInfo->GetStudiomodel(pEntity->GetModel());

	if (!pStudioHdr)
		return;

	Vector vParent, vChild, sParent, sChild;

	for (int j = 0; j < pStudioHdr->numbones; j++)
	{
		mstudiobone_t* pBone = pStudioHdr->GetBone(j);

		if (pBone && (pBone->flags & BONE_USED_BY_HITBOX) && (pBone->parent != -1))
		{
			vChild = pEntity->GetBonePos(j);
			vParent = pEntity->GetBonePos(pBone->parent);

			if (Render::WorldToScreen(vParent, sParent) && Render::WorldToScreen(vChild, sChild))
			{
				Render::Line(sParent[0], sParent[1], sChild[0], sChild[1], Color(255, 255, 255, 255));
			}
		}
	}
}