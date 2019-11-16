/*
- hoodhokage by ibanned -
*/

#pragma once
extern float autowalldmgtest[65];
#include "Hacks.h"
#include "Globalshhh.h"
class CRageBot : public CHack
{
public:
	void Init();
	void Draw();
	void Move(CUserCmd *pCmd, bool &bSendPacket);
private:
	// Targetting
	int GetTargetCrosshair();
	int GetTargetDistance();
	//int GetTargetDistance(CUserCmd* pCmd);
	int GetTargetHealth();
	bool TargetMeetsRequirements(IClientEntity* pEntity);
	void aimAtPlayer(CUserCmd* pCmd);
	float FovToPlayer(Vector ViewOffSet, Vector View, IClientEntity* pEntity, int HitBox);
	int HitScan(IClientEntity* pEntity);
	bool AimAtPoint(IClientEntity* pLocal, Vector point, CUserCmd *pCmd, bool &bSendPacket);

	// Functionality
	void DoAimbot(CUserCmd *pCmd, bool &bSendPacket);
	void DoNoRecoil(CUserCmd *pCmd);
	float InterpolationFix(CUserCmd* pCmd);
	void PositionAdjustment(CUserCmd* pCmd);

	void Edgezin(CBaseCombatWeapon * pLocalBaseEntity, CInput::CUserCmd * cmd, float Wall, float Corner);

	// AntiAim
	void DoAntiAim(CUserCmd *pCmd, bool&bSendPacket);
	bool EdgeAntiAim(IClientEntity* pLocalBaseEntity, CUserCmd* cmd, float flWall, float flCornor);
	bool bEdge;

	// AimStep
	bool IsAimStepping;
	Vector LastAimstepAngle;
	Vector LastAngle;

	// Aimbot
	bool IsLocked;
	int TargetID;
	int HitBox;
	Vector AimPoint;
};

extern float headPos;
extern float flipBool;
