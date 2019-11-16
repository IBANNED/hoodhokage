/*
- hoodhokage by ibanned -
*/

#include "LegitBot.h"
#include "RenderManager.h"
#include <iostream>

void CLegitBot::LegitAA(CUserCmd *pCmd, bool& bSendPacket)
{
	IClientEntity* pLocal = hackManager.pLocal();

	if ((pCmd->buttons & IN_USE) || pLocal->GetMoveType() == MOVETYPE_LADDER || pCmd->buttons & IN_ATTACK || pCmd->buttons & IN_ATTACK2)
		return;
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
	if (pWeapon)
	{
		CSWeaponInfo* pWeaponInfo = pWeapon->GetCSWpnData();
		// Knives or grenades
		CCSGrenade* csGrenade = (CCSGrenade*)pWeapon;

		if (GameUtils::IsBallisticWeapon(pWeapon))
		{
			if (csGrenade->GetThrowTime() > 0.f)
				return;
		}
	}
	//for the memes
	Vector oldAngle = pCmd->viewangles;
	float oldForward = pCmd->forwardmove;
	float oldSideMove = pCmd->sidemove;
	if (Menu::Window.LegitBotTab.LegitChoice.GetIndex() > 0)
	{
		switch (Menu::Window.LegitBotTab.LegitChoice.GetIndex())
		{
		case 1:
		{//you dont need brackets but for some shit you do like statics //wrapzii C++ class 101
			static bool kFlip = true;
			static int ChokedPackets = -1;
			static bool yFlip = true;
			if (1 > ChokedPackets)
			{
				bSendPacket = true;
				ChokedPackets++;
			}
			else
			{
				bSendPacket = false;
				//pCmd->viewangles.y += yFlip ? 90.f : -90.f;
				pCmd->viewangles.y += 180.f;
				ChokedPackets = -1;
			}
		}
		break;
		case 2:
		{
			static bool kFlip = true;
			static int ChokedPackets = -1;
			static bool yFlip = true;
			if (1 > ChokedPackets)
			{
				bSendPacket = true;
				ChokedPackets++;
			}
			else
			{
				bSendPacket = false;
				//pCmd->viewangles.y += yFlip ? 90.f : -90.f;
				pCmd->viewangles.y += 90.f;//to the right im pretty sure
				ChokedPackets = -1;
			}
		}
		break;
		case 3://put my p ass vel check one
			   //Sideways-switch
		{
			static int ChokedPackets = -1;//we choking 2 cuz 1 is too main stream
			if (1> ChokedPackets) {
				bSendPacket = false;
				static bool dir = false;
				static bool dir2 = false;
				int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); ++i;
				IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
				//if (pCmd->forwardmove > 1 || (IsVisible(pLocal, pEntity, 0) && pEntity->GetTeamNum() != pLocal->GetTeamNum()))// was trying to make a vis check to make it -180 if their visible //didnt seem to work
				//dir2 = true;
				//else {
				dir2 = false;
				if (pCmd->sidemove > 1) dir = true;
				else if (pCmd->sidemove < -1) dir = false;
				pCmd->viewangles.y = (dir) ? (pCmd->viewangles.y - 180) - 270.f : (pCmd->viewangles.y - 180) - 90.f;
				//}
				//if (dir2 = true)
				//pCmd->viewangles.y = pCmd->viewangles.y - 180;
				ChokedPackets++;
			}
			else
			{
				bSendPacket = true;
				ChokedPackets = -1;

			}
		}
		break;
		}

	}
	else
		pCmd->viewangles.y += 0;
}


void CLegitBot::Init()
{
	IsLocked = false;
	TargetID = -1;
}

void CLegitBot::Draw()
{

}

void CLegitBot::Move(CUserCmd *pCmd, bool &bSendPacket)
{
	// Master switch
	if (!Menu::Window.LegitBotTab.AimbotEnable.GetState())
		return;

	// Aimbot
	if (Menu::Window.LegitBotTab.AimbotEnable.GetState())
		DoAimbot(pCmd, bSendPacket);

	IClientEntity* pLocal = hackManager.pLocal();
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	if (pLocal->IsAlive())
	{
		LegitAA(pCmd, bSendPacket);
	}

	// Triggerbot
	if (Menu::Window.LegitBotTab.TriggerEnable.GetState() && (!Menu::Window.LegitBotTab.TriggerKeyPress.GetState() || GUI.GetKeyState(Menu::Window.LegitBotTab.TriggerKeyBind.GetKey())))
		DoTrigger(pCmd);
}

void CLegitBot::SyncWeaponSettings()
{
	std::vector<int> HitBoxesToScan;
	IClientEntity* pLocal = hackManager.pLocal();
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	if (!pWeapon)
		return;

	if (GameUtils::IsPistol2(pWeapon))
	{
		Speed = Menu::Window.LegitBotTab.WeaponPistSpeed.GetValue() / 100;
		FoV = Menu::Window.LegitBotTab.WeaponPistFoV.GetValue();
		RecoilControl = Menu::Window.LegitBotTab.WeaponPistRecoil.GetState();
		PSilent = Menu::Window.LegitBotTab.WeaponPistPSilent.GetState();
		Inacc = Menu::Window.LegitBotTab.WeaponPistInacc.GetValue();

		switch (Menu::Window.LegitBotTab.WeaponPistHitbox.GetIndex())
		{
		case 0:
			HitBox = ((int)CSGOHitboxID2::Head);
			Multihitbox = false;
			break;
		case 1:
			HitBox = ((int)CSGOHitboxID2::Neck);
			Multihitbox = false;
			break;
		case 2:
			HitBox = ((int)CSGOHitboxID2::Chest);
			Multihitbox = false;
			break;
		case 3:
			HitBox = ((int)CSGOHitboxID2::Stomach);
			Multihitbox = false;
			break;
		}
	}
	else if (GameUtils::SCAR20ZZ(pWeapon))
	{
		Speed = Menu::Window.LegitBotTab.WeaponSnipSpeed.GetValue() / 100;
		FoV = Menu::Window.LegitBotTab.WeaponSnipFoV.GetValue();
		RecoilControl = Menu::Window.LegitBotTab.WeaponSnipRecoil.GetState();
		PSilent = Menu::Window.LegitBotTab.WeaponSnipPSilent.GetState();
		Inacc = Menu::Window.LegitBotTab.WeaponSnipInacc.GetValue();

		switch (Menu::Window.LegitBotTab.WeaponSnipHitbox.GetIndex())
		{
		case 0:
			HitBox = ((int)CSGOHitboxID2::Head);
			Multihitbox = false;
			break;
		case 1:
			HitBox = ((int)CSGOHitboxID2::Neck);
			Multihitbox = false;
			break;
		case 2:
			HitBox = ((int)CSGOHitboxID2::Chest);
			Multihitbox = false;
			break;
		case 3:
			HitBox = ((int)CSGOHitboxID2::Stomach);
			Multihitbox = false;
			break;
		}
	}
	if (GameUtils::IsRifle(pWeapon))
	{
		Speed = Menu::Window.LegitBotTab.WeaponMainSpeed.GetValue() / 100;
		FoV = Menu::Window.LegitBotTab.WeaponMainFoV.GetValue();
		RecoilControl = Menu::Window.LegitBotTab.WeaponMainRecoil.GetState();
		PSilent = Menu::Window.LegitBotTab.WeaponMainPSilent.GetState();
		Inacc = Menu::Window.LegitBotTab.WeaponMainInacc.GetValue();

		switch (Menu::Window.LegitBotTab.WeaponMainHitbox.GetIndex())
		{
		case 0:
			HitBox = ((int)CSGOHitboxID2::Head);
			Multihitbox = false;
			break;
		case 1:
			HitBox = ((int)CSGOHitboxID2::Neck);
			Multihitbox = false;
			break;
		case 2:
			HitBox = ((int)CSGOHitboxID2::Chest);
			Multihitbox = false;
			break;
		case 3:
			HitBox = ((int)CSGOHitboxID2::Stomach);
			Multihitbox = false;
			break;
		}
	}
	if (GameUtils::M4A4S(pWeapon))
	{
		Speed = Menu::Window.LegitBotTab.WeaponMainSpeedM4.GetValue() / 100;
		FoV = Menu::Window.LegitBotTab.WeaponMainFoVM4.GetValue();
		RecoilControl = Menu::Window.LegitBotTab.WeaponMainRecoilM4.GetState();
		PSilent = Menu::Window.LegitBotTab.WeaponMainPSilentM4.GetState();
		Inacc = Menu::Window.LegitBotTab.WeaponMainInaccM4.GetValue();

		switch (Menu::Window.LegitBotTab.WeaponMainHitboxM4.GetIndex())
		{
		case 0:
			HitBox = ((int)CSGOHitboxID2::Head);
			Multihitbox = false;
			break;
		case 1:
			HitBox = ((int)CSGOHitboxID2::Neck);
			Multihitbox = false;
			break;
		case 2:
			HitBox = ((int)CSGOHitboxID2::Chest);
			Multihitbox = false;
			break;
		case 3:
			HitBox = ((int)CSGOHitboxID2::Stomach);
			Multihitbox = false;
			break;
		}
	}
	if (GameUtils::AUGZZ(pWeapon))
	{
		Speed = Menu::Window.LegitBotTab.WeaponMainSpeedaug.GetValue() / 100;
		FoV = Menu::Window.LegitBotTab.WeaponMainFoVaug.GetValue();
		RecoilControl = Menu::Window.LegitBotTab.WeaponMainRecoilaug.GetState();
		PSilent = Menu::Window.LegitBotTab.WeaponMainPSilentaug.GetState();
		Inacc = Menu::Window.LegitBotTab.WeaponMainInaccaug.GetValue();

		switch (Menu::Window.LegitBotTab.WeaponMainHitboxaug.GetIndex())
		{
		case 0:
			HitBox = ((int)CSGOHitboxID2::Head);
			Multihitbox = false;
			break;
		case 1:
			HitBox = ((int)CSGOHitboxID2::Neck);
			Multihitbox = false;
			break;
		case 2:
			HitBox = ((int)CSGOHitboxID2::Chest);
			Multihitbox = false;
			break;
		case 3:
			HitBox = ((int)CSGOHitboxID2::Stomach);
			Multihitbox = false;
			break;
		}
	}
	if (GameUtils::FAMASZZ(pWeapon))
	{
		Speed = Menu::Window.LegitBotTab.WeaponMainSpeedfamas.GetValue() / 100;
		FoV = Menu::Window.LegitBotTab.WeaponMainFoVfamas.GetValue();
		RecoilControl = Menu::Window.LegitBotTab.WeaponMainRecoilfamas.GetState();
		PSilent = Menu::Window.LegitBotTab.WeaponMainPSilentfamas.GetState();
		Inacc = Menu::Window.LegitBotTab.WeaponMainInaccfamas.GetValue();

		switch (Menu::Window.LegitBotTab.WeaponMainHitboxfamas.GetIndex())
		{
		case 0:
			HitBox = ((int)CSGOHitboxID2::Head);
			Multihitbox = false;
			break;
		case 1:
			HitBox = ((int)CSGOHitboxID2::Neck);
			Multihitbox = false;
			break;
		case 2:
			HitBox = ((int)CSGOHitboxID2::Chest);
			Multihitbox = false;
			break;
		case 3:
			HitBox = ((int)CSGOHitboxID2::Stomach);
			Multihitbox = false;
			break;
		}
	}
	if (GameUtils::MP9ZZ(pWeapon))
	{
		Speed = Menu::Window.LegitBotTab.WeaponMainSpeedMP9.GetValue() / 100;
		FoV = Menu::Window.LegitBotTab.WeaponMainFoVMP9.GetValue();
		RecoilControl = Menu::Window.LegitBotTab.WeaponMainRecoilMP9.GetState();
		PSilent = Menu::Window.LegitBotTab.WeaponMainPSilentMP9.GetState();
		Inacc = Menu::Window.LegitBotTab.WeaponMainInaccMP9.GetValue();

		switch (Menu::Window.LegitBotTab.WeaponMainHitboxMP9.GetIndex())
		{
		case 0:
			HitBox = ((int)CSGOHitboxID2::Head);
			Multihitbox = false;
			break;
		case 1:
			HitBox = ((int)CSGOHitboxID2::Neck);
			Multihitbox = false;
			break;
		case 2:
			HitBox = ((int)CSGOHitboxID2::Chest);
			Multihitbox = false;
			break;
		case 3:
			HitBox = ((int)CSGOHitboxID2::Stomach);
			Multihitbox = false;
			break;
		}
	}
	if (GameUtils::p90zz(pWeapon))
	{
		Speed = Menu::Window.LegitBotTab.WeaponMainSpeedP90.GetValue() / 100;
		FoV = Menu::Window.LegitBotTab.WeaponMainFoVP90.GetValue();
		RecoilControl = Menu::Window.LegitBotTab.WeaponMainRecoilP90.GetState();
		PSilent = Menu::Window.LegitBotTab.WeaponMainPSilentP90.GetState();
		Inacc = Menu::Window.LegitBotTab.WeaponMainInaccP90.GetValue();

		switch (Menu::Window.LegitBotTab.WeaponMainHitboxP90.GetIndex())
		{
		case 0:
			HitBox = ((int)CSGOHitboxID2::Head);
			Multihitbox = false;
			break;
		case 1:
			HitBox = ((int)CSGOHitboxID2::Neck);
			Multihitbox = false;
			break;
		case 2:
			HitBox = ((int)CSGOHitboxID2::Chest);
			Multihitbox = false;
			break;
		case 3:
			HitBox = ((int)CSGOHitboxID2::Stomach);
			Multihitbox = false;
			break;
		}
	}
	if (GameUtils::DESERT2(pWeapon))
	{
		Speed = Menu::Window.LegitBotTab.WeaponPistSpeedD.GetValue() / 100;
		FoV = Menu::Window.LegitBotTab.WeaponPistFoVD.GetValue();
		RecoilControl = Menu::Window.LegitBotTab.WeaponPistRecoilD.GetState();
		PSilent = Menu::Window.LegitBotTab.WeaponPistPSilentD.GetState();
		Inacc = Menu::Window.LegitBotTab.WeaponPistInaccD.GetValue();

		switch (Menu::Window.LegitBotTab.WeaponPistHitboxD.GetIndex())
		{
		case 0:
			HitBox = ((int)CSGOHitboxID2::Head);
			Multihitbox = false;
			break;
		case 1:
			HitBox = ((int)CSGOHitboxID2::Neck);
			Multihitbox = false;
			break;
		case 2:
			HitBox = ((int)CSGOHitboxID2::Chest);
			Multihitbox = false;
			break;
		case 3:
			HitBox = ((int)CSGOHitboxID2::Stomach);
			Multihitbox = false;
			break;
		}
	}
	if (GameUtils::CZ2(pWeapon))
	{
		Speed = Menu::Window.LegitBotTab.WeaponPistSpeedCZ.GetValue() / 100;
		FoV = Menu::Window.LegitBotTab.WeaponPistFoVCZ.GetValue();
		RecoilControl = Menu::Window.LegitBotTab.WeaponPistRecoilCZ.GetState();
		PSilent = Menu::Window.LegitBotTab.WeaponPistPSilentCZ.GetState();
		Inacc = Menu::Window.LegitBotTab.WeaponPistInaccCZ.GetValue();

		switch (Menu::Window.LegitBotTab.WeaponPistHitboxCZ.GetIndex())
		{
		case 0:
			HitBox = ((int)CSGOHitboxID2::Head);
			Multihitbox = false;
			break;
		case 1:
			HitBox = ((int)CSGOHitboxID2::Neck);
			Multihitbox = false;
			break;
		case 2:
			HitBox = ((int)CSGOHitboxID2::Chest);
			Multihitbox = false;
			break;
		case 3:
			HitBox = ((int)CSGOHitboxID2::Stomach);
			Multihitbox = false;
			break;
		}
	}
	if (GameUtils::TEC9(pWeapon))
	{
		Speed = Menu::Window.LegitBotTab.WeaponPistSpeedtec.GetValue() / 100;
		FoV = Menu::Window.LegitBotTab.WeaponPistFoVtec.GetValue();
		RecoilControl = Menu::Window.LegitBotTab.WeaponPistRecoiltec.GetState();
		PSilent = Menu::Window.LegitBotTab.WeaponPistPSilenttec.GetState();
		Inacc = Menu::Window.LegitBotTab.WeaponPistInacctec.GetValue();

		switch (Menu::Window.LegitBotTab.WeaponPistHitboxtec.GetIndex())
		{
		case 0:
			HitBox = ((int)CSGOHitboxID2::Head);
			Multihitbox = false;
			break;
		case 1:
			HitBox = ((int)CSGOHitboxID2::Neck);
			Multihitbox = false;
			break;
		case 2:
			HitBox = ((int)CSGOHitboxID2::Chest);
			Multihitbox = false;
			break;
		case 3:
			HitBox = ((int)CSGOHitboxID2::Stomach);
			Multihitbox = false;
			break;
		}
	}
	if (GameUtils::P250ZZ(pWeapon))
	{
		Speed = Menu::Window.LegitBotTab.WeaponPistSpeedp250.GetValue() / 100;
		FoV = Menu::Window.LegitBotTab.WeaponPistFoVp250.GetValue();
		RecoilControl = Menu::Window.LegitBotTab.WeaponPistRecoilp250.GetState();
		PSilent = Menu::Window.LegitBotTab.WeaponPistPSilentp250.GetState();
		Inacc = Menu::Window.LegitBotTab.WeaponPistInaccp250.GetValue();

		switch (Menu::Window.LegitBotTab.WeaponPistHitboxp250.GetIndex())
		{
		case 0:
			HitBox = ((int)CSGOHitboxID2::Head);
			Multihitbox = false;
			break;
		case 1:
			HitBox = ((int)CSGOHitboxID2::Neck);
			Multihitbox = false;
			break;
		case 2:
			HitBox = ((int)CSGOHitboxID2::Chest);
			Multihitbox = false;
			break;
		case 3:
			HitBox = ((int)CSGOHitboxID2::Stomach);
			Multihitbox = false;
			break;
		}
	}
	if (GameUtils::glockzz(pWeapon))
	{
		Speed = Menu::Window.LegitBotTab.WeaponPistSpeedGLOCK.GetValue() / 100;
		FoV = Menu::Window.LegitBotTab.WeaponPistFoVGLOCK.GetValue();
		RecoilControl = Menu::Window.LegitBotTab.WeaponPistRecoilGLOCK.GetState();
		PSilent = Menu::Window.LegitBotTab.WeaponPistPSilentGLOCK.GetState();
		Inacc = Menu::Window.LegitBotTab.WeaponPistInaccGLOCK.GetValue();

		switch (Menu::Window.LegitBotTab.WeaponPistHitboxGLOCK.GetIndex())
		{
		case 0:
			HitBox = ((int)CSGOHitboxID2::Head);
			Multihitbox = false;
			break;
		case 1:
			HitBox = ((int)CSGOHitboxID2::Neck);
			Multihitbox = false;
			break;
		case 2:
			HitBox = ((int)CSGOHitboxID2::Chest);
			Multihitbox = false;
			break;
		case 3:
			HitBox = ((int)CSGOHitboxID2::Stomach);
			Multihitbox = false;
			break;
		}
	}
	if (GameUtils::FIVEN7(pWeapon))
	{
		Speed = Menu::Window.LegitBotTab.WeaponPistSpeedFIVEN7.GetValue() / 100;
		FoV = Menu::Window.LegitBotTab.WeaponPistFoVFIVEN7.GetValue();
		RecoilControl = Menu::Window.LegitBotTab.WeaponPistRecoilFIVEN7.GetState();
		PSilent = Menu::Window.LegitBotTab.WeaponPistPSilentFIVEN7.GetState();
		Inacc = Menu::Window.LegitBotTab.WeaponPistInaccFIVEN7.GetValue();

		switch (Menu::Window.LegitBotTab.WeaponPistHitboxFIVEN7.GetIndex())
		{
		case 0:
			HitBox = ((int)CSGOHitboxID2::Head);
			Multihitbox = false;
			break;
		case 1:
			HitBox = ((int)CSGOHitboxID2::Neck);
			Multihitbox = false;
			break;
		case 2:
			HitBox = ((int)CSGOHitboxID2::Chest);
			Multihitbox = false;
			break;
		case 3:
			HitBox = ((int)CSGOHitboxID2::Stomach);
			Multihitbox = false;
			break;
		}
	}
	if (GameUtils::AWPZZ(pWeapon))
	{
		Speed = Menu::Window.LegitBotTab.WeaponSnipSpeedAWP.GetValue() / 100;
		FoV = Menu::Window.LegitBotTab.WeaponSnipFoVAWP.GetValue();
		RecoilControl = Menu::Window.LegitBotTab.WeaponSnipRecoilAWP.GetState();
		PSilent = Menu::Window.LegitBotTab.WeaponSnipPSilentAWP.GetState();
		Inacc = Menu::Window.LegitBotTab.WeaponSnipInaccAWP.GetValue();

		switch (Menu::Window.LegitBotTab.WeaponSnipHitboxAWP.GetIndex())
		{
		case 0:
			HitBox = ((int)CSGOHitboxID2::Head);
			Multihitbox = false;
			break;
		case 1:
			HitBox = ((int)CSGOHitboxID2::Neck);
			Multihitbox = false;
			break;
		case 2:
			HitBox = ((int)CSGOHitboxID2::Chest);
			Multihitbox = false;
			break;
		case 3:
			HitBox = ((int)CSGOHitboxID2::Stomach);
			Multihitbox = false;
			break;
		}
	}
	if (GameUtils::G3SG1ZZ(pWeapon))
	{
		Speed = Menu::Window.LegitBotTab.WeaponSnipSpeedG3SG1.GetValue() / 100;
		FoV = Menu::Window.LegitBotTab.WeaponSnipFoVG3SG1.GetValue();
		RecoilControl = Menu::Window.LegitBotTab.WeaponSnipRecoilG3SG1.GetState();
		PSilent = Menu::Window.LegitBotTab.WeaponSnipPSilentG3SG1.GetState();
		Inacc = Menu::Window.LegitBotTab.WeaponSnipInaccG3SG1.GetValue();

		switch (Menu::Window.LegitBotTab.WeaponSnipHitboxG3SG1.GetIndex())
		{
		case 0:
			HitBox = ((int)CSGOHitboxID2::Head);
			Multihitbox = false;
			break;
		case 1:
			HitBox = ((int)CSGOHitboxID2::Neck);
			Multihitbox = false;
			break;
		case 2:
			HitBox = ((int)CSGOHitboxID2::Chest);
			Multihitbox = false;
			break;
		case 3:
			HitBox = ((int)CSGOHitboxID2::Stomach);
			Multihitbox = false;
			break;
		}
	}
	if (GameUtils::SSG08Z(pWeapon))
	{
		Speed = Menu::Window.LegitBotTab.WeaponSnipSpeedSSG08Z.GetValue() / 100;
		FoV = Menu::Window.LegitBotTab.WeaponSnipFoVSSG08Z.GetValue();
		RecoilControl = Menu::Window.LegitBotTab.WeaponSnipRecoilSSG08Z.GetState();
		PSilent = Menu::Window.LegitBotTab.WeaponSnipPSilentSSG08Z.GetState();
		Inacc = Menu::Window.LegitBotTab.WeaponSnipInaccSSG08Z.GetValue();

		switch (Menu::Window.LegitBotTab.WeaponSnipHitboxSSG08Z.GetIndex())
		{
		case 0:
			HitBox = ((int)CSGOHitboxID2::Head);
			Multihitbox = false;
			break;
		case 1:
			HitBox = ((int)CSGOHitboxID2::Neck);
			Multihitbox = false;
			break;
		case 2:
			HitBox = ((int)CSGOHitboxID2::Chest);
			Multihitbox = false;
			break;
		case 3:
			HitBox = ((int)CSGOHitboxID2::Stomach);
			Multihitbox = false;
			break;
		}
	}
}

// Functionality
void CLegitBot::DoAimbot(CUserCmd *pCmd, bool &bSendPacket)
{
	IClientEntity* pTarget = nullptr;
	IClientEntity* pLocal = hackManager.pLocal();
	bool FindNewTarget = true;
	//IsLocked = false;

	// Don't aimbot with the knife..
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	if (pWeapon)
	{
		if (pWeapon->GetAmmoInClip() == 0 || !GameUtils::IsBallisticWeapon(pWeapon))
		{
			//TargetID = 0;
			//pTarget = nullptr;
			//HitBox = -1;
			return;
		}
		SyncWeaponSettings();

	}
	else
		return;

	// Make sure we have a good target
	if (IsLocked && TargetID >= 0 && HitBox >= 0)
	{
		pTarget = Interfaces::EntList->GetClientEntity(TargetID);
		if (pTarget  && TargetMeetsRequirements(pTarget))
		{
			SyncWeaponSettings();
			if (HitBox >= 0)
			{
				Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
				Vector View; Interfaces::Engine->GetViewAngles(View);
				View += pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.f;
				float nFoV = FovToPlayer(ViewOffset, View, pTarget, HitBox);
				if (nFoV < FoV)
					FindNewTarget = false;
			}
		}
	}

	// Find a new target, apparently we need to
	if (FindNewTarget)
	{
		TargetID = 0;
		pTarget = nullptr;
		HitBox = -1;

		TargetID = GetTargetCrosshair();

		// Memes
		if (TargetID >= 0)
		{
			pTarget = Interfaces::EntList->GetClientEntity(TargetID);
		}
		else
		{
			pTarget = nullptr;
			HitBox = -1;
		}
	}

	SyncWeaponSettings();

	// If we finally have a good target
	if (TargetID >= 0 && pTarget)
	{
		//HitBox = (int)CSGOHitboxID2::Head;//
		SyncWeaponSettings();

		// Key
		if (Menu::Window.LegitBotTab.AimbotKeyPress.GetState())
		{
			int Key = Menu::Window.LegitBotTab.AimbotKeyBind.GetKey();
			if (Key >= 0 && !GUI.GetKeyState(Key))
			{
				TargetID = -1;
				pTarget = nullptr;
				HitBox = -1;
				return;
			}
		}

		Vector AimPoint = GetHitboxPosition(pTarget, HitBox);

		if (Multihitbox)
		{
			AimPoint = GetHitboxPosition(pTarget, besthitbox);
		}
		else
		{
			AimPoint = GetHitboxPosition(pTarget, HitBox);
		}

		if (AimAtPoint(pLocal, AimPoint, pCmd, bSendPacket))
		{
			//IsLocked = true;
			if (Menu::Window.LegitBotTab.AimbotAutoFire.GetState() && !(pCmd->buttons & IN_ATTACK))
			{
				pCmd->buttons |= IN_ATTACK;
			}
		}
	}

	// Auto Pistol
	static bool WasFiring = false;
	if (GameUtils::IsPistol(pWeapon) && Menu::Window.LegitBotTab.AimbotAutoPistol.GetState())
	{
		if (pCmd->buttons & IN_ATTACK)
		{
			static bool WasFiring = false;
			WasFiring = !WasFiring;

			if (WasFiring)
			{
				pCmd->buttons &= ~IN_ATTACK;
			}
		}
	}
}

bool TargetMeetsTriggerRequirements(IClientEntity* pEntity)
{

	if (pEntity && pEntity->IsDormant() == false && pEntity->IsAlive() && pEntity->GetIndex() != hackManager.pLocal()->GetIndex())
	{

		ClientClass *pClientClass = pEntity->GetClientClass();
		player_info_t pinfo;
		if (pClientClass->m_ClassID == (int)CSGOClassID::CCSPlayer && Interfaces::Engine->GetPlayerInfo(pEntity->GetIndex(), &pinfo))
		{

			if (pEntity->GetTeamNum() != hackManager.pLocal()->GetTeamNum() || Menu::Window.LegitBotTab.AimbotFriendlyFire.GetState())
			{

				if (!pEntity->HasGunGameImmunity())
				{
					return true;
				}
			}
		}
	}


	return false;
}

void CLegitBot::DoTrigger(CUserCmd *pCmd)
{
	IClientEntity* pLocal = hackManager.pLocal();

	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	if (pWeapon)
	{
		if (pWeapon->GetAmmoInClip() == 0 || !GameUtils::IsBallisticWeapon(pWeapon))
		{
			return;
		}
	}
	else
		return;

	Vector ViewAngles;
	Interfaces::Engine->GetViewAngles(ViewAngles);
	ViewAngles += pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.f;

	Vector fowardVec;
	AngleVectors(ViewAngles, &fowardVec);
	fowardVec.NormalizeInPlace();
	fowardVec *= 10000;

	Vector start = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector end = start + fowardVec, endScreen;

	trace_t Trace;

	Ray_t ray;

	CTraceFilter traceFilter;
	traceFilter.pSkip = pLocal;

	ray.Init(start, end);
	Interfaces::Trace->TraceRay(ray, MASK_SHOT, &traceFilter, &Trace);

	if (Trace.m_pEnt && 0 < Trace.hitgroup <= 7)
	{
		if (TargetMeetsTriggerRequirements(Trace.m_pEnt))
		{
			pCmd->buttons |= IN_ATTACK;
		}
	}
}
bool CLegitBot::TargetMeetsRequirements(IClientEntity* pEntity)
{

	if (pEntity && pEntity->IsDormant() == false && pEntity->IsAlive() && pEntity->GetIndex() != hackManager.pLocal()->GetIndex())
	{
		ClientClass *pClientClass = pEntity->GetClientClass();
		player_info_t pinfo;
		if (pClientClass->m_ClassID == (int)CSGOClassID::CCSPlayer && Interfaces::Engine->GetPlayerInfo(pEntity->GetIndex(), &pinfo))
		{
			if (pEntity->GetTeamNum() != hackManager.pLocal()->GetTeamNum() || Menu::Window.LegitBotTab.AimbotFriendlyFire.GetState())
			{

				if (Menu::Window.LegitBotTab.AimbotSmokeCheck.GetState()) {
					typedef bool(__cdecl* GoesThroughSmoke)(Vector, Vector);

					static uint32_t GoesThroughSmokeOffset = (uint32_t)Utilities::Memory::FindPatternV2("client_panorama.dll", "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0");
					static GoesThroughSmoke GoesThroughSmokeFunction = (GoesThroughSmoke)GoesThroughSmokeOffset;

					if (GoesThroughSmokeFunction(hackManager.pLocal()->GetEyePosition(), pEntity->GetBonePos(8)))
						return false;
				}

				if (Multihitbox)
				{
					if (!pEntity->HasGunGameImmunity())
					{
						if (GameUtils::IsVisible(hackManager.pLocal(), pEntity, 0) || GameUtils::IsVisible(hackManager.pLocal(), pEntity, 8) || GameUtils::IsVisible(hackManager.pLocal(), pEntity, 4 || GameUtils::IsVisible(hackManager.pLocal(), pEntity, 1 || GameUtils::IsVisible(hackManager.pLocal(), pEntity, 2 || GameUtils::IsVisible(hackManager.pLocal(), pEntity, 3 || GameUtils::IsVisible(hackManager.pLocal(), pEntity, 5 || GameUtils::IsVisible(hackManager.pLocal(), pEntity, 7 || GameUtils::IsVisible(hackManager.pLocal(), pEntity, 9 || GameUtils::IsVisible(hackManager.pLocal(), pEntity, 10 || GameUtils::IsVisible(hackManager.pLocal(), pEntity, 6))))))))))
							return true;
						else
							return false;
					}
				}
				else
				{
					if (!pEntity->HasGunGameImmunity() && GameUtils::IsVisible(hackManager.pLocal(), pEntity, HitBox))
					{
						return true;
					}
				}

			}

		}
	}
	return false;
}


float CLegitBot::FovToPlayer(Vector ViewOffSet, Vector View, IClientEntity* pEntity, int aHitBox)
{
	Vector out[9];

	// Anything past 180 degrees is just going to wrap around
	CONST FLOAT MaxDegrees = 180.0f;

	// Get local angles
	Vector Angles = View;

	// Get local view / eye position
	Vector Origin = ViewOffSet;

	// Create and intiialize vectors for calculations below
	Vector Delta(0, 0, 0);
	//Vector Origin(0, 0, 0);
	Vector Forward(0, 0, 0);

	// Convert angles to normalized directional forward vector
	AngleVectors(Angles, &Forward);
	Vector AimPos = GetHitboxPosition(pEntity, aHitBox);
	// Get delta vector between our local eye position and passed vector
	VectorSubtract(AimPos, Origin, Delta);
	//Delta = AimPos - Origin;

	// Normalize our delta vector
	Normalize(Delta, Delta);

	// Get dot product between delta position and directional forward vectors
	FLOAT DotProduct = Forward.Dot(Delta);

	// Time to calculate the field of view
	return (acos(DotProduct) * (MaxDegrees / PI));
}

int CLegitBot::GetTargetCrosshair()
{
	// Target selection
	SyncWeaponSettings();
	int target = -1;
	float minFoV = FoV;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);
	View += pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.f;

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			if (Multihitbox)
			{

				float fov1 = FovToPlayer(ViewOffset, View, pEntity, 0);
				float fov2 = FovToPlayer(ViewOffset, View, pEntity, 4);
				float fov3 = FovToPlayer(ViewOffset, View, pEntity, 6);

				if (fov1 < FoV || fov2 < FoV && fov1 < FoV || fov3 < FoV)
				{
					FoV = fov1;
					target = i;
					besthitbox = 0;
				}

				if (fov2 < FoV || fov1 < FoV && fov2 < FoV || fov3 < FoV)
				{
					FoV = fov2;
					target = i;
					besthitbox = 4;
				}

				if (fov3 < FoV || fov1 < FoV && fov3 < FoV || fov2 < FoV)
				{
					FoV = fov3;
					target = i;
					besthitbox = 6;
				}

			}
			else
			{
				int NewHitBox = HitBox;
				if (NewHitBox >= 0)
				{
					float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
					if (fov < minFoV)
					{
						minFoV = fov;
						target = i;
					}
				}
			}
		}
	}

	return target;
}

bool ShouldFire()
{
	IClientEntity* pLocalEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (!pLocalEntity)
		return false;

	CBaseCombatWeapon* entwep = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocalEntity->GetActiveWeaponHandle());

	float flServerTime = (float)pLocalEntity->GetTickBase() * Interfaces::Globals->interval_per_tick;
	float flNextPrimaryAttack = entwep->GetNextPrimaryAttack();

	std::cout << flServerTime << " " << flNextPrimaryAttack << std::endl;

	return !(flNextPrimaryAttack > flServerTime);
}

bool CLegitBot::AimAtPoint(IClientEntity* pLocal, Vector point, CUserCmd *pCmd, bool &bSendPacket)
{
	// Get the full angles
	if (point.Length() == 0) return false;

	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	static Vector Inaccuracy;

	if (timeSoFar > 0.2)
	{
		Inaccuracy.Init(-50 + rand() % 100, -50 + rand() % 100, -50 + rand() % 100);
		Inaccuracy.NormalizeInPlace();
		Inaccuracy *= Inacc;
		start_t = clock();
	}

	point += Inaccuracy;
	Vector angles;
	Vector src = pLocal->GetOrigin() + pLocal->GetViewOffset();

	CalcAngle(src, point, angles);
	GameUtils::NormaliseViewAngle(angles);

	if (angles[0] != angles[0] || angles[1] != angles[1])
	{
		return false;
	}

	if (RecoilControl)
	{
		Vector AimPunch = pLocal->localPlayerExclusive()->GetAimPunchAngle();
		if (AimPunch.Length2D() > 0 && AimPunch.Length2D() < 150)
		{
			angles -= AimPunch * 2;
			GameUtils::NormaliseViewAngle(angles);
		}
	}

	IsLocked = true;
	//-----------------------------------------------

	Vector ang = angles - pCmd->viewangles;
	bool v = false;

	if (ang.Length() > Speed)
	{
		Normalize(ang, ang);
		ang *= Speed;
	}
	else
	{
		v = true;
	}

	if (PSilent)
	{
		Vector Oldview = pCmd->viewangles;
		Vector qAimAngles = pCmd->viewangles;
		float Oldsidemove = pCmd->sidemove;
		float Oldforwardmove = pCmd->forwardmove;

		static int ChokedPackets = -1;

		if (ShouldFire() && ChokedPackets < 6)
		{
			bSendPacket = false;
			pCmd->viewangles += ang;
			pCmd->viewangles = angles;
			ChokedPackets++;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles = Oldview;
			pCmd->sidemove = Oldsidemove;
			pCmd->forwardmove = Oldforwardmove;
			ChokedPackets = -1;
		}

		pCmd->viewangles.z = 0;
	}
	else
	{
		pCmd->viewangles += ang;
		Interfaces::Engine->SetViewAngles(pCmd->viewangles);
	}
	return v;
}
