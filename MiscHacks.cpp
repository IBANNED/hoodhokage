/*
- hoodhokage by ibanned -
*/

#define _CRT_SECURE_NO_WARNINGS

#include "MiscHacks.h"
#include "Interfaces.h"
#include "RenderManager.h"

#include <time.h>



template<class T, class U>
inline T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;
	else if (in >= high)
		return high;
	else
		return in;
}

inline float bitsToFloat(unsigned long i)
{
	return *reinterpret_cast<float*>(&i);
}

inline float FloatNegate(float f)
{
	return bitsToFloat(FloatBits(f) ^ 0x80000000);
}

Vector AutoStrafeView;

void CMiscHacks::Init()
{
	// Any init
}

void CMiscHacks::Draw()
{
	// Any drawing	
	// Spams

	if (Menu::Window.MiscTab.OtherCrasher.GetState())
		ChatSpamInterwebz();
	
}

void CMiscHacks::Move(CUserCmd *pCmd, bool &bSendPacket)
{
	// Any Move Stuff
	

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame()) /*Auto-Ambient-Lighting*/
		AmbientLighting();

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame()) /*Auto-Ambient-Lighting*/
		AmbientExposure();

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame()) /*Auto-skybox*/
		AmbientSkybox();

	// Bhop
	if (Menu::Window.MiscTab.OtherAutoJump.GetState())
		AutoJump(pCmd);

	if (Menu::Window.LegitBotTab.AimbotAutoPistol.GetState())
		AutoPistol(pCmd);


	// AutoStrafe
	Interfaces::Engine->GetViewAngles(AutoStrafeView);
	switch (Menu::Window.MiscTab.OtherAutoStrafe.GetIndex())
	{
	case 0:
		// Off
		break;
	case 1:
		RageStrafe(pCmd);
		break;
	case 2:
		LegitStrafe(pCmd);
		break;
	}

	if (Menu::Window.VisualsTab.PostProcessing.GetState())
		SpoofPostProcessing();

	//if (Interfaces::Engine->IsInGame() && Interfaces::Engine->IsConnected())
	//	fakeping();

	//Fake Lag
	//if (Menu::Window.MiscTab.FakeLagEnable.GetState())
		//Fakelag(pCmd, bSendPacket);
}

static __declspec(naked) void __cdecl Invoke_NET_SetConVar(void* pfn, const char* cvar, const char* value)
{
	__asm 
	{
		push    ebp
			mov     ebp, esp
			and     esp, 0FFFFFFF8h
			sub     esp, 44h
			push    ebx
			push    esi
			push    edi
			mov     edi, cvar
			mov     esi, value
			jmp     pfn
	}
}
void DECLSPEC_NOINLINE NET_SetConVar(const char* value, const char* cvar)
{
	static DWORD setaddr = Utilities::Memory::FindPattern("engine.dll", (PBYTE)"\x8D\x4C\x24\x1C\xE8\x00\x00\x00\x00\x56", "xxxxx????x");
	if (setaddr != 0) 
	{
		void* pvSetConVar = (char*)setaddr;
		Invoke_NET_SetConVar(pvSetConVar, cvar, value);
	}
}

void change_name(const char* name)
{
	if (Interfaces::Engine->IsInGame() && Interfaces::Engine->IsConnected())
		NET_SetConVar(name, "name");
}

void CMiscHacks::AutoJump(CUserCmd *pCmd)
{
	if (pCmd->buttons & IN_JUMP && GUI.GetKeyState(VK_SPACE))
	{
		int iFlags = hackManager.pLocal()->GetFlags();
		if (!(iFlags & FL_ONGROUND))
			pCmd->buttons &= ~IN_JUMP;

		if (hackManager.pLocal()->GetVelocity().Length() <= 50)
		{
			pCmd->forwardmove = 450.f;
		}
	}
}

void CMiscHacks::Telehop(CUserCmd * pCmd)
{
	static bool bDirection = true;
	static bool takethatshitoff = false;
	int iFlags = hackManager.pLocal()->GetFlags();

	if ((pCmd->buttons & IN_JUMP) && (iFlags & FL_ONGROUND))
	{
		pCmd->forwardmove = 450;
	}

	if ((pCmd->buttons & IN_JUMP) || !(iFlags &
		FL_ONGROUND)) {
		if (bDirection)
		{
			pCmd->sidemove = -450.f;
			bDirection = false;
		}
		else
		{
			pCmd->sidemove = +450.f;
			bDirection = true;
		}
	}
}

void CMiscHacks::LegitStrafe(CUserCmd *pCmd)
{
	IClientEntity* pLocal = hackManager.pLocal();
	if (!(pLocal->GetFlags() & FL_ONGROUND))
	{
		pCmd->forwardmove = 0.0f;

		if (pCmd->mousedx < 0)
		{
			pCmd->sidemove = -450.0f;
		}
		else if (pCmd->mousedx > 0)
		{
			pCmd->sidemove = 450.0f;
		}
	}
}

void CMiscHacks::AutoPistol(CUserCmd* pCmd)
{
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());

	if (pWeapon)
	{

		if (GameUtils::IsBomb(pWeapon))
		{
			return;
		}
		if (!GameUtils::IsNotPistol(pWeapon))
		{
			return;
		}
		if (GameUtils::IsGrenade(pWeapon))
		{
			return;
		}

	}
	static bool WasFiring = false;

	if (GameUtils::IsPistol)
	{
		if (pCmd->buttons & IN_ATTACK)
		{
			if (WasFiring)
			{
				pCmd->buttons &= ~IN_ATTACK;
			}
		}
		WasFiring = pCmd->buttons & IN_ATTACK ? true : false;
	}
}

void CMiscHacks::RageStrafe(CUserCmd *pCmd)
{
	IClientEntity* pLocal = hackManager.pLocal();

	bool bKeysPressed = true;
	if (GUI.GetKeyState(0x41) || GUI.GetKeyState(0x57) || GUI.GetKeyState(0x53) || GUI.GetKeyState(0x44)) bKeysPressed = false;

	if ((GetAsyncKeyState(VK_SPACE) && !(pLocal->GetFlags() & FL_ONGROUND)) && bKeysPressed)
	{
		if (pCmd->mousedx > 1 || pCmd->mousedx < -1) {
			pCmd->sidemove = pCmd->mousedx < 0.f ? -450.f : 450.f;
		}
		else {
			pCmd->forwardmove = (1800.f * 4.f) / pLocal->GetVelocity().Length2D();
			pCmd->sidemove = (pCmd->command_number % 2) == 0 ? -450.f : 450.f;
			if (pCmd->forwardmove > 450.f)
				pCmd->forwardmove = 450.f;
		}
	}

/*	IClientEntity* pLocal = hackManager.pLocal();
	static bool bDirection = true;


	static float move = 450; //400.f; // move = max(move, (abs(cmd->move.x) + abs(cmd->move.y)) * 0.5f);
	float s_move = move * 0.5065f;


	if ((pCmd->buttons & IN_JUMP) || !(pLocal->GetFlags() & FL_ONGROUND))
	{
		pCmd->forwardmove = move * 0.015f;
		pCmd->sidemove += (float)(((pCmd->tick_count % 2) * 2) - 1) * s_move;

		if (pCmd->mousedx)
			pCmd->sidemove = (float)clamp(pCmd->mousedx, -1, 1) * s_move;

		static float strafe = pCmd->viewangles.y;

		float rt = pCmd->viewangles.y, rotation;
		rotation = strafe - rt;

		if (rotation < FloatNegate(Interfaces::Globals->interval_per_tick))
			pCmd->sidemove = -s_move;

		if (rotation > Interfaces::Globals->interval_per_tick)
			pCmd->sidemove = s_move;

		strafe = rt;
	} */
}

Vector GetAutostrafeView()
{
	return AutoStrafeView;
}

// �e սʿ
void CMiscHacks::ChatSpamInterwebz()
{
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.001)
		return;

	static bool wasSpamming = true;
	//static std::string nameBackup = "INTERWEBZ";

	if (wasSpamming)
	{
		static bool useSpace = true;
		if (useSpace)
		{
			change_name("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n----------------------------hoodhokage crasher----------------------------");
			useSpace = !useSpace;
		}
		else
		{
			change_name("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n----------------------------hoodhokage crasher----------------------------");
			useSpace = !useSpace;
		}
	}

	start_t = clock();
}

/*		if (Menu::Window.MiscTab.lbyidicador.GetState())
		{
			CUserCmd* cmdlist = *(CUserCmd**)((DWORD)Interfaces::pInput + 0xEC);
			CUserCmd* pCmd = cmdlist;

			IClientEntity* localplayer = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
			RECT TextSize = Render::GetTextSize(Render::Fonts::LBY2, "LBY");
			RECT scrn = Render::GetViewport();
			if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())
				if (pCmd->viewangles.y - *localplayer->GetLowerBodyYawTarget() >= -35 && pCmd->viewangles.y - *localplayer->GetLowerBodyYawTarget() <= 35)
					Render::Text(10, scrn.bottom - 75, Color(255, 0, 0, 255), Render::Fonts::LBY2, "LBY");
				else
					Render::Text(10, scrn.bottom - 75, Color(0, 255, 0, 255), Render::Fonts::LBY2, "LBY");
		}*/

void CMiscHacks::ChatSpamDisperseName()
{
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.001)
		return;

	static bool wasSpamming = true;
	//static std::string nameBackup = "INTERWEBZ";

	if (wasSpamming)
	{
		change_name("plaSticware");
	}

	start_t = clock();
}

//void CMiscHacks::fakeping()
//{
//	int value1337 = Menu::Window.RageBotTab.FakePingValue.GetValue();
//	ConVar* net_fakelag = Interfaces::CVar->FindVar("net_fakelag");
//	static SpoofedConvar* fakelagspoof = new SpoofedConvar(net_fakelag);
//	if (Menu::Window.RageBotTab.FakePingExploit.GetState())
//		fakelagspoof->SetInt(value1337); //value
//}

void CMiscHacks::ChatSpamName()
{
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.001)
		return;

	std::vector < std::string > Names;

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		// Get the entity
		IClientEntity *entity = Interfaces::EntList->GetClientEntity(i);

		player_info_t pInfo;
		// If it's a valid entity and isn't the player
		if (entity && hackManager.pLocal()->GetTeamNum() == entity->GetTeamNum() && entity != hackManager.pLocal())
		{
			ClientClass* cClass = (ClientClass*)entity->GetClientClass();

			// If entity is a player
			if (cClass->m_ClassID == (int)CSGOClassID::CCSPlayer)
			{
				if (Interfaces::Engine->GetPlayerInfo(i, &pInfo))
				{
					if (!strstr(pInfo.name, "GOTV"))
						Names.push_back(pInfo.name);
				}
			}
		}
	}

	static bool wasSpamming = true;
	//static std::string nameBackup = "INTERWEBZ.CC";

	int randomIndex = rand() % Names.size();
	char buffer[128];
	sprintf_s(buffer, "%s ", Names[randomIndex].c_str());

	if (wasSpamming)
	{
		change_name(buffer);
	}
	else
	{
		change_name ("p$i 1337");
	}

	start_t = clock();
}

void CMiscHacks::ChatSpamRegular()
{
	// Don't spam it too fast so you can still do stuff
	static clock_t start_t = clock();
	int spamtime = Menu::Window.MiscTab.OtherChatDelay.GetValue();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < spamtime)
		return;

	static bool holzed = true;

	if (Menu::Window.MiscTab.OtherTeamChat.GetState())
	{
		SayInTeamChat("plaSticware");
	}
	else
	{
		SayInChat("plaSticware");
	}

	start_t = clock();
}

void CMiscHacks::Crasher()
{
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.001)
		return;

	std::vector < std::string > Names;

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		// Get the entity
		IClientEntity *entity = Interfaces::EntList->GetClientEntity(i);

		player_info_t pInfo;
		// If it's a valid entity and isn't the player
		if (entity && hackManager.pLocal()->GetTeamNum() == entity->GetTeamNum() && entity != hackManager.pLocal())
		{
			ClientClass* cClass = (ClientClass*)entity->GetClientClass();

			// If entity is a player
			if (cClass->m_ClassID == (int)CSGOClassID::CCSPlayer)
			{
				if (Interfaces::Engine->GetPlayerInfo(i, &pInfo))
				{
					if (!strstr(pInfo.name, "GOTV"))
						Names.push_back(pInfo.name);
				}
			}
		}
	}

	static bool wasSpamming = true;
	//static std::string nameBackup = "INTERWEBZ.CC";

	int randomIndex = rand() % Names.size();
	char buffer[1024];
	sprintf_s(buffer, "%s ", Names[randomIndex].c_str());

	if (wasSpamming)
	{
		change_name("hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER  hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER ");
	}
	else
	{
		change_name("hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER  hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER hoodhokage CRASHER ");
	}

	start_t = clock();
}

void CMiscHacks::AmbientLighting()
{
	float AmbientRedAmount = Menu::Window.VisualsTab.AmbientRed.GetValue();
	float AmbientGreenAmount = Menu::Window.VisualsTab.AmbientGreen.GetValue();
	float AmbientBlueAmount = Menu::Window.VisualsTab.AmbientBlue.GetValue();

	ConVar* AmbientRedCvar = Interfaces::CVar->FindVar("mat_ambient_light_r");
	*(float*)((DWORD)&AmbientRedCvar->fnChangeCallback + 0xC) = NULL;
	AmbientRedCvar->SetValue(AmbientRedAmount);

	ConVar* AmbientGreenCvar = Interfaces::CVar->FindVar("mat_ambient_light_g");
	*(float*)((DWORD)&AmbientGreenCvar->fnChangeCallback + 0xC) = NULL;
	AmbientGreenCvar->SetValue(AmbientGreenAmount);

	ConVar* AmbientBlueCvar = Interfaces::CVar->FindVar("mat_ambient_light_b");
	*(float*)((DWORD)&AmbientBlueCvar->fnChangeCallback + 0xC) = NULL;
	AmbientBlueCvar->SetValue(AmbientBlueAmount);
}

void CMiscHacks::AmbientExposure()
{
	float AmbientExposureMax = Menu::Window.VisualsTab.AmbientExposure.GetValue();
	float AmbientExposureMin = Menu::Window.VisualsTab.AmbientExposure.GetValue();

	ConVar* AmbientExposure2 = Interfaces::CVar->FindVar("mat_autoexposure_max_multiplier");
	*(float*)((DWORD)&AmbientExposure2->fnChangeCallback + 0xC) = NULL;
	AmbientExposure2->SetValue(AmbientExposureMax);

	ConVar* AmbientExposure3 = Interfaces::CVar->FindVar("mat_autoexposure_min");
	*(float*)((DWORD)&AmbientExposure3->fnChangeCallback + 0xC) = NULL;
	AmbientExposure3->SetValue(AmbientExposureMin);
}

void CMiscHacks::SpoofPostProcessing()
{
	ConVar* Meme = Interfaces::CVar->FindVar("mat_postprocess_enable");
	SpoofedConvar* meme_spoofed = new SpoofedConvar(Meme);
	meme_spoofed->SetString("mat_postprocess_enable 0");
}

void CMiscHacks::AmbientSkybox()
{
	//skybox
	switch (Menu::Window.VisualsTab.AmbientSkybox.GetIndex())
	{ 
	case 0:
		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())
		{


		}	break;

	case 1:
		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame()) {
			ConVar* NightSkybox1 = Interfaces::CVar->FindVar("sv_skyname"); /*Night-Skybox*/
			*(float*)((DWORD)&NightSkybox1->fnChangeCallback + 0xC) = NULL;
			NightSkybox1->SetValue("sky_csgo_night02b");
		}
		break;

	case 2:
		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame()) {
			ConVar* NightSkybox2 = Interfaces::CVar->FindVar("sv_skyname"); /*Night-Skybox*/
			*(float*)((DWORD)&NightSkybox2->fnChangeCallback + 0xC) = NULL;
			NightSkybox2->SetValue("sky_csgo_night02");
		}
		break;

	case 3:
		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame()) {
			ConVar* SkyBoxJungle = Interfaces::CVar->FindVar("sv_skyname"); /*Night-Skybox*/
			*(float*)((DWORD)&SkyBoxJungle->fnChangeCallback + 0xC) = NULL;
			SkyBoxJungle->SetValue("sky_venice");
		}
		break;
	case 4:
		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame()) {
			ConVar* NoSkybox = Interfaces::CVar->FindVar("sv_skyname"); /*No-Skybox*/
			*(float*)((DWORD)&NoSkybox->fnChangeCallback + 0xC) = NULL;
			NoSkybox->SetValue("cs_tibet");
		}
		break;
	case 5:
		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame()) {
			ConVar* NoSkybox = Interfaces::CVar->FindVar("sv_skyname"); /*No-Skybox*/
			*(float*)((DWORD)&NoSkybox->fnChangeCallback + 0xC) = NULL;
			NoSkybox->SetValue("sky_cs15_daylight03_hdr");
		}
	}
}


