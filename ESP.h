/*
- hoodhokage by ibanned -
*/

#pragma once

#include "Hacks.h"

class CEsp : public CHack
{
public:
	void Init();
	void BacktrackingCross(IClientEntity * pEntity);
	void Draw();
	void Move(CUserCmd *pCmd, bool &bSendPacket);
	void DrawMolotov(IClientEntity * pEntity, ClientClass * cClass);
	void DrawSmoke(IClientEntity * pEntity, ClientClass * cClass);
	void DrawDecoy(IClientEntity * pEntity, ClientClass * cClass);
	void DrawHE(IClientEntity * pEntity, ClientClass * cClass);
private:

	// Other shit
	IClientEntity *BombCarrier;

	struct ESPBox
	{
		int x, y, w, h;
	};

	// Draw a player
	void DrawPlayer(IClientEntity* pEntity, player_info_t pinfo);

	// Get player info
	Color GetPlayerColor(IClientEntity* pEntity);
	bool GetBox(IClientEntity* pEntity, ESPBox &result);

	// Draw shit about player
	void SpecList();
	void DrawPlayer(IClientEntity * pEntity, player_info_t pinfo, RECT rect);
	void ThreeDBox(Vector minimo, Vector maximo, Vector position, Color cor);
	void DrawGlow(IClientEntity *pEntity, int r, int g, int b, int a);
	void DrawBox(ESPBox size, Color color);
	void RemoveScope();
	void DrawThrowable(IClientEntity* throwable);
	void Fill(CEsp::ESPBox size, Color color);
	void DrawName(player_info_t pinfo, ESPBox size);
	void BulletTrace(IClientEntity * pEntity, Color color);
	void DrawPenis(ESPBox box, IClientEntity* pEntity);
	CEsp::ESPBox GetBOXX(IClientEntity * pEntity);
	void DrawHealth2(IClientEntity * pEntity, CEsp::ESPBox size);
	void DrawVitals(IClientEntity* pEntity, ESPBox size);
	void Barrel(CEsp::ESPBox size, Color color, IClientEntity* pEntity);
	void DrawLBY(IClientEntity* pEntity, CUserCmd* pCmd);
	void WallBangIndicator();
	void DrawInfo(IClientEntity* pEntity, ESPBox size);
	void DrawCross(IClientEntity* pEntity);
	void Indicator();
	void DrawSkeleton(IClientEntity* pEntity);
	void BoxAndText(IClientEntity * entity, std::string text);
	void DrawGlow(Color color);

	void GrenadeTrace();
	void GrenadeTraceOffMeme();
	void ShowImpacts();
	void ShowImpactsOffMeme();
	void DrawChicken(IClientEntity* pEntity, ClientClass* cClass);
	void DrawWeapon2(IClientEntity * pEntity, CEsp::ESPBox size);
	void DrawIcon(IClientEntity * pEntity, CEsp::ESPBox size);
	void DrawDrop(IClientEntity* pEntity, ClientClass* cClass);
	void DrawGrenades(IClientEntity* pEntity);
	void DrawBombPlanted(IClientEntity* pEntity, ClientClass* cClass);
	void DrawBomb(IClientEntity* pEntity, ClientClass* cClass);
	void* pSkip;
};

namespace Globals
{
	extern CUserCmd* UserCmd;
	extern IClientEntity* Target;
	extern int Shots;
	extern bool change;
	extern int TargetID;
	extern bool didhitHS;
	extern int missedshots;
	extern std::map<int, QAngle> storedshit;
}