/*
Syn's hoodhokage 2015
*/
/*#include "backtracking.h"
#include "Hooks.h"
#include "Hacks.h"
#include "Chams.h"
#include "Menu.h"
#include "igameventmanager.h"
#include "Interfaces.h"
#include "RenderManager.h"
#include "MiscHacks.h"
#include "intrin.h"
#include "CRC32.h"
#include "Resolver.h"
#include "Ragebot.h"*/
#include "Hooks.h"
#include "Hacks.h"
#include "Chams.h"
//#include "Globalshhh.h"
#include "Menu.h"
#include "AntiAntiAim.h"
#include "Interfaces.h"
#include "RenderManager.h"
#include "MiscHacks.h"
#include "CRC32.h"
#include "DamageIndicator.h"
#include "CBulletListener.h"
#include "Resolver.h"
#include "Backtracking Rage.h"
#include "BulletTracers.h"
#include "IPrediction.h"
#include "Materials.h"
#include <math.h>
#include <ctime>
#include <intrin.h>
#include "backtracking.h"
#include "EdgyLagComp.h"
#include "igameventmanager.h"
#include <Windows.h>
#include "endme.h"
#include "extrahelper.h"
#include "AntiAim.h"
#include "MemePredict.h"
#include "hitmarker.h"
#include "SkinChanger.h"

extern float lineLBY;
extern float lineRealAngle;
extern float lineFakeAngle;
Vector LastAngleAAReal;
Vector LBYThirdpersonAngle;

int bigboi::indicator;

#define STUDIO_NONE						0x00000000
#define STUDIO_RENDER					0x00000001
#define STUDIO_VIEWXFORMATTACHMENTS		0x00000002
#define STUDIO_DRAWTRANSLUCENTSUBMODELS 0x00000004
#define STUDIO_TWOPASS					0x00000008
#define STUDIO_STATIC_LIGHTING			0x00000010
#define STUDIO_WIREFRAME				0x00000020
#define STUDIO_ITEM_BLINK				0x00000040
#define STUDIO_NOSHADOWS				0x00000080
#define STUDIO_WIREFRAME_VCOLLIDE		0x00000100
#define STUDIO_NOLIGHTING_OR_CUBEMAP	0x00000200
#define STUDIO_SKIP_FLEXES				0x00000400
#define STUDIO_DONOTMODIFYSTENCILSTATE	0x00000800	// TERROR

// Not a studio flag, but used to flag model as a non-sorting brush model
#define STUDIO_TRANSPARENCY				0x80000000

// Not a studio flag, but used to flag model as using shadow depth material override
#define STUDIO_SHADOWDEPTHTEXTURE		0x40000000

// Not a studio flag, but used to flag model as doing custom rendering into shadow texture
#define STUDIO_SHADOWTEXTURE			0x20000000

#define STUDIO_SKIP_DECALS				0x10000000

float fakeangle;
QAngle realangle;

#define SETANGLE 180
#pragma comment(lib, "Winmm.lib")
#define M_PI       3.14159265358979323846   // pi
float Bolbilize(float Yaw)
{
	if (Yaw > 180)
	{
		Yaw -= (round(Yaw / 360) * 360.f);
	}
	else if (Yaw < -180)
	{
		Yaw += (round(Yaw / 360) * -360.f);
	}
	return Yaw;
}

Vector CalcAngle69(Vector dst, Vector src)
{
	Vector angles;

	double delta[3] = { (src.x - dst.x), (src.y - dst.y), (src.z - dst.z) };
	double hyp = sqrt(delta[1] * delta[1] + delta[2] * delta[2]);
	angles.x = (float)(atan(delta[2] / hyp) * 180.0 / 3.14159265);
	angles.y = (float)(atanf(delta[1] / delta[0]) * 57.295779513082f);
	angles.z = 0.0f;

	if (delta[0] >= 1.9)
	{
		angles.y += 180.0f;
	}

	return angles;
}

bool thirdpersonBool = false;
Vector LastAngleAA;
int Globals::Shots;
bool Globals::change;
CUserCmd* Globals::UserCmd;
int Globals::TargetID;
IClientEntity* Globals::Target;
bool Resolver::didhitHS;
std::map<int, QAngle>Globals::storedshit;
int Globals::missedshots;

// Funtion Typedefs
typedef void(__thiscall* DrawModelEx_)(void*, void*, void*, const ModelRenderInfo_t&, matrix3x4*);
typedef void(__thiscall* PaintTraverse_)(PVOID, unsigned int, bool, bool);
typedef bool(__thiscall* InPrediction_)(PVOID);
typedef void(__stdcall *FrameStageNotifyFn)(ClientFrameStage_t);
typedef bool(__thiscall *FireEventClientSideFn)(PVOID, IGameEvent*);
typedef void(__thiscall* RenderViewFn)(void*, CViewSetup&, CViewSetup&, int, int);

using OverrideViewFn = void(__fastcall*)(void*, void*, CViewSetup*);
typedef float(__stdcall *oGetViewModelFOV)();
typedef void(__thiscall *SceneEnd_t)(void *pEcx);

SceneEnd_t pSceneEnd;
PaintTraverse_ oPaintTraverse;
DrawModelEx_ oDrawModelExecute;
FrameStageNotifyFn oFrameStageNotify;
OverrideViewFn oOverrideView;
FireEventClientSideFn oFireEventClientSide;
RenderViewFn oRenderView;


// Hook function prototypes
void __fastcall PaintTraverse_Hooked(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce);
bool __stdcall Hooked_InPrediction();
bool __fastcall Hooked_FireEventClientSide(PVOID ECX, PVOID EDX, IGameEvent *Event);
void __fastcall Hooked_DrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld);
bool __stdcall CreateMoveClient_Hooked(/*void* self, int edx,*/ float frametime, CUserCmd* pCmd);
void  __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage);
void __fastcall Hooked_OverrideView(void* ecx, void* edx, CViewSetup* pSetup);
float __stdcall GGetViewModelFOV();
void __fastcall Hooked_RenderView(void* ecx, void* edx, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw);
void __fastcall	hkSceneEnd(void *pEcx, void *pEdx);

// VMT Managers
namespace Hooks
{
	// VMT Managers
	Utilities::Memory::VMTManager VMTPanel; // Hooking drawing functions
	Utilities::Memory::VMTManager VMTClient; // Maybe CreateMove
	Utilities::Memory::VMTManager VMTClientMode; // CreateMove for functionality
	Utilities::Memory::VMTManager VMTModelRender; // DrawModelEx for chams
	Utilities::Memory::VMTManager VMTPrediction; // InPrediction for no vis recoil
	Utilities::Memory::VMTManager VMTPlaySound; // Autoaccept 
	Utilities::Memory::VMTManager VMTRenderView;
	Utilities::Memory::VMTManager VMTEventManager;
	Utilities::Memory::VMTManager VMTSurface;

};

DamageEventListener* DamageListener;

// Undo our hooks
void Hooks::UndoHooks()
{
	VMTPanel.RestoreOriginal();
	VMTPrediction.RestoreOriginal();
	VMTModelRender.RestoreOriginal();
	VMTClientMode.RestoreOriginal();
}

int hitmarkertime = 0;
void imfinnarunuponya(IGameEvent* pEvent)
{
	int attackerid = pEvent->GetInt("attacker");
	int entityid = Interfaces::Engine->GetPlayerForUserID(attackerid);
	if (Menu::Window.MiscTab.OtherHitmarker.GetState())
	{
		if (entityid == Interfaces::Engine->GetLocalPlayer())
		{
			hitmarkertime = 255;
			Globals::missedshots = 0;
			Globals::Shots--;
			switch (Menu::Window.MiscTab.OtherHitmarkerType.GetIndex())
			{
			case 0:
				PlaySoundA(rawData, NULL, SND_ASYNC | SND_MEMORY);
				break;
			case 1:
			    Interfaces::Surface->IPlaySound("buttons\\arena_switch_press_02.wav"); break; //metal (gamesense)
				break;
			case 2:
				PlaySoundA(bameWare, NULL, SND_ASYNC | SND_MEMORY);
				break;
			}
		}
	}
}

void Hooks::DrawBeamd(Vector src, Vector end, Color color)
{
	BeamInfo_t beamInfo;
	beamInfo.m_nType = TE_BEAMPOINTS;
	beamInfo.m_pszModelName = "sprites/physbeam.vmt";
	beamInfo.m_nModelIndex = -1;
	beamInfo.m_flHaloScale = 0.0f;
	beamInfo.m_flLife = 3.0f;
	beamInfo.m_flWidth = 7.0f;
	beamInfo.m_flEndWidth = 7.0f;
	beamInfo.m_flFadeLength = 0.0f;
	beamInfo.m_flAmplitude = 2.0f;
	beamInfo.m_flBrightness = color.a();
	beamInfo.m_flSpeed = 0.2f;
	beamInfo.m_nStartFrame = 0.f;
	beamInfo.m_flFrameRate = 0.f;
	beamInfo.m_flRed = color.r();
	beamInfo.m_flGreen = color.g();
	beamInfo.m_flBlue = color.b();
	beamInfo.m_nSegments = 2;
	beamInfo.m_bRenderable = true;
	beamInfo.m_nFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;
	beamInfo.m_vecStart = src;
	beamInfo.m_vecEnd = end;

	Beam_t* myBeam = Interfaces::g_pViewRenderBeams->CreateBeamPoints(beamInfo);

	if (myBeam)
		Interfaces::g_pViewRenderBeams->DrawBeam(myBeam);
}

// Initialise all our hooks
void Hooks::Initialise()
{
	Interfaces::Engine->ExecuteClientCmd("clear");
	Interfaces::CVar->ConsoleColorPrintf(Color(200, 200, 200, 255), ("\n injected successfully! hoodhokage\n"));
	Interfaces::CVar->ConsoleColorPrintf(Color(200, 200, 200, 255), ("\n undetected - press insert, everything have * give banned."));
	Interfaces::CVar->ConsoleColorPrintf(Color(200, 200, 200, 255), ("\n update day: %s \n"), __DATE__);
	Interfaces::CVar->ConsoleColorPrintf(Color(200, 200, 200, 255), ("\n update time: %s \n"), __TIME__);

	// Panel hooks for drawing to the screen via surface functions
	VMTPanel.Initialise((DWORD*)Interfaces::Panels);
	oPaintTraverse = (PaintTraverse_)VMTPanel.HookMethod((DWORD)&PaintTraverse_Hooked, Offsets::VMT::Panel_PaintTraverse);
	//Utilities::Log("Paint Traverse Hooked");

	// No Visual Recoi	l
	VMTPrediction.Initialise((DWORD*)Interfaces::Prediction);
	VMTPrediction.HookMethod((DWORD)&Hooked_InPrediction, 14);
	Utilities::Log("Prediction Open");

	// Chams
	VMTModelRender.Initialise((DWORD*)Interfaces::ModelRender);
	oDrawModelExecute = (DrawModelEx_)VMTModelRender.HookMethod((DWORD)&Hooked_DrawModelExecute, Offsets::VMT::ModelRender_DrawModelExecute);
	Utilities::Log("ModelExecute Open");

	DamageListener = new DamageEventListener(imfinnarunuponya);
	// ...In injection thread
	Interfaces::GameEventManager->AddListener(DamageListener, "player_hurt", false);

	// Setup ClientMode Hooks
	VMTClientMode.Initialise((DWORD*)Interfaces::ClientMode);
	VMTClientMode.HookMethod((DWORD)CreateMoveClient_Hooked, 24);

	oOverrideView = (OverrideViewFn)VMTClientMode.HookMethod((DWORD)&Hooked_OverrideView, 18);
	VMTClientMode.HookMethod((DWORD)&GGetViewModelFOV, 35);

	// Setup client hooks
	VMTClient.Initialise((DWORD*)Interfaces::Client);
	oFrameStageNotify = (FrameStageNotifyFn)VMTClient.HookMethod((DWORD)&Hooked_FrameStageNotify, 37);

	// p coder
	VMTRenderView.Initialise((DWORD*)Interfaces::RenderView);
	pSceneEnd = (SceneEnd_t)VMTRenderView.HookMethod((DWORD)&hkSceneEnd, 9);

	VMTEventManager.Initialise((DWORD*)Interfaces::GameEventManager);
	oFireEventClientSide = (FireEventClientSideFn)VMTEventManager.HookMethod((DWORD)&Hooked_FireEventClientSide, 9);
}

void MovementCorrection(CUserCmd* pCmd)
{

}

//---------------------------------------------------------------------------------------------------------
//                                         Hooked Functions
//---------------------------------------------------------------------------------------------------------

void SetClanTag(const char* tag, const char* name)
{
	static auto pSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(((DWORD)Utilities::Memory::FindPattern("engine.dll", (PBYTE)"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15\x00\x00\x00\x00\x6A\x24\x8B\xC8\x8B\x30", "xxxxxxxxx????xxxxxx")));
	pSetClanTag(tag, name);
}

void NoClantag()
{
	SetClanTag("", "");
}

void ClanTag()
{
	static int counter = 0;
	{
		static int motion = 0;
		int ServerTime = (float)Interfaces::Globals->interval_per_tick * hackManager.pLocal()->GetTickBase() * 2.5;

		if (counter % 48 == 0)
			motion++;
		int value = ServerTime % 17;
		switch (value) {
		case 0:SetClanTag("  1 4 213", "[hoodhokage]"); break;
		case 1:SetClanTag("4  53 653", "[hoodhokage]"); break;
		case 2:SetClanTag("12 43 642", "[hoodhokage]"); break;
		case 3:SetClanTag("78  543 3", "[hoodhokage]"); break;
		case 4:SetClanTag("43 4 5 45", "[hoodhokage]"); break;
		case 5:SetClanTag("65 4 46 7", "[hoodhokage]"); break;
		case 6:SetClanTag("hoodhokage", "[hoodhokage]"); break;
		case 7:SetClanTag("             ", "[hoodhokage]"); break;
		case 8:SetClanTag("hoodhokage", "[hoodhokage]"); break;
		case 9:SetClanTag("             ", "[hoodhokage]"); break;
		case 10:SetClanTag("hoodhokage", "[hoodhokage]"); break;
		case 11:SetClanTag("+ hoodhokage -", "[hoodhokage]"); break;
		case 12:SetClanTag("- hoodhokage +", "[hoodhokage]"); break;
		case 13:SetClanTag("+ hoodhokage -", "[hoodhokage]"); break;
		case 14:SetClanTag("- hoodhokage +", "[hoodhokage]"); break;
		case 15:SetClanTag("+ hoodhokage -", "[hoodhokage]"); break;
		case 16:SetClanTag("- hoodhokage +", "[hoodhokage]"); break;
		}
		counter++;
	}
}

int LagCompBreak() {
	IClientEntity *pLocalPlayer = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	Vector velocity = pLocalPlayer->GetVelocity();
	velocity.z = 0;
	float speed = velocity.Length();
	if (speed > 0.f) {
		auto distance_per_tick = speed * Interfaces::Globals->interval_per_tick;
		int choked_ticks = std::ceilf(65.f / distance_per_tick);
		return std::min<int>(choked_ticks, 14);
	}
	return 1;
}

bool __stdcall CreateMoveClient_Hooked(/*void* self, int edx,*/ float frametime, CUserCmd* pCmd)
{
	if (!pCmd->command_number)
		return true;


	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())
	{
		IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
		PVOID pebp;
		__asm mov pebp, ebp;
		bool* pbSendPacket = (bool*)(*(DWORD*)pebp - 0x1C);
		bool& bSendPacket = *pbSendPacket;
		static int ticks = 0;
		static int ticks1 = 0;
		static int iTick = 0;
		static int iTick1 = 0;
		static int iTick2 = 0;
		int ticksMax = 16;
		int value = Menu::Window.MiscTab.FakeLagChoke.GetValue();

		if (Menu::Window.MiscTab.OtherClantag.GetState())
			ClanTag();


		Vector origView = pCmd->viewangles;
		Vector viewforward, viewright, viewup, aimforward, aimright, aimup;
		Vector qAimAngles;
		qAimAngles.Init(0.0f, pCmd->viewangles.y, 0.0f);
		AngleVectors(qAimAngles, &viewforward, &viewright, &viewup);


		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && pLocal && pLocal->IsAlive())
			Hacks::MoveHacks(pCmd, bSendPacket);
		backtracking->legitBackTrack(pCmd, pLocal);
		backtracking->ragebackTrack2(pCmd, pLocal);

		//Movement Fix
		qAimAngles.Init(0.0f, GetAutostrafeView().y, 0.0f); // if pCmd->viewangles.x > 89, set pCmd->viewangles.x instead of 0.0f on first
		AngleVectors(qAimAngles, &viewforward, &viewright, &viewup);
		qAimAngles.Init(0.0f, pCmd->viewangles.y, 0.0f);
		AngleVectors(qAimAngles, &aimforward, &aimright, &aimup);
		Vector vForwardNorm;		Normalize(viewforward, vForwardNorm);
		Vector vRightNorm;			Normalize(viewright, vRightNorm);
		Vector vUpNorm;				Normalize(viewup, vUpNorm);

		// Original shit for movement correction
		float forward = pCmd->forwardmove;
		float right = pCmd->sidemove;
		float up = pCmd->upmove;
		if (forward > 450) forward = 450;
		if (right > 450) right = 450;
		if (up > 450) up = 450;
		if (forward < -450) forward = -450;
		if (right < -450) right = -450;
		if (up < -450) up = -450;
		pCmd->forwardmove = DotProduct(forward * vForwardNorm, aimforward) + DotProduct(right * vRightNorm, aimforward) + DotProduct(up * vUpNorm, aimforward);
		pCmd->sidemove = DotProduct(forward * vForwardNorm, aimright) + DotProduct(right * vRightNorm, aimright) + DotProduct(up * vUpNorm, aimright);
		pCmd->upmove = DotProduct(forward * vForwardNorm, aimup) + DotProduct(right * vRightNorm, aimup) + DotProduct(up * vUpNorm, aimup);

		// Angle normalisation
		if (Menu::Window.MiscTab.OtherSafeMode.GetState())
		{
			GameUtils::NormaliseViewAngle(pCmd->viewangles);

			if (pCmd->viewangles.z != 0.0f)
			{
				pCmd->viewangles.z = 0.00;
			}

			if (pCmd->viewangles.x < -89 || pCmd->viewangles.x > 89 || pCmd->viewangles.y < -180 || pCmd->viewangles.y > 180)
			{
				Utilities::Log("Having to re-normalise!");
				GameUtils::NormaliseViewAngle(pCmd->viewangles);
				Beep(750, 800); // Why does it do this
				if (pCmd->viewangles.x < -89 || pCmd->viewangles.x > 89 || pCmd->viewangles.y < -180 || pCmd->viewangles.y > 180)
				{
					pCmd->viewangles = origView;
					pCmd->sidemove = right;
					pCmd->forwardmove = forward;
				}
			}
		}

		if (pCmd->viewangles.x > 90)
		{
			pCmd->forwardmove = -pCmd->forwardmove;
		}

		if (pCmd->viewangles.x < -90)
		{
			pCmd->forwardmove = -pCmd->forwardmove;
		}

		switch (Menu::Window.MiscTab.OtherThirdpersonType.GetIndex())
		{
		case 0:
			if (!bSendPacket)
				LastAngleAA = pCmd->viewangles;
			break;
		case 1:
			if (bSendPacket)
				LastAngleAA = pCmd->viewangles;
			break;
		}
		//if (!bSendPacket)
		//LastAngleAA = pCmd->viewangles;

		if (bSendPacket == true) {
			lineFakeAngle = pCmd->viewangles.y;
		}
		else {
			lineRealAngle = pCmd->viewangles.y;
		}

	}

	return false;
}

std::string GetTimeString()
{
	time_t current_time;
	struct tm *time_info;
	static char timeString[10];
	time(&current_time);
	time_info = localtime(&current_time);
	strftime(timeString, sizeof(timeString), "%X", time_info);
	return timeString;
}

extern int hitmarkertime;
// Paint Traverse Hooked function
void __fastcall PaintTraverse_Hooked(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	int angles = Menu::Window.VisualsTab.angleindicator.GetIndex();
	if (Menu::Window.VisualsTab.RemoveScope.GetState() && !strcmp("HudZoom", Interfaces::Panels->GetName(vguiPanel)))
		return;
	int w, h;
	int centerW, centerh, topH;
	Interfaces::Engine->GetScreenSize(w, h);
	centerW = w / 2;
	centerh = h / 2;
	oPaintTraverse(pPanels, vguiPanel, forceRepaint, allowForce);

	static unsigned int FocusOverlayPanel = 0;
	static bool FoundPanel = false;



	if (!FoundPanel)
	{
		PCHAR szPanelName = (PCHAR)Interfaces::Panels->GetName(vguiPanel);
		if (strstr(szPanelName, "MatSystemTopPanel"))
		{
			FocusOverlayPanel = vguiPanel;
			FoundPanel = true;
		}
	}
	else if (FocusOverlayPanel == vguiPanel)
	{

		static float rainbow;
		rainbow += 0.005f;
		if (rainbow > 1.f) rainbow = 0.f;
		//Render::GradientV(8, 8, 160, 18, Color(0, 0, 0, 0), Color(0, 0, 0, 255));
		//Render::Text(10, 10, Color::FromHSB(rainbow, 1.f, 1.f), Render::Fonts::WaterMark, "plaSticware");
		IClientEntity* pLocal = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
		CUserCmd* cmdlist = *(CUserCmd**)((DWORD)Interfaces::pInput + 0xEC);
		RECT scrn = Render::GetViewport();
		if (Menu::Window.VisualsTab.Watermark.GetState() == 1)
		{
			Render::Text(10, 10, Color(255, 255, 255, 255), Render::Fonts::ESP, "hoodhokage");
			Render::Textf(10, 23, Color(255, 255, 255, 255), Render::Fonts::ESP, ("%s"), GetTimeString().c_str());
		}
		//Render::Text(10, 40, Color(255, 255, 255, 255), Render::Fonts::ESP, "Engine Prediction: Off");
		//switch (Menu::Window.RageBotTab.AccuracyDrop.GetIndex())
		//{
		//case 0: Render::Text(10, 55, Color(255, 255, 255, 255), Render::Fonts::ESP, "Accuracy: Hit-Chance"); break;
		//case 1: Render::Text(10, 55, Color(255, 255, 255, 255), Render::Fonts::ESP, "Accuracy: Spread Limit"); break;
		//}

		if (angles == 0)
		{

		}
		else if (angles == 1)
		{
			if (bigboi::indicator == 1)
			{
				Render::Text((centerW + 70), (centerh - 15), Color(255, 255, 255, 240), Render::Fonts::LBY3, "Q");
			}
			else if (bigboi::indicator == 2)
			{
				Render::Text((centerW - 80), (centerh - 15), Color(255, 255, 255, 240), Render::Fonts::LBY3, "R");
			}
			else if (bigboi::indicator == 3)
			{
				Render::Text((centerW - 10), (centerh + 70), Color(255, 255, 255, 240), Render::Fonts::LBY3, "T");
			}
			else if (bigboi::indicator == 4)
			{
				Render::Text((centerW - 8), (centerh + 70), Color(240, 0, 20, 140), Render::Fonts::LBY3, "T");
				Render::Text((centerW - 200), (centerh - 60), Color(240, 0, 20, 140), Render::Fonts::ESP, "Anti Aims Are Enabled, but fakelag is off");

			}
		}

		if (hitmarkertime > 0)
		{

			RECT View = Render::GetViewport();
			int x = View.right / 2;
			int y = View.bottom / 2;
			int length_x = 5;
			int length_y = 5;
			float alpha = hitmarkertime;
			IGameEvent* pEvent;

			Render::Line(x + 5, y - 5, x + 5 + length_x, y - 5 - length_y, Color(255, 255, 255, alpha));

			//bottom right
			Render::Line(x + 5, y + 5, x + 5 + length_x, y + 5 + length_y, Color(255, 255, 255, alpha));

			//top left
			Render::Line(x - 5, y - 5, x - 5 - length_x, y - 5 - length_y, Color(255, 255, 255, alpha));

			//bottom left
			Render::Line(x - 5, y + 5, x - 5 - length_x, y + 5 + length_y, Color(255, 255, 255, alpha));

			hitmarkertime -= 2;
		}
		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())
			Hacks::DrawHacks();

		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && Menu::Window.VisualsTab.DamageIndicator.GetState())
			damage_indicators.paint();

		// Update and draw the menu
		Menu::DoUIFrame();
	}
}


// InPrediction Hooked Function
bool __stdcall Hooked_InPrediction()
{

	bool result;
	static InPrediction_ origFunc = (InPrediction_)Hooks::VMTPrediction.GetOriginalFunction(14);
	static DWORD *ecxVal = Interfaces::Prediction;
	result = origFunc(ecxVal);

	// If we are in the right place where the player view is calculated
	// Calculate the change in the view and get rid of it
	if (Menu::Window.VisualsTab.OtherNoVisualRecoil.GetState() && (DWORD)(_ReturnAddress()) == Offsets::Functions::dwCalcPlayerView)
	{
		IClientEntity* pLocalEntity = NULL;

		float* m_LocalViewAngles = NULL;

		__asm
		{
			MOV pLocalEntity, ESI
			MOV m_LocalViewAngles, EBX
		}

		Vector viewPunch = pLocalEntity->localPlayerExclusive()->GetViewPunchAngle();
		Vector aimPunch = pLocalEntity->localPlayerExclusive()->GetAimPunchAngle();

		m_LocalViewAngles[0] -= (viewPunch[0] + (aimPunch[0] * 2 * 0.4499999f));
		m_LocalViewAngles[1] -= (viewPunch[1] + (aimPunch[1] * 2 * 0.4499999f));
		m_LocalViewAngles[2] -= (viewPunch[2] + (aimPunch[2] * 2 * 0.4499999f));
		return true;
	}

	return result;
}

#define TEXTURE_GROUP_OTHER							"Other textures"
void __fastcall Hooked_DrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld)
{
	Color color;
	float flColor[3] = { 0.f };
	static IMaterial* CoveredLit = CreateMaterial(true);
	static IMaterial* OpenLit = CreateMaterial(false);
	static IMaterial* CoveredFlat = CreateMaterial(true, false);
	static IMaterial* OpenFlat = CreateMaterial(false, false);
	static IMaterial* Chrome = CreateMaterial("$envmap env_cube");
	bool DontDraw = false;

	if (Menu::Window.VisualsTab.Active.GetState())
	{
		const char* ModelName = Interfaces::ModelInfo->GetModelName((model_t*)pInfo.pModel);
		IClientEntity* pModelEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(pInfo.entity_index);
		IClientEntity* pLocal = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

		int ChamsStyle = Menu::Window.VisualsTab.OptionsChams.GetIndex();
		int HandsStyle = Menu::Window.VisualsTab.OtherNoHands.GetIndex();

		if (pModelEntity == pLocal)
		{
			if (pLocal->IsScoped())
			{
				Interfaces::RenderView->SetBlend(0.5);
			}
			else
			{
				Interfaces::RenderView->SetBlend(1);
			}
		}

		int TChamsR = Menu::Window.VisualsTab.TChamsR.GetValue();
		int TChamsG = Menu::Window.VisualsTab.TChamsG.GetValue();
		int TChamsB = Menu::Window.VisualsTab.TChamsB.GetValue();

		int CTChamsR = Menu::Window.VisualsTab.CTChamsR.GetValue();
		int CTChamsG = Menu::Window.VisualsTab.CTChamsG.GetValue();
		int CTChamsB = Menu::Window.VisualsTab.CTChamsB.GetValue();

		int TChamsRVis = Menu::Window.VisualsTab.TChamsRVis.GetValue();
		int TChamsGVis = Menu::Window.VisualsTab.TChamsGVis.GetValue();
		int TChamsBVis = Menu::Window.VisualsTab.TChamsBVis.GetValue();

		int CTChamsRVis = Menu::Window.VisualsTab.CTChamsRVis.GetValue();
		int CTChamsGVis = Menu::Window.VisualsTab.CTChamsGVis.GetValue();
		int CTChamsBVis = Menu::Window.VisualsTab.CTChamsBVis.GetValue();

		if (strstr(ModelName, "models/player"))
		{
			if (pLocal && pModelEntity && ChamsStyle != 0)
			{
				IMaterial *skeletonz = Interfaces::MaterialSystem->FindMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass", TEXTURE_GROUP_OTHER);
				IMaterial *material1 = Interfaces::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", TEXTURE_GROUP_OTHER);

				if ((Menu::Window.VisualsTab.Localplayerz.GetState() || pModelEntity->GetTeamNum() != pLocal->GetTeamNum()))
				{
					IMaterial *covered = ChamsStyle == 1 ? CoveredLit : CoveredFlat;
					IMaterial *open = ChamsStyle == 1 ? OpenLit : OpenFlat;

					if (pModelEntity->IsAlive() && pModelEntity->GetHealth() > 0 /*&& pModelEntity->GetTeamNum() != local->GetTeamNum()*/)
					{
						float alpha = 1.f;

						if (pModelEntity->HasGunGameImmunity())
							alpha = 0.5f;

						if (pModelEntity->GetTeamNum() != pLocal->GetTeamNum())
						{
							flColor[0] = CTChamsR / 255.f;
							flColor[1] = CTChamsG / 255.f;
							flColor[2] = CTChamsB / 255.f;
						}
						else
						{
							flColor[0] = CTChamsRVis / 255.f;
							flColor[1] = CTChamsGVis / 255.f;
							flColor[2] = CTChamsBVis / 255.f;
						}
						if (Menu::Window.VisualsTab.OptionsChams.GetIndex() == 1 || Menu::Window.VisualsTab.OptionsChams.GetIndex() == 2 && !pLocal->IsScoped())
						{
							Interfaces::RenderView->SetColorModulation(flColor);
							Interfaces::RenderView->SetBlend(alpha);
							Interfaces::ModelRender->ForcedMaterialOverride(covered);
							oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
						}

						if (pModelEntity->GetTeamNum() == pLocal->GetTeamNum())
						{
							flColor[0] = TChamsR / 255.f;
							flColor[1] = TChamsG / 255.f;
							flColor[2] = TChamsB / 255.f;
						}
						else
						{
							flColor[0] = TChamsRVis / 255.f;
							flColor[1] = TChamsGVis / 255.f;
							flColor[2] = TChamsBVis / 255.f;
						}
						if (Menu::Window.VisualsTab.OptionsChams.GetIndex() == 3)
						{
							Interfaces::RenderView->SetColorModulation(flColor);
							Interfaces::RenderView->SetBlend(alpha);
							Interfaces::ModelRender->ForcedMaterialOverride(OpenLit);
						}
						else {
							Interfaces::RenderView->SetColorModulation(flColor);
							Interfaces::RenderView->SetBlend(alpha);
							Interfaces::ModelRender->ForcedMaterialOverride(open);
						}
					}
					else
					{
						color.SetColor(255, 255, 255, 255);
						ForceMaterial(color, open);
					}
				}
			}
		}
		else if (strstr(ModelName, "arms"))
		{
			if (HandsStyle != 0 && pLocal && pLocal->IsAlive())
			{
				if (HandsStyle == 1)
				{
					DontDraw = true;
				}
				else if (HandsStyle == 2)
				{
					IMaterial *covered = ChamsStyle == 1 ? CoveredLit : CoveredFlat;
					IMaterial *open = ChamsStyle == 1 ? OpenLit : OpenFlat;
					if (pLocal)
					{
						if (pLocal->IsAlive())
						{
							int alpha = pLocal->HasGunGameImmunity() ? 150 : 255;

							if (pLocal->GetTeamNum() == 2)
								color.SetColor(185, 255, 70, alpha);
							else
								color.SetColor(185, 255, 70, alpha);

							ForceMaterial(color, covered);
							oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

							if (pLocal->GetTeamNum() == 2)
								color.SetColor(185, 255, 70, alpha);
							else
								color.SetColor(185, 255, 70, alpha);
						}
						else
						{
							color.SetColor(185, 255, 70, 255);
						}

						ForceMaterial(color, covered);
					}
				}
				else
				{
					static int counter = 0;
					static float colors[3] = { 1.f, 0.f, 0.f };

					if (colors[counter] >= 1.0f)
					{
						colors[counter] = 1.0f;
						counter += 1;
						if (counter > 2)
							counter = 0;
					}
					else
					{
						int prev = counter - 1;
						if (prev < 0) prev = 2;
						colors[prev] -= 0.05f;
						colors[counter] += 0.05f;
					}

					Interfaces::RenderView->SetColorModulation(colors);
					Interfaces::RenderView->SetBlend(0.3);
					Interfaces::ModelRender->ForcedMaterialOverride(CoveredLit);
				}
			}
		}
	}

	if (!DontDraw)
		oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
	Interfaces::ModelRender->ForcedMaterialOverride(NULL);
}


void NormalizedAngle(float& flAngle)
{
	if (std::isnan(flAngle)) flAngle = 0.0f;
	if (std::isinf(flAngle)) flAngle = 0.0f;

	float flRevolutions = flAngle / 360;

	if (flAngle > 180 || flAngle < -180)
	{
		if (flRevolutions < 0)
			flRevolutions = -flRevolutions;

		flRevolutions = round(flRevolutions);

		if (flAngle < 0)
			flAngle = (flAngle + 360 * flRevolutions);
		else
			flAngle = (flAngle - 360 * flRevolutions);
	}
}

static inline bool IsNearEqual(float v1, float v2, float Tolerance)
{
	return std::abs(v1 - v2) <= std::abs(Tolerance);
}

std::vector<const char*> vistasmoke_mats =
{
	"particle/vistasmokev1/vistasmokev1_fire",
	"particle/vistasmokev1/vistasmokev1_smokegrenade",
	"particle/vistasmokev1/vistasmokev1_emods",
	"particle/vistasmokev1/vistasmokev1_emods_impactdust",
};

// Hooked FrameStageNotify for removing visual recoil
void  __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage)
{
	DWORD eyeangles = NetVar.GetNetVar(0xBFEA4E7B);
	IClientEntity* pEntity = nullptr;


	static auto linegoesthrusmoke = Utilities::Memory::FindPattern("client_panorama.dll", (PBYTE)"\x55\x8B\xEC\x83\xEC\x08\x8B\x15\x00\x00\x00\x00\x0F\x57\xC0", "xxxxxxxx????xxx");
	static auto smokecout = *(DWORD*)(linegoesthrusmoke + 0x8);

	if (Menu::Window.VisualsTab.OtherNoSmoke.GetState())
	{
		std::vector<const char*> vistasmoke_mats =
		{
			"particle/vistasmokev1/vistasmokev1_fire",
			"particle/vistasmokev1/vistasmokev1_smokegrenade",
			"particle/vistasmokev1/vistasmokev1_emods",
			"particle/vistasmokev1/vistasmokev1_emods_impactdust",
		};

		for (auto matName : vistasmoke_mats) {
			IMaterial* mat = Interfaces::MaterialSystem->FindMaterial(matName, "Other textures");
			mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
		}


		if (Menu::Window.VisualsTab.OtherNoSmoke.GetState()) {
			*(int*)(smokecout) = 0;
		}
	}
	IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && curStage == FRAME_RENDER_START)
	{

		if (*(bool*)((DWORD)Interfaces::pInput + 0xA5))
			*(Vector*)((DWORD)pLocal + 0x31C8) = LastAngleAA;

		int thirdpersonKey = Menu::Window.MiscTab.OtherThirdpersonKey.GetKey();
		if (thirdpersonKey > 0 && GUI.GetKeyPress(thirdpersonKey))
		{
			thirdpersonBool = !thirdpersonBool;
		}

		if (thirdpersonBool)
		{
			static bool rekt = false;
			if (!rekt)
			{
				ConVar* sv_cheats = Interfaces::CVar->FindVar("sv_cheats");
				SpoofedConvar* sv_cheats_spoofed = new SpoofedConvar(sv_cheats);
				sv_cheats_spoofed->SetInt(1);
				rekt = true;
			}
		}
		static bool rekt1 = false;
		if (thirdpersonBool && pLocal->IsAlive() && pLocal->IsScoped() == 0)
		{
			if (!rekt1)
			{
				Interfaces::Engine->ClientCmd_Unrestricted("thirdperson");
				rekt1 = true;
			}
		}
		else if (!thirdpersonBool)
		{
			rekt1 = false;
		}
		static bool rekt = false;
		if (!thirdpersonBool || pLocal->IsAlive() == 0)
		{
			if (!rekt)
			{
				Interfaces::Engine->ClientCmd_Unrestricted("firstperson");
				rekt = true;
			}
		}
		else if (thirdpersonBool || pLocal->IsAlive() || pLocal->IsScoped() == 0)
		{
			rekt = false;
		}

		static bool meme = false;
		if (thirdpersonBool && pLocal->IsScoped() == 0)
		{
			if (!meme)
			{
				Interfaces::Engine->ClientCmd_Unrestricted("thirdperson");
				meme = true;
			}
		}
		else if (pLocal->IsScoped())
		{
			meme = false;
		}

		static bool kek = false;
		if (thirdpersonBool && pLocal->IsAlive())
		{
			if (!kek)
			{
				Interfaces::Engine->ClientCmd_Unrestricted("thirdperson");
				kek = true;
			}
		}
		else if (pLocal->IsAlive() == 0)
		{
			Interfaces::Engine->ClientCmd_Unrestricted("firstperson");
			rekt = true;
		}
	}
	CCacheAngle OptionsManager;


	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
		{

			for (int i = 1; i < 65; i++)
			{
				if (pLocal->IsAlive())
				{
					IClientEntity* pEnt = Interfaces::EntList->GetClientEntity(i);
					if (!pEnt) continue;
					if (pEnt->IsDormant()) continue;
					if (pEnt->GetHealth() < 1) continue;
					if (pEnt->GetLifeState() != 0) continue;

					static bool isMoving;
					float PlayerIsMoving = abs(pEnt->GetVelocity().Length());
					if (PlayerIsMoving > 36) isMoving = true;
					else if (PlayerIsMoving <= 36) isMoving = false;
					animstyte();
					bool MeetsLBYReq;
					if (pEnt->GetFlags() & FL_ONGROUND)
						MeetsLBYReq = true;
					else
						MeetsLBYReq = false;

					float newsimtime;
					float storedsimtime;
					bool lbyupdated;
					float storedlbyFGE;
					float storedanglesFGE;
					float storedsimtimeFGE;
					bool didhitHS;
					float newlby;
					float newdelta;
					float newlbydelta;
					float finaldelta;
					float finallbydelta;
					float storedlby[64];
					float storeddelta[64];
					float storedlbydelta[64];
					float StoredAngles[64];
					float StoredLBY[65];

					newlby = pEnt->GetLowerBodyYaw();
					newsimtime = pEnt->GetSimulationTime();
					newdelta = *(float*)((DWORD)pEnt + eyeangles);
					newlbydelta = pEnt->GetLowerBodyYaw();
					finaldelta = newdelta - storeddelta[pEnt->GetIndex()];
					finallbydelta = newlbydelta - storedlbydelta[pEnt->GetIndex()];
					StoredAngles[pEnt->GetIndex()] = *(float*)((DWORD)pEnt + eyeangles);
					storedlby[pEnt->GetIndex()] = pEnt->GetLowerBodyYaw();
					storedlbydelta[pEnt->GetIndex()] = pEnt->GetLowerBodyYaw();
					storedsimtime = pEnt->GetSimulationTime();
					storeddelta[pEnt->GetIndex()] = *(float*)((DWORD)pEnt + eyeangles);
					storedanglesFGE = *(float*)((DWORD)pEnt + eyeangles);
					storedlbyFGE = pEnt->GetLowerBodyYaw();
					storedsimtimeFGE = pEnt->GetSimulationTime();

					if (newlby == *storedlby)
						lbyupdated = false;
					else
						lbyupdated = true;


					if (Menu::Window.RageBotTab.AccuracyResolverenable.GetState()) {
						static float time_at_update[65];
						int index = pEnt->GetIndex();
						float simTime_since_lby_update = pEnt->GetSimulationTime() - time_at_update[index];

						Vector reset = Vector(0, 0, 0); reset.y = pEnt->GetEyeAnglesPointer()->y;
						static float LatestLowerBodyYawUpdateTime[55];
						float lby = pEnt->GetLowerBodyYaw();
						static bool bLowerBodyIsUpdated;
						if (pEnt->GetLowerBodyYaw() != StoredLBY[pEnt->GetIndex()]) bLowerBodyIsUpdated = true;
						else bLowerBodyIsUpdated = false;
						float bodyeyedelta = *(float*)((DWORD)pEnt + eyeangles) - pEnt->GetLowerBodyYaw();
						float curTime_since_lby_update = LatestLowerBodyYawUpdateTime[pEnt->GetIndex()] - Interfaces::Globals->curtime;
						float LastUpdatedLBY = 0.f;
						bool bClientMoving = false;
						if (pEnt->GetVelocity().Length2D() > 0.1f) bClientMoving = true;
						else bClientMoving = false;
						float flLastUpdateTime = LatestLowerBodyYawUpdateTime[pEnt->GetIndex()] - Interfaces::Globals->curtime;

						if (bLowerBodyIsUpdated || isMoving || fabsf(bodyeyedelta) >= 35.0f) // i have gay
						{
							*(float*)((DWORD)pEnt + eyeangles) = pEnt->GetLowerBodyYaw();
							LatestLowerBodyYawUpdateTime[pEnt->GetIndex()] = Interfaces::Globals->curtime;
							StoredLBY[pEnt->GetIndex()] = pEnt->GetLowerBodyYaw();
						}
						else if (Globals::missedshots >= 2) // dont know why i didn't put this in the statement below
						{
							*(float*)((DWORD)pEnt + eyeangles) = SETANGLE;
						}
						else
						{
							switch (Globals::Shots % 2) // smart attarget bruteforce that will hit (almost) any static angle after just 3 shots (thanks bolbi)
							{
							case 0: *(float*)((DWORD)pEnt + eyeangles) = Bolbilize(CalcAngle69(pEnt->GetVecOrigin(), pLocal->GetVecOrigin()).y + SETANGLE); break;
							case 1: *(float*)((DWORD)pEnt + eyeangles) = Bolbilize(CalcAngle69(pEnt->GetVecOrigin(), pLocal->GetVecOrigin()).y + 180); break;
							}
						}
					}
				}
			}
		}
	}
#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin)

	//if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	//{
	//	newskinchanger();
	//}
	oFrameStageNotify(curStage);
}

bool __fastcall Hooked_FireEventClientSide(PVOID ECX, PVOID EDX, IGameEvent *Event)
{
	//cbullet_tracer* tracer;
	//tracer->log(Event);
	if (Menu::Window.VisualsTab.DamageIndicator.GetState())
	{
		if (strcmp(Event->GetName(), "player_hurt") == 0)
		{
			IClientEntity* pLocalPlayer = hackManager.pLocal();
			IClientEntity* hurt = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetPlayerForUserID(Event->GetInt("userid")));
			IClientEntity* attacker = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetPlayerForUserID(Event->GetInt("attacker")));

			if (hurt != pLocalPlayer && attacker == pLocalPlayer)
			{
				DamageIndicator_t DmgIndicator;
				DmgIndicator.iDamage = Event->GetInt("dmg_health");
				DmgIndicator.Player = hurt;
				DmgIndicator.flEraseTime = pLocalPlayer->GetTickBase() * Interfaces::Globals->interval_per_tick + 3.f;
				DmgIndicator.bInitialized = false;
				damage_indicators.data.push_back(DmgIndicator);
			}
		}
	}
	if (Menu::Window.RageBotTab.AccuracyResolverenable.GetState())
	{
		if (!strcmp(Event->GetName(), "player_hurt"))
		{
			int deadfag = Event->GetInt("userid");
			int attackingfag = Event->GetInt("attacker");
			IClientEntity* pLocal = hackManager.pLocal();
			if (Interfaces::Engine->GetPlayerForUserID(deadfag) != Interfaces::Engine->GetLocalPlayer() && Interfaces::Engine->GetPlayerForUserID(attackingfag) == Interfaces::Engine->GetLocalPlayer())
			{
				IClientEntity* hittedplayer = (IClientEntity*)(Interfaces::Engine->GetPlayerForUserID(deadfag));
				int hit = Event->GetInt("hitgroup");
				if (hit == 1 && hittedplayer && deadfag && attackingfag)
				{
					Resolver::didhitHS = true;
					Globals::missedshots = 0;
				}
				else
				{
					Resolver::didhitHS = false;
					Globals::missedshots++;
				}
			}
		}
	}
	return oFireEventClientSide(ECX, Event);
}
int currentfov;
void __fastcall Hooked_OverrideView(void* ecx, void* edx, CViewSetup* pSetup)
{

	IClientEntity* localplayer1 = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (!localplayer1)
		return;

	if (!Interfaces::Engine->IsConnected())
		return;
	if (!Interfaces::Engine->IsInGame())
		return;

//	grenade_prediction::instance().View(pSetup);

	if (localplayer1)
	{
		pSetup->fov += Menu::Window.VisualsTab.OtherFOV.GetValue();
	}
	currentfov = pSetup->fov;
	oOverrideView(ecx, edx, pSetup);
}

void GetViewModelFOV(float& fov)
{
	IClientEntity* localplayer = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())
	{

		if (!localplayer)
			return;


		if (Menu::Window.VisualsTab.Active.GetState())
			fov += Menu::Window.VisualsTab.OtherViewmodelFOV.GetValue();
	}
}

float __stdcall GGetViewModelFOV()
{
	float fov = Hooks::VMTClientMode.GetMethod<oGetViewModelFOV>(35)();

	GetViewModelFOV(fov);

	return fov;
}

void __fastcall  hkSceneEnd(void *pEcx, void *pEdx) {

	if (Menu::Window.VisualsTab.FakeAngleChams.GetState())
	{
		IClientEntity* pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
		if (pLocal)
		{
			static  IMaterial* CoveredLit = CreateMaterial(true);
			if (CoveredLit)
			{
				Vector OrigAng;
				OrigAng = pLocal->GetEyeAngles();
				pLocal->SetAngle2(Vector(0, lineFakeAngle, 0));

				bool LbyColor = false;
				float	NormalColor[3] = { 0.75, 0.75 , 0.75 };
				float lbyUpdateColor[3] = { 0, 1, 0 };
				Interfaces::RenderView->SetColorModulation(LbyColor ? lbyUpdateColor : NormalColor);
				Interfaces::ModelRender->ForcedMaterialOverride(CoveredLit);
				pLocal->draw_model(STUDIO_RENDER, 200);
				Interfaces::ModelRender->ForcedMaterialOverride(nullptr);
				pLocal->SetAngle2(OrigAng);
			}
		}
	}
	pSceneEnd(pEcx);
}

namespace resolvokek
{
	int Shots;
	int missedshots;
	float RealAngle;
	float FakeAngle;
	Vector AimPoint;
	bool shouldflip;
	bool ySwitch;
	float NextTime;
	int reso = 1;
	float fakeAngle;
	float OldSimulationTime[65];
	bool error;
}


void __fastcall Hooked_RenderView(void* ecx, void* edx, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw)
{
	static DWORD oRenderView = Hooks::VMTRenderView.GetOriginalFunction(6);

	IClientEntity* pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	__asm
	{
		PUSH whatToDraw
		PUSH nClearFlags
		PUSH hudViewSetup
		PUSH setup
		MOV ECX, ecx
		CALL oRenderView
	}
} //hooked for no reason yay