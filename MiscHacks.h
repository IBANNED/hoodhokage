/*
- hoodhokage by ibanned -
*/

#pragma once

#include "Hacks.h"

Vector GetAutostrafeView();

class CMiscHacks : public CHack
{
public:
	void Init();
	void Draw();
	void Move(CUserCmd *pCmd, bool &bSendPacket);
private:
	void AutoJump(CUserCmd *pCmd);
	void Telehop(CUserCmd *pCmd);
	void xGodWalk(CUserCmd * pCmd);
	void AutoJumpSMAC(CUserCmd *pCmd);
	void EdgeJump(CUserCmd *pCmd);
	void Walkbot(CUserCmd *pCmd);
	void PerfectStrafe(CUserCmd *pCmd);
	void LegitStrafe(CUserCmd *pCmd);
	void AutoPistol(CUserCmd * pCmd);
	void duckinair(CUserCmd * cmd);
	void RageStrafe(CUserCmd *pCmd);
	void circle_strafe(CUserCmd * cmd, float * circle_yaw);
	void FakeWalk(CUserCmd * pCmd, bool & bSendPacket);
	void ChatSpamInterwebz();
	void ChatSpamName();
	void ChatSpamDisperseName();
	void fakeping();
	void ChatSpamRegular();
	void ChatSpamReports();
	void ChatSpamCallout();
//	void Fakelag(CUserCmd *pCmd, bool &bSendPacket);
//	void FakelagInAir(CUserCmd *pCmd, bool &bSendPacket);
	void Crasher();
	void AmbientLighting();
	void AmbientExposure();
	void SpoofPostProcessing1();
	void SpoofPostProcessing();
	void AmbientSkybox();
	void RemoveScope();

	void OtherdoFakeWalk(CUserCmd * pCmd, bool & bSendPacket);

	//std::string GetNameChange();

	/*void ChatSpam();
	void Namechanger();

	std::string GetRegularSpam();
	std::string GetCustomSpam();
	std::string GetPlayerSpam();
	std::string GetSinglePlayerSpam();

	std::string GetNameChange();
	std::string GetNameCalloutChange();*/
};



