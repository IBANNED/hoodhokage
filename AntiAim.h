#pragma once

#include "Hacks.h"

class CAntiAim : public CHack
{
public:
	void Init();
	void Draw();
	void Move(CUserCmd *pCmd, bool &bSendPacket);
private:
	// Targetting
	void aimAtPlayer(CUserCmd* pCmd);
	bool TargetMeetsRequirements(IClientEntity* pEntity);

	// AntiAim
	void DoAntiAim(CUserCmd *pCmd, bool&bSendPacket);
	bool EdgeAntiAim(IClientEntity* pLocalBaseEntity, CUserCmd* cmd, float flWall, float flCornor);
	bool bEdge;
	bool freestanding(CUserCmd* m_pcmd, bool packet);

	// AimStep
	bool IsAimStepping;
	Vector LastAimstepAngle;
	Vector LastAngle;
};

extern void animstyte();
extern QAngle angAbsRotationAfterUpdatingClientSideAnimation;
extern QAngle angRotationAfterUpdatingClientSideAnimation;
extern QAngle angNetworkAnglesAfterUpdatingClientSideAnimation;
