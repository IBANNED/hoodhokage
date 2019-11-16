/*
- hoodhokage by ibanned -
*/

#include "RageBot.h"
#include "RenderManager.h"
#include "Resolver.h"
#include "Autowall.h"
#include <iostream>
#include "UTIL Functions.h"

#define TICK_INTERVAL			( Interfaces::Globals->interval_per_tick )
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t ) ( Interfaces::Globals->interval_per_tick *( t ) )

void CRageBot::Init()
{
	IsAimStepping = false;
	IsLocked = false;
	TargetID = -1;
}

void CRageBot::Draw()
{

}

bool IsAbleToShoot(IClientEntity* pLocal)
{
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	if (!pLocal)
		return false;

	if (!pWeapon)
		return false;

	float flServerTime = pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;

	return (!(pWeapon->GetNextPrimaryAttack() > flServerTime));
}


float hitchance(IClientEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	//	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	float hitchance = 101;
	if (!pWeapon) return 0;
	if (Menu::Window.RageBotTab.AccuracyHitchanceVal.GetValue() > 1)
	{//Inaccuracy method
		float inaccuracy = pWeapon->GetInaccuracy();
		if (inaccuracy == 0) inaccuracy = 0.0000001;
		inaccuracy = 1 / inaccuracy;
		hitchance = inaccuracy;

	}
	return hitchance;
}

bool spreadlimit(IClientEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	if (!pWeapon) return false;
	float inaccuracy = pWeapon->GetInaccuracy();
	return inaccuracy < (float)Menu::Window.RageBotTab.AccuracyHitchanceVal.GetValue() / 1000.f;
}


// (DWORD)g_pNetVars->GetOffset("DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
// You need something like this
bool CanOpenFire() // Creds to untrusted guy
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

void CRageBot::Move(CUserCmd *pCmd, bool &bSendPacket)
{
	IClientEntity* pLocalEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (!pLocalEntity)
		return;

	// Master switch
	if (!Menu::Window.RageBotTab.AimbotEnable.GetState())
		return;

	// Anti Aim 
	if (Menu::Window.RageBotTab.AimbotEnable.GetState())
	{
		static int ChokedPackets = -1;

		CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
		if (!pWeapon)
			return;

		if (ChokedPackets < 1 && pLocalEntity->GetLifeState() == LIFE_ALIVE && pCmd->buttons & IN_ATTACK && CanOpenFire() && GameUtils::IsBallisticWeapon(pWeapon))
		{
			bSendPacket = false;
		}
		else
		{
			if (pLocalEntity->GetLifeState() == LIFE_ALIVE)
			{
				DoAntiAim(pCmd, bSendPacket);
			}
			ChokedPackets = -1;
		}
	}

	// Position Adjustment
	if (Menu::Window.RageBotTab.AccuracyPositionAdjustment.GetState())
		PositionAdjustment(pCmd);

	// Aimbot
	if (Menu::Window.RageBotTab.AimbotEnable.GetState())
		DoAimbot(pCmd, bSendPacket);

	// Recoil
	if (Menu::Window.RageBotTab.AccuracyRecoil.GetState())
		DoNoRecoil(pCmd);

	// Aimstep
	if (Menu::Window.RageBotTab.AimbotAimStep.GetState())
	{
		Vector AddAngs = pCmd->viewangles - LastAngle;
		if (AddAngs.Length2D() > 25.f)
		{
			Normalize(AddAngs, AddAngs);
			AddAngs *= 25;
			pCmd->viewangles = LastAngle + AddAngs;
			GameUtils::NormaliseViewAngle(pCmd->viewangles);
		}
	}

	LastAngle = pCmd->viewangles;
}

Vector BestPoint(IClientEntity *targetPlayer, Vector &final)
{
	IClientEntity* pLocal = hackManager.pLocal();

	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pHit = targetPlayer;
	ray.Init(final + Vector(0, 0, 10), final);
	Interfaces::Trace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	final = tr.endpos;
	return final;
}

// Functionality
void CRageBot::DoAimbot(CUserCmd *pCmd, bool &bSendPacket) // Creds to encore1337 for getting it to work
{
	IClientEntity* pTarget = nullptr;
	IClientEntity* pLocal = hackManager.pLocal();
	Vector Start = pLocal->GetViewOffset() + pLocal->GetOrigin();
	bool FindNewTarget = true;
	//IsLocked = false;

	CSWeaponInfo* weapInfo = ((CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle()))->GetCSWpnData();

	// Don't aimbot with the knife..
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	if (GameUtils::IsRevolver(pWeapon) && Menu::Window.RageBotTab.AimbotAutoRevolver.GetState())
	{
		static int delay = 0;
		delay++;

		if (delay <= 15)
			pCmd->buttons |= IN_ATTACK;
		else
			delay = 0;
	}

	if (pWeapon)
	{
		if (pWeapon->GetAmmoInClip() == 0 || !GameUtils::IsBallisticWeapon(pWeapon))
		{
			TargetID = 0;
			pTarget = nullptr;
			HitBox = -1;
			return;
		}
	}
	else
		return;

	// Make sure we have a good target
	if (IsLocked && TargetID >= 0 && HitBox >= 0)
	{
		pTarget = Interfaces::EntList->GetClientEntity(TargetID);
		if (pTarget  && TargetMeetsRequirements(pTarget))
		{
			HitBox = HitScan(pTarget);
			if (HitBox >= 0)
			{
				Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
				Vector View;
				Interfaces::Engine->GetViewAngles(View);
				float FoV = FovToPlayer(ViewOffset, View, pTarget, HitBox);
				if (FoV < Menu::Window.RageBotTab.AimbotFov.GetValue())
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

		// Target selection type
		switch (Menu::Window.RageBotTab.TargetSelection.GetIndex())
		{
		case 0:
			TargetID = GetTargetCrosshair();
			break;
		}

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

	Globals::Target = pTarget;
	Globals::TargetID = TargetID;

	// If we finally have a good target
	if (TargetID >= 0 && pTarget)
	{
		// Get the hitbox to shoot at
		HitBox = HitScan(pTarget);

		if (!CanOpenFire())
			return;

		// Key
		if (Menu::Window.RageBotTab.AimbotKeyPress.GetState())
		{
			int Key = Menu::Window.RageBotTab.AimbotKeyBind.GetKey();
			if (Key >= 0 && !GUI.GetKeyState(Key))
			{
				TargetID = -1;
				pTarget = nullptr;
				HitBox = -1;
				return;
			}
		}

		// Stop key
		int StopKey = Menu::Window.RageBotTab.AimbotStopKey.GetKey();
		if (StopKey >= 0 && GUI.GetKeyState(StopKey))
		{
			TargetID = -1;
			pTarget = nullptr;
			HitBox = -1;
			return;
		}

		float pointscale = Menu::Window.RageBotTab.TargetPointscale.GetValue() - 5.f; // Aim height
		/*//float value = Menu::Window.RageBotTab.AccuracyHitchance.GetValue(); // Hitchance
		float hitchance = 75.f + (Menu::Window.RageBotTab.AccuracyHitchanceVal.GetValue() / 4);
		float Spread = pWeapon->GetAccuracyPenalty();*/

		Vector Point;
		Vector AimPoint = GetHitboxPosition(pTarget, HitBox) + Vector(0, 0, pointscale);

		pTarget->GetPredicted(AimPoint); /*Velocity Prediction*/

		if (Menu::Window.RageBotTab.TargetMultipoint.GetState())
		{
			Point = BestPoint(pTarget, AimPoint);
		}
		else
		{
			Point = AimPoint;
		}

		if (GameUtils::IsScopedWeapon(pWeapon) && !pWeapon->IsScoped() && Menu::Window.RageBotTab.AccuracyAutoScope.GetState()) // Autoscope
		{
			pCmd->buttons |= IN_ATTACK2;
		}
		else
		{
			switch (Menu::Window.RageBotTab.AccuracyDrop.GetIndex())
			{
		case 0:
			if ((Menu::Window.RageBotTab.AccuracyHitchanceVal.GetValue() * 1.5 <= hitchance(pLocal, pWeapon)) || Menu::Window.RageBotTab.AccuracyHitchanceVal.GetValue() == 0 || *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == 64)
			{
				if (AimAtPoint(pLocal, Point, pCmd, bSendPacket))
				{
					if (Menu::Window.RageBotTab.AimbotAutoFire.GetState() && !(pCmd->buttons & IN_ATTACK))
					{
						pCmd->buttons |= IN_ATTACK;
					}
					else
					{
						return;
					}
				}
				else if (Menu::Window.RageBotTab.AimbotAutoFire.GetState() && !(pCmd->buttons & IN_ATTACK))
				{
					pCmd->buttons |= IN_ATTACK;
				}
			}
			break;
		case 1:
			if (spreadlimit(pLocal, pWeapon))
			{
				if (AimAtPoint(pLocal, Point, pCmd, bSendPacket))
				{
					if (Menu::Window.RageBotTab.AimbotAutoFire.GetState() && !(pCmd->buttons & IN_ATTACK))
					{
						pCmd->buttons |= IN_ATTACK;
					}
					else
					{
						return;
					}
				}
				else if (Menu::Window.RageBotTab.AimbotAutoFire.GetState() && !(pCmd->buttons & IN_ATTACK))
				{
					pCmd->buttons |= IN_ATTACK;
				}
			}
			break;
			}
			
		}

		if (IsAbleToShoot(pLocal) && pCmd->buttons & IN_ATTACK)
			Globals::Shots += 1;
		//if (Menu::Window.RageBotTab.AccuracyBacktracking.GetState()) pCmd->tick_count += TICKS_TO_TIME(Interfaces::Globals->interval_per_tick);

		// Stop and Crouch
		if (TargetID >= 0 && pTarget)
		{
			if (Menu::Window.RageBotTab.AccuracyAutoStop.GetState())
			{
				pCmd->forwardmove = 0.f;
				pCmd->sidemove = 0.f;
			}
		}
	}

	if (Menu::Window.RageBotTab.Autorevolver.GetState()) {
		if (GameUtils::IsRevolver(pWeapon))
		{
			static int delay = 0;
			delay++;
			if (delay <= 15)pCmd->buttons |= IN_ATTACK;
			else delay = 0;
		}
	}

	// Auto Pistol
	if (GameUtils::IsPistol(pWeapon) && Menu::Window.LegitBotTab.AimbotAutoPistol.GetState())
	{
		if (pCmd->buttons & IN_ATTACK)
		{
			static bool Revolver = false;

			static bool WasFiring = false;
			WasFiring = !WasFiring;

			if (GameUtils::IsRevolver(pWeapon)) {

				Revolver;
			}
			if (WasFiring)
			{
				pCmd->buttons &= ~IN_ATTACK;
			}
		}
	}
	if (Menu::Window.RageBotTab.AutoHitChance.GetState())
	{
		if (GameUtils::IsSSG08(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(35);
		}

		else if (GameUtils::IsAK47(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(20);
		}

		else if (GameUtils::IsDEAGLE(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(65);
		}

		else if (GameUtils::IsAutoSniper(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(30);
		}

		else if (GameUtils::IsDuals(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(25);
		}

		else if (GameUtils::IsMP9(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(10);
		}

		else if (GameUtils::IsMP7(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(10);
		}

		else if (GameUtils::IsM4A1(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(20);
		}

		else if (GameUtils::IsM4A1S(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(20);
		}

		else if (GameUtils::IsFAMAS(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(20);
		}

		else if (GameUtils::IsMAC10(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(13);
		}

		else if (GameUtils::IsAWP(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(90);
		}

		else if (GameUtils::IsBizon(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(10);
		}

		else if (GameUtils::IsMP7(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(12);
		}

		else if (GameUtils::IsMP9(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(5);
		}

		else if (GameUtils::IsUMP(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(10);
		}

		else if (GameUtils::IsUSPS(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(20);
		}

		else if (GameUtils::IsGALIL(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(20);
		}

		else if (GameUtils::IsGLOCK(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(5);
		}

		else if (GameUtils::IsFSEVEN(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(10);
		}

		else if (GameUtils::IsP250(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(20);
		}

		else if (GameUtils::IsTEC9(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(20);
		}
		else if (GameUtils::IsRevolver(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(55);
		}
		else if (GameUtils::IsCZ75(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyHitchanceVal.SetValue(35);
		}

	}
	else
	{
		Menu::Window.RageBotTab.AccuracyHitchanceVal.GetValue();
	}


	if (Menu::Window.RageBotTab.AutoMinimumDamage.GetState())
	{
		if (GameUtils::IsSSG08(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(65);
		}

		else if (GameUtils::IsAK47(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(15);
		}

		else if (GameUtils::IsDEAGLE(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(65);
		}

		else if (GameUtils::IsAutoSniper(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(15);
		}

		else if (GameUtils::IsDuals(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(10);
		}

		else if (GameUtils::IsMP9(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(10);
		}

		else if (GameUtils::IsMP7(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(10);
		}

		else if (GameUtils::IsM4A1(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(15);
		}

		else if (GameUtils::IsM4A1S(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(15);
		}

		else if (GameUtils::IsFAMAS(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(15);
		}

		else if (GameUtils::IsMAC10(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(1);
		}

		else if (GameUtils::IsAWP(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(85);
		}

		else if (GameUtils::IsBizon(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(1);
		}

		else if (GameUtils::IsMP7(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(1);
		}

		else if (GameUtils::IsMP9(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(1);
		}

		else if (GameUtils::IsUMP(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(15);
		}

		else if (GameUtils::IsUSPS(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(15);
		}

		else if (GameUtils::IsGALIL(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(9);
		}

		else if (GameUtils::IsGLOCK(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(15);
		}

		else if (GameUtils::IsFSEVEN(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(15);
		}

		else if (GameUtils::IsP250(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(15);
		}

		else if (GameUtils::IsTEC9(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(15);
		}
		else if (GameUtils::IsRevolver(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(35);
		}
		else if (GameUtils::IsCZ75(pWeapon))
		{
			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue(35);
		}

	}
	else
	{
		Menu::Window.RageBotTab.AccuracyMinimumDamage.GetValue();
	}
}


bool CRageBot::TargetMeetsRequirements(IClientEntity* pEntity)
{
	// Is a valid player
	if (pEntity && pEntity->IsDormant() == false && pEntity->IsAlive() && pEntity->GetIndex() != hackManager.pLocal()->GetIndex())
	{
		// Entity Type checks
		ClientClass *pClientClass = pEntity->GetClientClass();
		player_info_t pinfo;
		if (pClientClass->m_ClassID == (int)CSGOClassID::CCSPlayer && Interfaces::Engine->GetPlayerInfo(pEntity->GetIndex(), &pinfo))
		{
			// Team Check
			if (pEntity->GetTeamNum() != hackManager.pLocal()->GetTeamNum() || Menu::Window.RageBotTab.TargetFriendlyFire.GetState())
			{
				// Spawn Check
				if (!pEntity->HasGunGameImmunity())
				{
					return true;
				}
			}
		}
	}

	// They must have failed a requirement
	return false;
}


float CRageBot::FovToPlayer(Vector ViewOffSet, Vector View, IClientEntity* pEntity, int aHitBox)
{
	CONST FLOAT MaxDegrees = 180.0f;
	Vector Angles = View;
	Vector Origin = ViewOffSet;
	Vector Delta(0, 0, 0);
	Vector Forward(0, 0, 0);
	AngleVectors(Angles, &Forward);
	Vector AimPos = GetHitboxPosition(pEntity, aHitBox);
	VectorSubtract(AimPos, Origin, Delta);
	Normalize(Delta, Delta);
	FLOAT DotProduct = Forward.Dot(Delta);
	return (acos(DotProduct) * (MaxDegrees / PI));
}

int CRageBot::GetTargetCrosshair()
{
	// Target selection
	int target = -1;
	float minFoV = Menu::Window.RageBotTab.AimbotFov.GetValue();

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++) //GetHighestEntityIndex()
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
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

	return target;
}

int CRageBot::GetTargetDistance()
{
	// New Distance Aimbot
	int target = -1;
	int minDist = 8192;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				Vector Difference = pLocal->GetOrigin() - pEntity->GetOrigin();
				int Distance = Difference.Length();
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (Distance < minDist && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minDist = Distance;
					target = i;
				}
			}
		}
	}
	return target;
}

int CRageBot::GetTargetHealth()
{
	// Target selection
	int target = -1;
	int minHealth = 101;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				int Health = pEntity->GetHealth();
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (Health < minHealth && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minHealth = Health;
					target = i;
				}
			}
		}
	}

	return target;
}

Vector TickPrediction(Vector AimPoint, IClientEntity* pTarget)
{
	return AimPoint + (pTarget->GetVelocity() * Interfaces::Globals->interval_per_tick);
}

int CRageBot::HitScan(IClientEntity* pEntity)
{
	IClientEntity* pLocal = hackManager.pLocal();
	std::vector<int> HitBoxesToScan;

#pragma region GetHitboxesToScan
	int HitScanMode = Menu::Window.RageBotTab.TargetHitscan.GetIndex();
	int huso = (pEntity->GetHealth());
	int health = Menu::Window.RageBotTab.BaimIfUnderXHealth.GetValue();
	bool AWall = Menu::Window.RageBotTab.AccuracyAutoWall.GetState();
	bool Multipoint = Menu::Window.RageBotTab.TargetMultipoint.GetState();
	int TargetHitbox = Menu::Window.RageBotTab.TargetHitbox.GetIndex();
	static bool enemyHP = false;
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());

	if (huso < health)
	{
		HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
		HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_BELLY);
		HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
		HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
		HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
		HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_UPPER_ARM);
		HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_UPPER_ARM);
		HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_THIGH);
		HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_THIGH);
		HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_HAND);
		HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_HAND);

		HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_FOREARM);
		HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_FOREARM);
	}
	else if (Menu::Window.RageBotTab.AwpAtBody.GetState() && GameUtils::AWP(pWeapon))
	{
		HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
		HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_BELLY);
		HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
		HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
		HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
	}
	else
	{
		if (HitScanMode == 0)
		{
			// No Hitscan, just a single hitbox
			switch (Menu::Window.RageBotTab.TargetHitbox.GetIndex())
			{
			case 0:
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_HEAD);
				break;
			case 1:
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_NECK);
				break;
			case 2:
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
				break;
			case 3:
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_BELLY);
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
				break;
			case 4:
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_CALF);
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_CALF);
				break;
			}
		}
		else
		{
			if (GameUtils::IsZeus(pWeapon))
			{

				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_BELLY);
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_THIGH);
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_THIGH);
			}
			else if (pEntity->GetHealth() <= Menu::Window.RageBotTab.BaimIfUnderXHealth.GetValue() && pEntity->GetHealth() != 0)
			{
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_NECK);
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_BELLY);
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_FOREARM);
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_FOREARM);
			}
			else if (GetAsyncKeyState(Menu::Window.RageBotTab.bigbaim.GetKey()))
			{
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_NECK);
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_BELLY);
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_THIGH);
				HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_THIGH);
			}
			else if (Globals::missedshots >= Menu::Window.RageBotTab.baim.GetValue())
			{
				if (Globals::missedshots <= Menu::Window.RageBotTab.bruteX.GetValue() || Globals::missedshots >= Menu::Window.RageBotTab.bruteX.GetValue() + 4)
				{
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_NECK);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_BELLY);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_THIGH);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_THIGH);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);

				}
				else if (GetAsyncKeyState(Menu::Window.RageBotTab.bigbaim.GetKey()))
				{
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_NECK);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_BELLY);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_THIGH);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_THIGH);
				}
				else
				{
					switch (HitScanMode)
					{
					case 1:

						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_HEAD);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_BELLY);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_THIGH);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_THIGH);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_CALF);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_CALF);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_FOOT);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_FOOT);
						break;
					case 2:
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_HEAD);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_NECK);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_BELLY);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_THIGH);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_THIGH);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_CALF);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_CALF);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_FOOT);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_FOOT);
						break;
					case 3:
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_HEAD);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_NECK);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_BELLY);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_THIGH);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_THIGH);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_FOOT);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_FOOT);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_NECK);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_HAND);
						HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_HAND);
						break;
					}
				}
			}
			else
			{
				switch (HitScanMode)
				{
				case 1:

					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_HEAD);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_NECK);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_BELLY);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_CALF);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_CALF);
					break;
				case 2:
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_HEAD);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_NECK);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_BELLY);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_FOOT);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_CALF);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_CALF);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_FOOT);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_FOOT);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_THIGH);
					HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_THIGH);

					break;
				case 3:
				{
					if (pEntity->IsMoving())
					{
						if (pEntity->GetMoveType() & FL_DUCKING)
						{
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_NECK);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_BELLY);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_THIGH);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_THIGH);
						}
						else
						{
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_HEAD);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_NECK);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_FOOT);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_FOOT);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_BELLY);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_FOREARM);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_FOREARM);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_THIGH);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_CALF);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_CALF);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
						}
					}
					else
					{
						if (pEntity->GetMoveType() & FL_DUCKING)
						{
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_NECK);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_BELLY);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_THIGH);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_THIGH);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_FOOT);
							HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_FOOT);
						}
						else
						{
							if (Globals::missedshots <= Menu::Window.RageBotTab.bruteX.GetValue() || Globals::missedshots >= Menu::Window.RageBotTab.bruteX.GetValue() + 4)
							{
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_HEAD);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_NECK);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_FOOT);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_FOOT);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_BELLY);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_FOREARM);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_FOREARM);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_THIGH);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_CALF);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_CALF);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_NECK);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
							}
							else
							{
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_HEAD);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_NECK);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_BELLY);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_PELVIS);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_FOREARM);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_RIGHT_FOREARM);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_LEFT_THIGH);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_NECK);
								HitBoxesToScan.push_back((int)CSGOHitboxID::HITBOX_UPPER_CHEST);
							}
						}
					}

				}

				break;
				}
			}
		}
	}

#pragma endregion Get the list of shit to scan

	// check hits
	for (auto HitBoxID : HitBoxesToScan)
	{
		if (HitBoxID >= 0)
		{
			Vector Point = GetHitboxPosition(pEntity, HitBoxID);
			Vector Pointhd = GetHitboxPosition(pEntity, 0);
			float headdmg = 0.f;
			float Damage = 0.f;
			Color c = Color(255, 255, 255, 255);

			if (CanHit(pEntity, Point, &Damage))
			{
				/*if (headdmg > Damage && !g_menu::Window.RageBotTab.AWPAtBody.GetState() && !g_menu::Window.RageBotTab.PreferBodyAim.GetState())
				bestHitbox = 0;*/
				autowalldmgtest[pEntity->GetIndex()] = Damage;
				if ((Damage >= Menu::Window.RageBotTab.AccuracyMinimumDamage.GetValue()) || (Menu::Window.RageBotTab.AccuracyMinimumDamage.GetValue() > pEntity->GetHealth()))
				{
					return HitBoxID;
				}
			}
			else
			{
				if (GameUtils::IsVisible(hackManager.pLocal(), pEntity, HitBoxID))
					return HitBoxID;
			}
		}
	}

	return -1;
}

float InterpolationFix(CUserCmd* pCmd)
{
	if (Menu::Window.RageBotTab.AccuracyPositionAdjustment.GetState())
	{
		static ConVar* cvar_cl_interp = Interfaces::CVar->FindVar("cl_interp");
		static ConVar* cvar_cl_updaterate = Interfaces::CVar->FindVar("cl_updaterate");
		static ConVar* cvar_sv_maxupdaterate = Interfaces::CVar->FindVar("sv_maxupdaterate");
		static ConVar* cvar_sv_minupdaterate = Interfaces::CVar->FindVar("sv_minupdaterate");
		static ConVar* cvar_cl_interp_ratio = Interfaces::CVar->FindVar("cl_interp_ratio");

		IClientEntity* pLocal = hackManager.pLocal();
		CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());

		float cl_interp = cvar_cl_interp->GetFloat();
		int cl_updaterate = cvar_cl_updaterate->GetInt();
		int sv_maxupdaterate = cvar_sv_maxupdaterate->GetInt();
		int sv_minupdaterate = cvar_sv_minupdaterate->GetInt();
		int cl_interp_ratio = cvar_cl_interp_ratio->GetInt();

		if (sv_maxupdaterate <= cl_updaterate)
			cl_updaterate = sv_maxupdaterate;

		if (sv_minupdaterate > cl_updaterate)
			cl_updaterate = sv_minupdaterate;

		float new_interp = (float)cl_interp_ratio / (float)cl_updaterate;

		if (new_interp > cl_interp)
			cl_interp = new_interp;

		return max(cl_interp, cl_interp_ratio / cl_updaterate);
	}
}

void CRageBot::PositionAdjustment(CUserCmd* pCmd)
{
	if (Menu::Window.RageBotTab.AccuracyPositionAdjustment.GetState())
	{
		static ConVar* cvar_cl_interp = Interfaces::CVar->FindVar("cl_interp");
		static ConVar* cvar_cl_updaterate = Interfaces::CVar->FindVar("cl_updaterate");
		static ConVar* cvar_sv_maxupdaterate = Interfaces::CVar->FindVar("sv_maxupdaterate");
		static ConVar* cvar_sv_minupdaterate = Interfaces::CVar->FindVar("sv_minupdaterate");
		static ConVar* cvar_cl_interp_ratio = Interfaces::CVar->FindVar("cl_interp_ratio");

		IClientEntity* pLocal = hackManager.pLocal();

		if (!pLocal)
			return;

		CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
		if (!pWeapon)
			return;

		float cl_interp = cvar_cl_interp->GetFloat();
		int cl_updaterate = cvar_cl_updaterate->GetInt();
		int sv_maxupdaterate = cvar_sv_maxupdaterate->GetInt();
		int sv_minupdaterate = cvar_sv_minupdaterate->GetInt();
		int cl_interp_ratio = cvar_cl_interp_ratio->GetInt();

		if (sv_maxupdaterate <= cl_updaterate)
			cl_updaterate = sv_maxupdaterate;

		if (sv_minupdaterate > cl_updaterate)
			cl_updaterate = sv_minupdaterate;

		float new_interp = (float)cl_interp_ratio / (float)cl_updaterate;

		if (new_interp > cl_interp)
			cl_interp = new_interp;

		float flSimTime = pLocal->GetSimulationTime();
		float flOldAnimTime = pLocal->GetAnimTime();

		int iTargetTickDiff = (int)(10.0f + (flSimTime - flOldAnimTime) / Interfaces::Globals->interval_per_tick);
	}
}

void CRageBot::DoNoRecoil(CUserCmd *pCmd)
{
	// Ghetto rcs shit, implement properly later
	IClientEntity* pLocal = hackManager.pLocal();
	if (pLocal)
	{
		Vector AimPunch = pLocal->localPlayerExclusive()->GetAimPunchAngle();
		if (AimPunch.Length2D() > 0 && AimPunch.Length2D())
		{
			pCmd->viewangles -= AimPunch * 2;
			GameUtils::NormaliseViewAngle(pCmd->viewangles);
		}
	}
}

bool CRageBot::AimAtPoint(IClientEntity* pLocal, Vector point, CUserCmd *pCmd, bool &bSendPacket)
{
	bool ReturnValue = false;
	// Get the full angles
	if (point.Length() == 0) return ReturnValue;
	IClientEntity* Pent;
	Vector angles;
	Vector src = pLocal->GetEyePosition();
	Vector delta = point - pLocal->GetEyePosition();

	VectorAngles(point - src, angles);
	if (Menu::Window.MiscTab.OtherSafeMode.GetState()) {
		GameUtils::NormaliseViewAngle(angles);
	}
	if (angles[0] != angles[0] || angles[1] != angles[1])
	{
		return ReturnValue;
	} //Uhh wut ?


	IsLocked = true;
	//-----------------------------------------------

	// Aim Step Calcs
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	if (!IsAimStepping)
		LastAimstepAngle = LastAngle; // Don't just use the viewangs because you need to consider aa

	float fovLeft = FovToPlayer(ViewOffset, LastAimstepAngle, Interfaces::EntList->GetClientEntity(TargetID), 0);

	if (fovLeft > 25.0f && Menu::Window.RageBotTab.AimbotAimStep.GetState())
	{
		Vector AddAngs = angles - LastAimstepAngle;
		Normalize(AddAngs, AddAngs);
		AddAngs *= 25;
		LastAimstepAngle += AddAngs;
		GameUtils::NormaliseViewAngle(LastAimstepAngle);
		angles = LastAimstepAngle;
	}
	else
	{
		ReturnValue = true;
	}


	switch (Menu::Window.RageBotTab.AimbotSilentAim.GetIndex())
	{

	case 0: /*Disabled*/
		Interfaces::Engine->SetViewAngles(angles);
		break;

	case 1: /*Client*/
		pCmd->viewangles = angles;
		break;

	case 2:
	// pSilent Aim
	CBaseCombatWeapon* entwep = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	QAngle oldView = pCmd->viewangles;
	float oldSidemove = pCmd->sidemove;
	float oldForwardmove = pCmd->forwardmove;
	float flServerTime = (float)pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;
	float flNextPrimaryAttack = entwep->GetNextPrimaryAttack();

		static int ChokedPackets = -1;
		ChokedPackets++;

		if (ChokedPackets < 6)
		{
			bSendPacket = false;
			pCmd->viewangles = angles;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles = oldView;
			pCmd->sidemove = oldSidemove;
			pCmd->forwardmove = oldForwardmove;
			ChokedPackets = -1;
			ReturnValue = false;
		}
	}
	return ReturnValue;
}