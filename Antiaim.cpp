#include "RageBot.h"
#include "RenderManager.h"
#include "Resolver.h"
#include "Autowall.h"
#include <iostream>
#include "UTIL Functions.h"
#include "extrahelper.h"


/* AntiAim-List

YAW-Backward
YAW-Static
YAW-Jitter
YAW-Spinbot
YAW-FakeJitter
YAW-FakeSpin
YAW-FakeBackward
YAW-FakeForward
YAW-FakeStatic
*/

QAngle angAbsRotationAfterUpdatingClientSideAnimation;
QAngle angRotationAfterUpdatingClientSideAnimation;
QAngle angNetworkAnglesAfterUpdatingClientSideAnimation;
class CBaseAnimState
{
public:
	char pad[3];
	char bUnknown; //0x4
	char pad2[91];
	void* pBaseEntity; //0x60
	void* pActiveWeapon; //0x64
	void* pLastActiveWeapon; //0x68
	float m_flLastClientSideAnimationUpdateTime; //0x6C
	int m_iLastClientSideAnimationUpdateFramecount; //0x70
	float m_flEyePitch; //0x74
	float m_flEyeYaw; //0x78
	float m_flPitch; //0x7C
	float m_flGoalFeetYaw; //0x80
	float m_flCurrentFeetYaw; //0x84
	float m_flCurrentTorsoYaw; //0x88
	float m_flUnknownVelocityLean; //0x8C //changes when moving/jumping/hitting ground
	float m_flLeanAmount; //0x90
	char pad4[4]; //NaN
	float m_flFeetCycle; //0x98 0 to 1
	float m_flFeetYawRate; //0x9C 0 to 1
	float m_fUnknown2;
	float m_fDuckAmount; //0xA4
	float m_fLandingDuckAdditiveSomething; //0xA8
	float m_fUnknown3; //0xAC
	Vector m_vOrigin; //0xB0, 0xB4, 0xB8
	Vector m_vLastOrigin; //0xBC, 0xC0, 0xC4
	float m_vVelocityX; //0xC8
	float m_vVelocityY; //0xCC
	char pad5[4];
	float m_flUnknownFloat1; //0xD4 Affected by movement and direction
	char pad6[8];
	float m_flUnknownFloat2; //0xE0 //from -1 to 1 when moving and affected by direction
	float m_flUnknownFloat3; //0xE4 //from -1 to 1 when moving and affected by direction
	float m_unknown; //0xE8
	float speed_2d; //0xEC
	float flUpVelocity; //0xF0
	float m_flSpeedNormalized; //0xF4 //from 0 to 1
	float m_flFeetSpeedForwardsOrSideWays; //0xF8 //from 0 to 2. something  is 1 when walking, 2.something when running, 0.653 when crouch walking
	float m_flFeetSpeedUnknownForwardOrSideways; //0xFC //from 0 to 3. something
	float m_flTimeSinceStartedMoving; //0x100
	float m_flTimeSinceStoppedMoving; //0x104
	unsigned char m_bOnGround; //0x108
	unsigned char m_bInHitGroundAnimation; //0x109
	char pad7[10];
	float m_flLastOriginZ; //0x114
	float m_flHeadHeightOrOffsetFromHittingGroundAnimation; //0x118 from 0 to 1, is 1 when standing
	float m_flStopToFullRunningFraction; //0x11C from 0 to 1, doesnt change when walking or crouching, only running
	char pad8[4]; //NaN
	float m_flUnknownFraction; //0x124 affected while jumping and running, or when just jumping, 0 to 1
	char pad9[4]; //NaN
	float m_flUnknown3;
	char pad10[528];
};
void animstyte()
{
	if (Menu::Window.RageBotTab.AnimFixz.GetState())
	{
		auto local_player = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
		CBaseAnimState *animstate = local_player->GetAnimState();

		static matrix3x4 pBoneToWorldOut[128];
		//if (!entity->SetupBones(pBoneToWorldOut, 128, 256, INTERFACES::Globals->curtime))
		//  return;
		if (!(local_player->GetFlags() & FL_ONGROUND))
		{

			if (animstate->m_iLastClientSideAnimationUpdateFramecount)
				animstate->m_iLastClientSideAnimationUpdateFramecount -= Interfaces::Globals->interval_per_tick + Interfaces::Globals->interval_per_tick;

			if (animstate->m_flLastClientSideAnimationUpdateTime)
				animstate->m_flLastClientSideAnimationUpdateTime -= Interfaces::Globals->interval_per_tick + Interfaces::Globals->interval_per_tick;

			//local_player->UpdateClientSideAnimation();
		}
		else
		{
			if (animstate->m_iLastClientSideAnimationUpdateFramecount)
				animstate->m_iLastClientSideAnimationUpdateFramecount = Interfaces::Globals->interval_per_tick;

			if (animstate->m_flLastClientSideAnimationUpdateTime)
				animstate->m_flLastClientSideAnimationUpdateTime = Interfaces::Globals->curtime;

			//local_player->UpdateClientSideAnimation();
		}
		angAbsRotationAfterUpdatingClientSideAnimation = *(QAngle*)((DWORD)local_player + 0xC4);
		angRotationAfterUpdatingClientSideAnimation = *(QAngle*)((DWORD)local_player + 0xD0);
		angNetworkAnglesAfterUpdatingClientSideAnimation = *(QAngle*)((DWORD)local_player + 0x128);
	}
}

void AngleVectors3(const Vector &angles, Vector *forward)
{
	Assert(s_bMathlibInitialized);
	Assert(forward);

	float	sp, sy, cp, cy;

	sy = sin(DEG2RAD(angles[1]));
	cy = cos(DEG2RAD(angles[1]));

	sp = sin(DEG2RAD(angles[0]));
	cp = cos(DEG2RAD(angles[0]));

	forward->x = cp*cy;
	forward->y = cp*sy;
	forward->z = -sp;
}

template<class T, class U>
T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}

float RandomNumber(float Min, float Max)
{
	return ((float(rand()) / float(RAND_MAX)) * (Max - Min)) + Min;
}

float GetLatency()
{
	INetChannelInfo *nci = Interfaces::Engine->GetNetChannelInfo();
	if (nci)
	{
		float Latency = nci->GetAvgLatency(FLOW_OUTGOING) + nci->GetAvgLatency(FLOW_INCOMING);
		return Latency;
	}
	else
	{
		return 0.0f;
	}
}
float GetOutgoingLatency()
{
	INetChannelInfo *nci = Interfaces::Engine->GetNetChannelInfo();
	if (nci)
	{
		float OutgoingLatency = nci->GetAvgLatency(FLOW_OUTGOING);
		return OutgoingLatency;
	}
	else
	{
		return 0.0f;
	}
}
float GetIncomingLatency()
{
	INetChannelInfo *nci = Interfaces::Engine->GetNetChannelInfo();
	if (nci)
	{
		float IncomingLatency = nci->GetAvgLatency(FLOW_INCOMING);
		return IncomingLatency;
	}
	else
	{
		return 0.0f;
	}
}

float OldLBY;
float LBYBreakerTimer;
float LastLBYUpdateTime;
bool bSwitch;
float CurrentVelocity(IClientEntity* LocalPlayer)
{
	int vel = LocalPlayer->GetVelocity().Length2D();
	return vel;
}

/*bool NextLBYUpdate()
{
	IClientEntity* LocalPlayer = hackManager.pLocal();

	float flServerTime = (float)(LocalPlayer->GetTickBase()  * Interfaces::Globals->interval_per_tick);


	if (OldLBY != LocalPlayer->GetLowerBodyYaw())
	{
		LBYBreakerTimer++;
		OldLBY = LocalPlayer->GetLowerBodyYaw();
		bSwitch = !bSwitch;
		LastLBYUpdateTime = flServerTime;
	}

	if (CurrentVelocity(LocalPlayer) > 0.1)
	{
		LastLBYUpdateTime = flServerTime;
		return false;
	}

	if ((LastLBYUpdateTime + 1.0725 - GetOutgoingLatency() < flServerTime) && (LocalPlayer->GetFlags() & FL_ONGROUND))
	{
		if (LastLBYUpdateTime + 1.1 - GetOutgoingLatency() < flServerTime)
		{
			LastLBYUpdateTime = flServerTime;
		}
		return true;
	}
	return false;
}*/

bool NextLBYUpdate()
{
	IClientEntity* LocalPlayer = hackManager.pLocal();

	float flServerTime = (float)(LocalPlayer->GetTickBase()  * Interfaces::Globals->interval_per_tick);

	if (OldLBY != LocalPlayer->GetLowerBodyYaw())
	{
		LBYBreakerTimer++;
		OldLBY = LocalPlayer->GetLowerBodyYaw();
		bSwitch = !bSwitch;
		LastLBYUpdateTime = flServerTime;
	}

	if (CurrentVelocity(LocalPlayer) > 0.1)
	{
		LastLBYUpdateTime = flServerTime;
		return false;
	}


	if ((LastLBYUpdateTime + 1.0 - GetOutgoingLatency() < flServerTime) && (LocalPlayer->GetFlags() & FL_ONGROUND))
	{
		if (LastLBYUpdateTime + 0.9 - GetOutgoingLatency() < flServerTime)
		{
			LastLBYUpdateTime = flServerTime;
		}
		return true;
	}
	return false;
}

namespace AAYaw /*AA-Yaw*/
{

	void Backward(CUserCmd* pCmd)
	{
		pCmd->viewangles.y -= 180.00f;
	}

	void FakeBackward(CUserCmd* pCmd, bool& bSendPacket)
	{
		if (bSendPacket)
		{
			pCmd->viewangles.y += 180;
		}
		else
		{
			pCmd->viewangles.y -= 90;
		}
	}

	void FakeSideways(CUserCmd* pCmd, bool& bSendPacket)
	{

		static bool flip;
		static bool flip2;
		flip = !flip;
		flip2 = !flip2;

		if (pCmd->command_number % 2)
		{
			bSendPacket = false;

			if (flip2)
				pCmd->viewangles.y += 90.5f;

			else
				pCmd->viewangles.y -= 90.5f;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y += 180.0f;
		}
	}

	void FakeSidewaysALT(CUserCmd* pCmd, bool& bSendPacket)
	{

		static bool flip;
		static bool flip2;
		flip = !flip;
		flip2 = !flip2;

		if (pCmd->command_number % 2)
		{
			bSendPacket = false;

			if (flip2)
				pCmd->viewangles.y -= 90.5f;

			else
				pCmd->viewangles.y += 90.5f;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y += 180.0f;
		}
	}
	void FakeJitterSideways(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool Fast = false;
		if (Fast)
		{
			bSendPacket = false; //true angle
			pCmd->viewangles.y -= 90.0;
		}
		else
		{
			bSendPacket = true; //fake angle
			static bool Fast2 = false;
			if (Fast2)
			{
				pCmd->viewangles.y += 75;
			}
			else
			{
				pCmd->viewangles.y += 105;
			}
			Fast2 = !Fast2;
		}
		Fast = !Fast;
	}
	void FakeJitterSidewaysALT(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool Fast = false;
		if (Fast)
		{
			bSendPacket = false; //true angle
			pCmd->viewangles.y += 90.0;
		}
		else
		{
			bSendPacket = true; //fake angle
			static bool Fast2 = false;
			if (Fast2)
			{
				pCmd->viewangles.y -= 75;
			}
			else
			{
				pCmd->viewangles.y -= 105;
			}
			Fast2 = !Fast2;
		}
		Fast = !Fast;
	}
	void PAA(CUserCmd *pCmd, bool &bSendPacket)
	{

		int random = rand() % 100;
		int maxJitter = rand() % (85 - 70 + 1) + 70;
		hackManager.pLocal()->GetLowerBodyYaw() - (rand() % maxJitter);
		if (random < 35 + (rand() % 15))
		{
			bSendPacket = false;
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() - (rand() % maxJitter);;
		}

		else if (random < 85 + (rand() % 15))
		{
			bSendPacket = true;
			pCmd->viewangles.y += hackManager.pLocal()->GetLowerBodyYaw() - (rand() % maxJitter);;
		}
	}

	void FakeSwitch(CUserCmd *pCmd, bool &bSendPacket)
	{
		IClientEntity *pLocal = hackManager.pLocal();

		static bool f_flip = true;
		f_flip = !f_flip;

		if (f_flip)
		{
			bSendPacket = false;
			if (NextLBYUpdate())
			{

				pCmd->viewangles.y -= 87.554f;;
			}
			else
			{

				pCmd->viewangles.y += 93.946f;
			}
		}
		else if (!f_flip)
		{
			bSendPacket = true;
			pCmd->viewangles.y += (rand() % 50 + 145);
		}
	}

	void FakeSwitchALT(CUserCmd *pCmd, bool &bSendPacket)
	{
		IClientEntity *pLocal = hackManager.pLocal();

		static bool f_flip = true;
		f_flip = !f_flip;

		if (f_flip)
		{
			bSendPacket = false;
			if (NextLBYUpdate())
			{

				pCmd->viewangles.y += 87.554f;;
			}
			else
			{

				pCmd->viewangles.y -= 93.946f;
			}
		}
		else if (!f_flip)
		{
			bSendPacket = true;
			pCmd->viewangles.y += (rand() % 50 + 145);
		}
	}

	void TestLBYBreak(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool Fast = false;
		bool flip = true;
		QAngle angle_for_yaw;
		static int counter = 0;
		static int motion = 0;
		int ServerTime = (float)Interfaces::Globals->interval_per_tick * hackManager.pLocal()->GetTickBase() * 1;
		int oldlowerbodyyaw = 0;
		if (Fast)
		{
			static bool f_flip = true;
			f_flip = !f_flip;

			if (f_flip)
			{
				pCmd->viewangles.y -= 170.00f;
				bSendPacket = false;
			}
			else if (!f_flip)
			{
				pCmd->viewangles.y -= 190.00f;
				bSendPacket = false;
			}
		}
		else
		{


			if (flip)
			{
				if (counter % 48 == 0)
					motion++;
				int value = ServerTime % 2;
				switch (value) {

				case 0:pCmd->viewangles.y += 180;
					bSendPacket = true;
				case 1:pCmd->viewangles.y += 90;
					bSendPacket = true;
				}
			}
		}
		Fast = !Fast;
	}

/*	void BolbiBreak(CUserCmd *pCmd, bool &bSendPacket)
	{
		IClientEntity *pLocal = hackManager.pLocal();

		static bool f_flip = true;
		f_flip = !f_flip;

		if (f_flip)
		{
			bSendPacket = false;
			if (NextLBYUpdate())
			{
				pCmd->viewangles.y -= 155;
			}
			else
			{

				pCmd->viewangles.y += 90;
			}
		}
		else if (!f_flip)
		{
			bSendPacket = true;
			if (NextLBYUpdate())
			{
				pCmd->viewangles.y += 90;
			}
			else
			{
				pCmd->viewangles.y -= 155;
			}
		}
	}*/

/*	void BolbiBreakALT(CUserCmd *pCmd)
	{
		IClientEntity *pLocal = hackManager.pLocal();

		if (NextLBYUpdate())
		{
			pCmd->viewangles.y += 155;
		}
		else
		{
			pCmd->viewangles.y -= 90;
		}
	}

	void BolbiBreak(CUserCmd *pCmd)
	{
		IClientEntity *pLocal = hackManager.pLocal();

		if (NextLBYUpdate())
		{
			pCmd->viewangles.y -= 155;
		}
		else
		{
			pCmd->viewangles.y += 90;
		}
	}*/


	void BolbiBreakALT(CUserCmd *pCmd, bool& bSendPacket)
	{
		float randfloat = rand() % -180 + 360;
		IClientEntity *pLocal = hackManager.pLocal();
		static int ChokedPackets = -1;
		ChokedPackets++;
		
		if (NextLBYUpdate())
		{
			pCmd->viewangles.y -= randfloat;
		}
		else
		{
			pCmd->viewangles.y += 90;
		}
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.y -= randfloat;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y += 90;
			ChokedPackets = -1;
		}
	}

	void AntiCorrection(CUserCmd* pCmd)
	{
		Vector newAngle = pCmd->viewangles;

		static int ChokedPackets = -1;
		ChokedPackets++;

		float yaw;
		static int state = 0;
		static bool LBYUpdated = false;

		float flCurTime = Interfaces::Globals->curtime;
		static float flTimeUpdate = 1.09f;
		static float flNextTimeUpdate = flCurTime + flTimeUpdate;
		if (flCurTime >= flNextTimeUpdate) {
			LBYUpdated = !LBYUpdated;
			state = 0;
		}

		if (flNextTimeUpdate < flCurTime || flNextTimeUpdate - flCurTime > 10.f)
			flNextTimeUpdate = flCurTime + flTimeUpdate;

		if (LBYUpdated)
			yaw = 90;
		else
			yaw = -90;

		if (yaw)
			newAngle.y += yaw;

		pCmd->viewangles = newAngle;
	}

	void AntiCorrectionALT(CUserCmd* pCmd)
	{
		Vector newAngle = pCmd->viewangles;

		static int ChokedPackets = -1;
		ChokedPackets++;

		float yaw;
		static int state = 0;
		static bool LBYUpdated = false;

		float flCurTime = Interfaces::Globals->curtime;
		static float flTimeUpdate = 1.09f;
		static float flNextTimeUpdate = flCurTime + flTimeUpdate;
		if (flCurTime >= flNextTimeUpdate) {
			LBYUpdated = !LBYUpdated;
			state = 0;
		}

		if (flNextTimeUpdate < flCurTime || flNextTimeUpdate - flCurTime > 10.f)
			flNextTimeUpdate = flCurTime + flTimeUpdate;

		if (LBYUpdated)
			yaw = -90;
		else
			yaw = 90;

		if (yaw)
			newAngle.y += yaw;

		pCmd->viewangles = newAngle;
	}

	void anti_aim1(CUserCmd* command, bool& send_packet)
	{
		float outgoing_latency = 0.f; //inetchannel
		float current_lby = hackManager.pLocal()->GetLowerBodyYaw();
		float last_lby = 0.f;
		float last_lby_time = 0.f;
		float spin = rand() % 100;
		static bool jitter_switch = false;

		if (current_lby != last_lby || fabs(hackManager.pLocal()->GetVelocity().Length2D()) > 0.1f) {
			last_lby_time = Interfaces::Globals->curtime;
			last_lby = current_lby;
		}
		else {
			if (!send_packet) {
				if (fabs(last_lby < 1.1 - Interfaces::Globals->curtime)) { /* instead of this check u can also do last_lby < 1.1 - curtime*/
					command->viewangles.y += 90.f;
				}
				else {
					command->viewangles.y += 180.f;
				}
			}
			else {
				command->viewangles.y -= 90.f;
			}
			if (send_packet) { /* when faking throw a random spin in*/
				if (current_lby != last_lby) {
					command->viewangles.y += spin;
				}
				else {
					command->viewangles.y += 60.0f;
				}
			}
			else {
				command->viewangles.y += 180.0f;
			}
		}
	}

	void TestBreak(CUserCmd *pCmd, bool& bSendPacket)
	{
		IClientEntity *pLocal = hackManager.pLocal();
		if (!bSendPacket)
		{
			if (NextLBYUpdate())
				pCmd->viewangles.y += 160;
			else
				pCmd->viewangles.y -= 160;
		}
		else
		{
			if (NextLBYUpdate())
			{
				switch (pCmd->tick_count % 4)
				{
				case 0:
					pCmd->viewangles.y = pLocal->GetLowerBodyYaw() - 90;
				case 1:
					pCmd->viewangles.y += 90;
				case 2:
					pCmd->viewangles.y = pLocal->GetLowerBodyYaw() - 90;
				case 3:
					pCmd->viewangles.y += 90;
				}
			}
		}
	}

	void BolbiBreak(CUserCmd *pCmd, bool& bSendPacket)
	{
		float randfloat = rand() % -180 + 360;
		IClientEntity *pLocal = hackManager.pLocal();
		static int ChokedPackets = -1;
		ChokedPackets++;

		if (NextLBYUpdate())
		{
			pCmd->viewangles.y += randfloat;
		}
		else
		{
			pCmd->viewangles.y -= 90;
		}
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.y -= randfloat;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y += 90;
			ChokedPackets = -1;
		}
	}

	void Arizona1(CUserCmd *pCmd, bool &bSendPacket) //Jitter Movement mit static bool Fast :)
	{
		static bool Fast = false;
		if (Fast)
		{
			bSendPacket = false; //true angle
			pCmd->viewangles.y = pCmd->viewangles.y - 134.0;
		}
		else
		{
			bSendPacket = true; //fake angle
			pCmd->viewangles.y = pCmd->viewangles.y - 226.0;
		}
		Fast = !Fast;
	}


	void LBY90switch(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool f_flip = true;
		f_flip = !f_flip;

		if (f_flip)
		{
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + 90.00f;
			bSendPacket = false;
		}
		else if (!f_flip)
		{
			pCmd->viewangles.y += hackManager.pLocal()->GetLowerBodyYaw() - 0.00f;
			bSendPacket = true;
		}
	}

	void TestPLBY(CUserCmd *pCmd, bool &bSendPacket)


	{
		static bool Fast = false;
		bool flip = true;
		QAngle angle_for_yaw;
		static int counter = 0;
		static int motion = 0;
		int ServerTime = (float)Interfaces::Globals->interval_per_tick * hackManager.pLocal()->GetTickBase() * 1;
		int oldlowerbodyyaw = 0;
		if (Fast)
		{
			static bool f_flip = true;
			f_flip = !f_flip;

			if (f_flip)
			{
				pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() - 87.554f;
				bSendPacket = false;
			}
			else if (!f_flip)
			{
				pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + 93.946f;
				bSendPacket = false;
			}
		}
		else
		{


			if (flip)
			{
				if (counter % 48 == 0)
					motion++;
				int value = ServerTime % 2;
				switch (value) {

				case 0:pCmd->viewangles.y += hackManager.pLocal()->GetLowerBodyYaw() - 126.446f;
					bSendPacket = true;
				case 1:pCmd->viewangles.y += hackManager.pLocal()->GetLowerBodyYaw() + 124.874f;
					bSendPacket = true;
				}
			}
		}
		Fast = !Fast;
	}

	void lbyfake(CUserCmd* pCmd, bool& bSendPacket)
	{
		if (hackManager.pLocal()->GetVelocity().Length() > 0)
		{
			int var1;
			int var2;
			float var3;

			pCmd->viewangles.y += 180;
			var1 = rand() % 100;
			var2 = rand() % (10 - 6 + 1) + 10;
			var3 = var2 - (rand() % var2);
			if (var1 < 60 + (rand() % 15))
				pCmd->viewangles.y -= var3;
			else if (var1 < 100 + (rand() % 15))
				pCmd->viewangles.y += var3;
		}
		else {
			static bool lby;
			lby = !lby;

			if (lby)
			{
				bSendPacket = false;
				pCmd->viewangles.y += 195.f;
			}
			else {
				bSendPacket = true;
				int meme = Menu::Window.RageBotTab.AntiAimSpinspeed.GetValue() * 1000.0;
				float CalculatedCurTime_1 = (Interfaces::Globals->curtime * meme);
				pCmd->viewangles.y = CalculatedCurTime_1;
			}
		}
	}
	void testbindedlby(CUserCmd* pCmd, bool& bSendPacket)
	{
		{
			static bool Fast = false;
			bool flip = true;
			QAngle angle_for_yaw;
			static int counter = 0;
			static int motion = 0;
			int ServerTime = (float)Interfaces::Globals->interval_per_tick * hackManager.pLocal()->GetTickBase() * 1;
			int oldlowerbodyyaw = 0;
			if (Fast)
			{
				static bool f_flip = true;
				f_flip = !f_flip;

				if (f_flip)
				{
					pCmd->viewangles.y -= 170.00f;
					bSendPacket = false;
				}
				else if (!f_flip)
				{
					pCmd->viewangles.y -= 190.00f;
					bSendPacket = false;
				}
			}
			else
			{


				if (flip)
				{

					if (counter % 48 == 0)
						motion++;
					int value = ServerTime % 2;
					switch (value) {
						int bindkey3 = Menu::Window.MiscTab.bindkey3.GetKey();
						if (bindkey3 > 0 && GUI.GetKeyState(bindkey3))
							pCmd->viewangles.y += 180;
						bSendPacket = true;
					}
					{
						pCmd->viewangles.y += 90;
						bSendPacket = true;
					}
				}
			}
			Fast = !Fast;
		}
	}
	void FakeSlowSpinBackwards(CUserCmd* pCmd, bool& bSendPacket)
	{
		{

			int random = rand() % 100;
			int random2 = rand() % 1000;
			static bool dir;
			static float current_y = pCmd->viewangles.y;
			IClientEntity* pLocal = hackManager.pLocal();
			float server_time = pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;

			if (bSendPacket) {
				pCmd->viewangles.y = (float)(fmod(server_time / 0.39f * 360.0f, 360.0f));
			}
			else
			{
				if (random == 1) dir = !dir;
				if (dir)
					current_y -= 100.9;
				else
					current_y += 100.9;
				pCmd->viewangles.y = current_y;
				if (random == random2)
					pCmd->viewangles.y -= random;
			}
		}
		pCmd->viewangles.y -= 180;
		bSendPacket = false;
	}




	void RotateYaw(CUserCmd *pCmd, bool& bSendPacket)
	{
		{
			bSendPacket = false;
			int RotateCurrent = rand() % 1 + 2;
			if (RotateCurrent == 1)
			{
				pCmd->viewangles.y = 271.f;
			}
			else if (RotateCurrent == 2)
			{
				pCmd->viewangles.y = -271.f;
			}
		}
		pCmd->viewangles.y -= 180;
		bSendPacket = false;

	}
	void FakeHeadPeja(CUserCmd *pCmd, bool &bSendPacket)
	{
		int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); ++i;
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

		static bool isMoving;
		float PlayerIsMoving = abs(pLocal->GetVelocity().Length());
		if (PlayerIsMoving > 0.1) isMoving = true;
		else if (PlayerIsMoving <= 0.1) isMoving = false;

		int flip = (int)floorf(Interfaces::Globals->curtime / 1.1) % 2;
		static bool bFlipYaw;
		float flInterval = Interfaces::Globals->interval_per_tick;
		float flTickcount = pCmd->tick_count;
		float flTime = flInterval * flTickcount;
		if (std::fmod(flTime, 1) == 0.f)
			bFlipYaw = !bFlipYaw;

		if (PlayerIsMoving <= 0.1)
		{
			if (bSendPacket)
			{
				pCmd->viewangles.y += 180.f;
			}
			else
			{
				if (flip)
				{
					pCmd->viewangles.y += bFlipYaw ? 90.f : -90.f;

				}
				else
				{
					pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + bFlipYaw ? 90.f : -90.f;
				}
			}
		}
		else if (PlayerIsMoving > 0.1)
		{
			if (bSendPacket)
			{
				pCmd->viewangles.y += 180.f;
			}
			else
			{
				pCmd->viewangles.y += 90.f;
			}
		}
	}

	void BackwardsJitter(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool choke = false;
		static bool s = false;
		if (choke)
		{
			bSendPacket = false; //true angle
			pCmd->viewangles.y += (rand() % 50 + 145);
		}
		else
		{
			bSendPacket = true;
			if (s) {
				pCmd->viewangles.y += 90.f;
			}
			else {
				pCmd->viewangles.y -= 90.f;
			}
			s = !s; //fake angle
		}
		choke = !choke;
	}

	void FakeBackwards(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool choke = false;
		static bool s = false;
		if (choke)
		{
			bSendPacket = false; //true angle
			if (s) {
				pCmd->viewangles.y += 90.f;
			}
			else {
				pCmd->viewangles.y -= 90.f;
			}
			s = !s;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y += 180.f; //fake angle
		}
		choke = !choke;
	}

	void FakeSidewaysJitter(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.y -= 90;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y -= 45;
			pCmd->viewangles.y -= 40;
			pCmd->viewangles.y -= 35;
			ChokedPackets = -1;
		}
	}

	void FakeSidewaysJitterALT(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.y += 90;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y += 45;
			pCmd->viewangles.y += 40;
			pCmd->viewangles.y += 35;
			ChokedPackets = -1;
		}
	}



	void try1(CUserCmd *pCmd, bool& bSendPacket)
	{
		static bool SwitchY;
		static bool SwitchY2;
		static bool SwitchY4;

		SwitchY4 = !SwitchY4;
		if (SwitchY4)
		{
			static bool RandX1;
			static bool RandX2;
			if (RandX2)
			{
				pCmd->viewangles.y = -36000264.00000000;
			}
			else if (RandX1)
			{
				pCmd->viewangles.y = 36000264.00000000;
			}
		}

		SwitchY = !SwitchY;
		if (SwitchY)
		{
			pCmd->viewangles.y = 36000264.00000000f;
		}
		SwitchY2 = !SwitchY2;
		if (SwitchY2)
		{
			pCmd->viewangles.y = 36000264.00000000f;
		}
		else if (!SwitchY && !SwitchY2)
		{
			static bool JoelSwitchY;

			JoelSwitchY = !JoelSwitchY;
			if (JoelSwitchY)
			{
				pCmd->viewangles.y = -36000264.00000000;
			}
			else
			{
				pCmd->viewangles.y = 36000264.00000000f;
			}
		}
		else
		{
			static bool SwitchY3;
			SwitchY3 = !SwitchY3;
			if (SwitchY3)
			{
				pCmd->viewangles.y = 36000264.00000000f;
			}
			else
			{
				pCmd->viewangles.y = -36000264.00000000f;
			}
		}
	}
	/*
	{
	static bool Fast = false;
	if (Fast)
	{
	bSendPacket = false; //true angle
	pCmd->viewangles.y += 175.0;
	}
	else
	{
	bSendPacket = true; //fake angle
	static bool Fast2 = false;
	if (Fast2)
	{
	pCmd->viewangles.y += 75;
	}
	else
	{
	pCmd->viewangles.y += 105;
	}
	Fast2 = !Fast2;
	}
	Fast = !Fast;
	}
	*/
	void sidejittermemes(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool Fast = false;
		bool flip = true;
		QAngle angle_for_yaw;
		static int counter = 0;
		static int motion = 0;
		int ServerTime = (float)Interfaces::Globals->interval_per_tick * hackManager.pLocal()->GetTickBase() * 1;
		int oldlowerbodyyaw = 0;
		if (Fast)
		{
			static bool f_flip = true;
			f_flip = !f_flip;

			if (f_flip)
			{
				pCmd->viewangles.y -= 180;
				bSendPacket = false;
			}
			else if (!f_flip)
			{
				{
					int random = rand() % 100;

					// Small chance of starting fowards
					if (random < 98)
						// Look backwards
						pCmd->viewangles.y -= 180;

					// Some gitter
					if (random < 15)
					{
						float change = -70 + (rand() % (int)(140 + 1));
						pCmd->viewangles.y += change;
					}
					if (random == 69)
					{
						float change = -90 + (rand() % (int)(180 + 1));
						pCmd->viewangles.y += change;
					}
				}
				bSendPacket = false;
			}
		}
		else
		{
			if (flip)
			{
				if (counter % 48 == 0)
					motion++;
				int value = ServerTime % 2;
				switch (value) {

				case 0:pCmd->viewangles.y += 75;
					bSendPacket = true;
				case 1:pCmd->viewangles.y += 105;
					bSendPacket = true;
				}
			}
		}
		Fast = !Fast;
	}





	/*
	{
	int bindkey2 = Menu::Window.MiscTab.bindkey2.GetKey();
	int bindkey3 = Menu::Window.MiscTab.bindkey3.GetKey();
	{
	static bool wilupdate;
	static float LastLBYUpdateTime = 0;
	float server_time = hackManager.pLocal()->GetTickBase() * Interfaces::Globals->interval_per_tick;
	if (server_time >= LastLBYUpdateTime)
	{
	LastLBYUpdateTime = server_time + 1.1f;
	wilupdate = true;

	}
	else
	{
	wilupdate = false;
	}
	// LZ Do edgeScan
	if (bSendPacket = false)
	{
	if (bindkey2 > 0 && GUI.GetKeyState(bindkey2)) //b
	pCmd->viewangles.y -= 90.f;
	else if (bindkey3 > 0 && GUI.GetKeyState(bindkey3)) //N
	pCmd->viewangles.y += 90.f;
	}
	else {
	if (bindkey3 > 0 && GUI.GetKeyState(bindkey3)) //b
	pCmd->viewangles.y += 90.f;
	else if (bindkey3 > 0 && GUI.GetKeyState(bindkey3)) //N
	pCmd->viewangles.y -= 90.f;
	}
	if (wilupdate)
	{
	if (bSendPacket = true)
	if (bindkey2 > 0 && GUI.GetKeyState(bindkey2)) //b
	pCmd->viewangles.y -= 180.f;
	else if (bindkey3 > 0 && GUI.GetKeyState(bindkey3)) //N
	pCmd->viewangles.y += 180.f;
	}
	}
	}
	*/
	void fakeanglespin(CUserCmd* pCmd)
	{
		if (pCmd->command_number % 2) {
			pCmd->viewangles.y = pCmd->viewangles.y + 36000180.0;
		}
		float CalculatedCurTime_2 = (Interfaces::Globals->curtime * 5000.0);
		pCmd->viewangles.y = CalculatedCurTime_2 + 36000000.0;

	}
	

	void FakeEdge(CUserCmd *pCmd, bool &bSendPacket)
	{
	IClientEntity* pLocal = hackManager.pLocal();

	Vector vEyePos = pLocal->GetOrigin() + pLocal->GetViewOffset();

	CTraceFilter filter;
	filter.pSkip = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	for (int y = 0; y < 360; y++)
	{
	Vector qTmp(10.0f, pCmd->viewangles.y, 0.0f);
	qTmp.y += y;

	if (qTmp.y > 180.0)
	qTmp.y -= 360.0;
	else if (qTmp.y < -180.0)
	qTmp.y += 360.0;

	GameUtils::NormaliseViewAngle(qTmp);

	Vector vForward;

	VectorAngles(qTmp, vForward);

	float fLength = (19.0f + (19.0f * sinf(DEG2RAD(10.0f)))) + 7.0f;
	vForward *= fLength;

	trace_t tr;

	Vector vTraceEnd = vEyePos + vForward;

	Ray_t ray;

	ray.Init(vEyePos, vTraceEnd);
	Interfaces::Trace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &tr);

	if (tr.fraction != 1.0f)
	{
	Vector angles;

	Vector vNegative = Vector(tr.plane.normal.x * -1.0f, tr.plane.normal.y * -1.0f, tr.plane.normal.z * -1.0f);

	VectorAngles(vNegative, angles);

	GameUtils::NormaliseViewAngle(angles);

	qTmp.y = angles.y;

	GameUtils::NormaliseViewAngle(qTmp);

	trace_t trLeft, trRight;

	Vector vLeft, vRight;
	VectorAngles(qTmp + Vector(0.0f, 30.0f, 0.0f), vLeft);
	VectorAngles(qTmp + Vector(0.0f, 30.0f, 0.0f), vRight);

	vLeft *= (fLength + (fLength * sinf(DEG2RAD(30.0f))));
	vRight *= (fLength + (fLength * sinf(DEG2RAD(30.0f))));

	vTraceEnd = vEyePos + vLeft;

	ray.Init(vEyePos, vTraceEnd);
	Interfaces::Trace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &trLeft);

	vTraceEnd = vEyePos + vRight;

	ray.Init(vEyePos, vTraceEnd);
	Interfaces::Trace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &trRight);

	if ((trLeft.fraction == 1.0f) && (trRight.fraction != 1.0f))
	qTmp.y -= 90.f;
	else if ((trLeft.fraction != 1.0f) && (trRight.fraction == 1.0f))
	qTmp.y += 90.f;

	if (qTmp.y > 180.0)
	qTmp.y -= 360.0;
	else if (qTmp.y < -180.0)
	qTmp.y += 360.0;

	pCmd->viewangles.y = qTmp.y;

	int offset = Menu::Window.RageBotTab.AntiAimOffset.GetValue();

	static int ChokedPackets = -1;
	ChokedPackets++;
	if (ChokedPackets < 1)
	{
	bSendPacket = false; // +=180?
	}
	else
	{
	bSendPacket = true;
	pCmd->viewangles.y -= offset;
	ChokedPackets = -1;
	}
	return;
	}
	}
	pCmd->viewangles.y += 360.0f;
	}
	

	void comeback(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool first;
		static bool lowerbody;
		lowerbody = !lowerbody;
		if (lowerbody)
		{
			if (first)
			{
				first = false;
				pCmd->viewangles.y += 180.0f;
				bSendPacket = true;
			}
			else
			{
				{
					bSendPacket = true; //fake angle
					static bool Fast2 = false;
					if (Fast2)
					{
						pCmd->viewangles.y += 75;
					}
					else
					{
						pCmd->viewangles.y += 105;
					}
					Fast2 = !Fast2;
				}
			}
		}
		else
		{
			if (lowerbody)
			{
				first = false;
				pCmd->viewangles.y -= 180.0f;
				bSendPacket = true;
			}
			else
			{
				bSendPacket = true; //fake angle
				static bool Fast2 = false;
				if (Fast2)
				{
					pCmd->viewangles.y -= 75;
				}
				else
				{
					pCmd->viewangles.y -= 105;
				}
				Fast2 = !Fast2;
			}
		}
	}

	enum ADAPTIVE_SIDE {
		ADAPTIVE_UNKNOWN,
		ADAPTIVE_LEFT,
		ADAPTIVE_RIGHT
	};

	enum ADAPTIVE_SIDE2 {
		ADAPTIVE_UNKNOWN2,
		ADAPTIVE_LEFT2,
		ADAPTIVE_RIGHT2
	};

	void adaptive2(CUserCmd * pCmd, bool& bSendPacket) {
		auto fov_to_player = [](Vector view_offset, Vector view, IClientEntity* m_entity, int hitbox)
		{
			CONST FLOAT MaxDegrees = 180.0f;
			Vector Angles = view;
			Vector Origin = view_offset;
			Vector Delta(0, 0, 0);
			Vector Forward(0, 0, 0);
			AngleVectors3(Angles, &Forward);
			Vector AimPos = GetHitboxPosition(m_entity, hitbox);
			VectorSubtract(AimPos, Origin, Delta);
			Normalize(Delta, Delta);
			FLOAT DotProduct = Forward.Dot(Delta);
			return (acos(DotProduct) * (MaxDegrees / PI));
		};

		auto m_local = hackManager.pLocal();

		int target = -1;
		float mfov = 20;

		Vector viewoffset = m_local->GetOrigin() + m_local->GetViewOffset();
		Vector view; Interfaces::Engine->GetViewAngles(view);

		for (int i = 0; i < Interfaces::Engine->GetMaxClients(); i++) {
			IClientEntity* m_entity = Interfaces::EntList->GetClientEntity(i);

			if (m_entity && m_entity->IsDormant() == false && m_entity->IsAlive() && m_entity->GetIndex() != hackManager.pLocal()->GetIndex()) {

				float fov = fov_to_player(viewoffset, view, m_entity, 0);
				if (fov < mfov) {
					mfov = fov;
					target = i;
				}
			}
		}

		ADAPTIVE_SIDE2 side = ADAPTIVE_UNKNOWN2;

		Vector at_target_angle;

		if (target) {
			auto m_entity = Interfaces::EntList->GetClientEntity(target);

			if (m_entity && m_entity->IsDormant() == false && m_entity->IsAlive() && m_entity->GetIndex() != hackManager.pLocal()->GetIndex()) {
				Vector pos_enemy;
				if (Render::WorldToScreen(m_entity->GetOrigin(), pos_enemy)) {
					CalcAngle(m_local->GetOrigin(), m_entity->GetOrigin(), at_target_angle);

					POINT mouse = GUI.GetMouse();

					if (mouse.x > pos_enemy.x) side = ADAPTIVE_RIGHT2;
					else if (mouse.x < pos_enemy.x) side = ADAPTIVE_LEFT2;
					else side = ADAPTIVE_UNKNOWN2;
				}
			}
		}

			if (side == ADAPTIVE_RIGHT) {
				pCmd->viewangles.y = at_target_angle.y + 90;
			}
			else if (side == ADAPTIVE_LEFT) {
				pCmd->viewangles.y = at_target_angle.y - 90;
			}

		if (side == ADAPTIVE_UNKNOWN) {
			pCmd->viewangles.y -= 180;
		}
	}


	void adaptive(CUserCmd * pCmd, bool& bSendPacket) {
		auto fov_to_player = [](Vector view_offset, Vector view, IClientEntity* m_entity, int hitbox)
		{
			CONST FLOAT MaxDegrees = 180.0f;
			Vector Angles = view;
			Vector Origin = view_offset;
			Vector Delta(0, 0, 0);
			Vector Forward(0, 0, 0);
			AngleVectors3(Angles, &Forward);
			Vector AimPos = GetHitboxPosition(m_entity, hitbox);
			VectorSubtract(AimPos, Origin, Delta);
			Normalize(Delta, Delta);
			FLOAT DotProduct = Forward.Dot(Delta);
			return (acos(DotProduct) * (MaxDegrees / PI));
		};

		auto m_local = hackManager.pLocal();

		int target = -1;
		float mfov = 20;

		Vector viewoffset = m_local->GetOrigin() + m_local->GetViewOffset();
		Vector view; Interfaces::Engine->GetViewAngles(view);

		for (int i = 0; i < Interfaces::Engine->GetMaxClients(); i++) {
			IClientEntity* m_entity = Interfaces::EntList->GetClientEntity(i);

			if (m_entity && m_entity->IsDormant() == false && m_entity->IsAlive() && m_entity->GetIndex() != hackManager.pLocal()->GetIndex()) {

				float fov = fov_to_player(viewoffset, view, m_entity, 0);
				if (fov < mfov) {
					mfov = fov;
					target = i;
				}
			}
		}

		ADAPTIVE_SIDE side = ADAPTIVE_UNKNOWN;

		Vector at_target_angle;

		if (target) {
			auto m_entity = Interfaces::EntList->GetClientEntity(target);

			if (m_entity && m_entity->IsDormant() == false && m_entity->IsAlive() && m_entity->GetIndex() != hackManager.pLocal()->GetIndex()) {
				Vector pos_enemy;
				if (Render::WorldToScreen(m_entity->GetOrigin(), pos_enemy)) {
					CalcAngle(m_local->GetOrigin(), m_entity->GetOrigin(), at_target_angle);

					POINT mouse = GUI.GetMouse();

					if (mouse.x > pos_enemy.x) side = ADAPTIVE_RIGHT;
					else if (mouse.x < pos_enemy.x) side = ADAPTIVE_LEFT;
					else side = ADAPTIVE_UNKNOWN;
				}
			}
		}

			if (side == ADAPTIVE_RIGHT) {
				pCmd->viewangles.y = at_target_angle.y - 90;
			}
			else if (side == ADAPTIVE_LEFT) {
				pCmd->viewangles.y = at_target_angle.y + 90;
			}


		if (side == ADAPTIVE_UNKNOWN) {
			pCmd->viewangles.y -= 180;
		}
	}

	void lbynibybraekr(CUserCmd* pCmd, bool &bSendPacket)
	{
		static float StartLbyBreaker;
		static float LastLBYUpdateTime;
		static float lbyDelta;
		static int LBYBreakerTimer1 = 1;
		float realAngle = pCmd->viewangles.y += 180.0f;;
		float fakeAngle;
		float lby;
		static int random;
		random = rand() % 2;


		// FAKE ANGLE ROFL ROFL ROFL
		if (random == 1) {
			fakeAngle = pCmd->viewangles.y -= 90.0f;

		}
		else {
			fakeAngle = pCmd->viewangles.y += 90.0f;

		}


		// ROFL ROFL I KNOW
		while (realAngle < -180.0f)
			realAngle += 360.0f;

		while (realAngle > 180.0f)
			realAngle -= 360.0f;

		while (fakeAngle < -180.0f)
			fakeAngle += 360.0f;

		while (fakeAngle > 180.0f)
			fakeAngle -= 360.0f;


		// Real Angle should always be more then 35° FAKE Angle & LBY ; 45 because most bruteforce ur lby + 40°
		if (fabsf(realAngle - lby) > 45 && fabsf(realAngle - fakeAngle) > 45) {
			realAngle += realAngle;
		}

		if (fabsf(realAngle - lby) < 45 || fabsf(realAngle - fakeAngle) < 45) {
			realAngle += 45;
		}

		// IF LBY Update from Proxy then fake Angle "Peek"... like a fagg

		if (StartLbyBreaker != LBYBreakerTimer1)
		{
			bSendPacket = true;
			StartLbyBreaker = LBYBreakerTimer1;
			pCmd->viewangles.y += fakeAngle;
		}
		else
		{
			pCmd->viewangles.y += realAngle;
		}
	}

	void fake2(CUserCmd* pCmd, bool& bSendPacket)
	{
#define RandomInt(min, max) (rand() % (max - min + 1) + min)
		int flip = (int)floorf(Interfaces::Globals->curtime / 3) % 2;
		static bool lowerbody;
		lowerbody = !lowerbody;
		if (lowerbody)
		{
			bSendPacket = true;
			int rand2;
			static bool flipit;
			rand2 = RandomInt(1, 100);
			if (flipit) {
				if (rand2 < 2) {
					pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() - 260.27 + flip * hackManager.pLocal()->GetLowerBodyYaw() - 184.86;
				}
				else
				{
					pCmd->viewangles.y += flip * hackManager.pLocal()->GetLowerBodyYaw() - 260.27;
				}
				flipit = false;
			}
			else {
				if (rand2 < 2) {
					pCmd->viewangles.y += -260.27 + flip * 184.86;
				}
				else
				{
					pCmd->viewangles.y += flip * 260.27;
				}
				flipit = true;
			}
			lowerbody = false;
		}
		else {
			bSendPacket = false;
			pCmd->viewangles.y += 180;
			lowerbody = true;
		}
	}
	void pLBY(CUserCmd* pCmd, bool& bSendPacket)
	{
		static bool ySwitch;
		static bool jbool;
		static bool jboolt;
		ySwitch = !ySwitch;
		jbool = !jbool;
		jboolt = !jbool;
		if (ySwitch)
		{
			if (jbool)
			{
				if (jboolt)
				{
					pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() - 87.554f;
					bSendPacket = false;
				}
				else
				{
					pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() + 93.946f;
					bSendPacket = false;
				}
			}
			else
			{
				if (jboolt)
				{
					pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() - 126.446f;
					bSendPacket = false;
				}
				else
				{
					pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() + 124.874f;
					bSendPacket = false;
				}
			}
		}
		else
		{
			pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw();
			bSendPacket = true;
		}
	}
	/*
	int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); ++i;
	IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
	IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	static bool isMoving;
	float PlayerIsMoving = abs(pLocal->GetVelocity().Length());
	if (PlayerIsMoving > 0.1) isMoving = true;
	else if (PlayerIsMoving <= 0.1) isMoving = false;
	*/

	void Paatest(CUserCmd *pCmd, bool &bSendPacket)
	{
		int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); ++i;
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

		static bool isMoving;
		float PlayerIsMoving = abs(pLocal->GetVelocity().Length());
		if (PlayerIsMoving > 0.1) isMoving = true;
		else if (PlayerIsMoving <= 0.1) isMoving = false;

		if (PlayerIsMoving <= 0.1)
		{
			static bool Fast = false;
			bool flip = true;
			QAngle angle_for_yaw;
			static int counter = 0;
			static int motion = 0;
			int ServerTime = (float)Interfaces::Globals->interval_per_tick * hackManager.pLocal()->GetTickBase() * 1;
			int oldlowerbodyyaw = 0;
			if (Fast)
			{
				static bool f_flip = true;
				f_flip = !f_flip;

				if (f_flip)
				{
					pCmd->viewangles.y -= 170.00f;
					bSendPacket = false;
				}
				else if (!f_flip)
				{
					pCmd->viewangles.y -= 190.00f;
					bSendPacket = false;
				}
			}
			else
			{


				if (flip)
				{
					if (counter % 48 == 0)
						motion++;
					int value = ServerTime % 2;
					switch (value) {

					case 0:pCmd->viewangles.y += 180;
						bSendPacket = true;
					case 1:pCmd->viewangles.y += 90;
						bSendPacket = true;
					}
				}
			}
			Fast = !Fast;
		}
		if (PlayerIsMoving > 0.1)
		{
			static bool Fast = false;
			if (Fast)
			{
				bSendPacket = false; //true angle
				pCmd->viewangles.y += 175.0;
			}
			else
			{
				bSendPacket = true; //fake angle
				static bool Fast2 = false;
				if (Fast2)
				{
					pCmd->viewangles.y += 75;
				}
				else
				{
					pCmd->viewangles.y += 105;
				}
				Fast2 = !Fast2;
			}
			Fast = !Fast;
		}
	}
	void skeet(CUserCmd* pCmd, bool& bSendPacket)
	{
		static bool pJitter;
		if (pJitter <= 1)
		{
			pCmd->viewangles.x = 88.000000;
			pCmd->viewangles.y += 135.000000;
		}
		else if (pJitter > 1 && pJitter <= 3)
		{
			pCmd->viewangles.x = 88.000000;
			pCmd->viewangles.y += 22.000000;
		}
		if (pCmd->tick_count % 3)
		{
			bSendPacket = false;
			if (pJitter <= 1)
			{
				pCmd->viewangles.y += 60.000000;
				pJitter += 1;
			}
			else if (pJitter > 1 && pJitter <= 3)
			{
				pCmd->viewangles.y -= 55.000000;
				pJitter += 1;
			}
			else
			{
				pJitter = 0;
			}
		}
		else
		{
			bSendPacket = true;
		}
	}
	void aagud(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool f_flip = true;
		f_flip = !f_flip;
		static int fakeaa = pCmd->viewangles.y = -90 + bSendPacket;


		if (f_flip)
		{
			pCmd->viewangles.y += 90;
			bSendPacket = false;
		}
		else if (!f_flip)
		{
			pCmd->viewangles.y -= 50;
			bSendPacket = true;
		}
		else if (fakeaa)
		{
			pCmd->viewangles.y = fakeaa;
			bSendPacket = false;
		}
		else if (fakeaa = bSendPacket)
		{
			fakeaa;
		}
		else if (fakeaa = false)
		{
			pCmd->viewangles.z = 50.f;
		}

	}
	void fake90p100yaw(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool f_flip = true;
		f_flip = !f_flip;

		if (f_flip)
		{
			QAngle v37 = pCmd->viewangles;
			pCmd->viewangles.y = v37.y + 90.0;
			bSendPacket = true;
		}
		else if (!f_flip)
		{
			QAngle v37 = pCmd->viewangles;
			pCmd->viewangles.y = v37.y - 90.0;
			bSendPacket = false;
		}
		else if (f_flip)
		{
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + 90;
			bSendPacket = true;
		}
		else if (!f_flip)
		{
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() - 90;
			bSendPacket = false;
		}
	}
	void SideFake2(CUserCmd *pCmd, bool& bSendPacket)
	{
		bool faked123;
		if (faked123 = true)
		{
			bSendPacket = true; //fake aa = obviously some aa with some + shit xddd 
			pCmd->viewangles.y += 90 + rand() % 10; //pAntiAWaa :33 + non ut <3333333333333333333333333333333333333333333333333333333
			faked123 = false;
		}
		else if (Menu::Window.MiscTab.FakeLagEnable.GetState())
		{
			//if fakelag is on then dont care about anything else just put on backwards with some -
			pCmd->viewangles.y = 180 - rand() % 7, 5;
		}
		else
		{ //but when fakelag is off then put on real angle jitter 
			bSendPacket = false;
			bool pJitter;
			if (pJitter = true)
			{
				pCmd->viewangles.y = -70 + 60;
				pJitter = false;
			}
			else
			{
				pCmd->viewangles.y = 69 - rand() % 15;
				pJitter = true;
			}
			faked123 = true;
		}
	}
/*	void fakehead(CUserCmd *pCmd, bool& bSendPacket)
	{
		IClientEntity* pLocal = hackManager.pLocal();

		Vector vEyePos = pLocal->GetOrigin() + pLocal->GetViewOffset();

		CTraceFilter filter;
		filter.pSkip = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

		for (int y = 0; y < 360; y++)
		{
			Vector qTmp(10.0f, pCmd->viewangles.y, 0.0f);
			qTmp.y += y;

			if (qTmp.y > 180.0)
				qTmp.y -= 360.0;
			else if (qTmp.y < -180.0)
				qTmp.y += 360.0;

			GameUtils::NormaliseViewAngle(qTmp);

			Vector vForward;

			VectorAngles(qTmp, vForward);

			float fLength = (19.0f + (19.0f * sinf(DEG2RAD(10.0f)))) + 7.0f;
			vForward *= fLength;

			trace_t tr;

			Vector vTraceEnd = vEyePos + vForward;

			Ray_t ray;

			ray.Init(vEyePos, vTraceEnd);
			Interfaces::Trace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &tr);

			if (tr.fraction != 1.0f)
			{
				Vector angles;

				Vector vNegative = Vector(tr.plane.normal.x * -1.0f, tr.plane.normal.y * -1.0f, tr.plane.normal.z * -1.0f);

				VectorAngles(vNegative, angles);

				GameUtils::NormaliseViewAngle(angles);

				qTmp.y = angles.y;

				GameUtils::NormaliseViewAngle(qTmp);

				trace_t trLeft, trRight;

				Vector vLeft, vRight;
				VectorAngles(qTmp + Vector(0.0f, 30.0f, 0.0f), vLeft);
				VectorAngles(qTmp + Vector(0.0f, 30.0f, 0.0f), vRight);

				vLeft *= (fLength + (fLength * sinf(DEG2RAD(30.0f))));
				vRight *= (fLength + (fLength * sinf(DEG2RAD(30.0f))));

				vTraceEnd = vEyePos + vLeft;

				ray.Init(vEyePos, vTraceEnd);
				Interfaces::Trace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &trLeft);

				vTraceEnd = vEyePos + vRight;

				ray.Init(vEyePos, vTraceEnd);
				Interfaces::Trace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &trRight);


				int offset = Menu::Window.AntiAimTab.AntiAimOffset.GetValue();

				{
					static bool Fast = false;
					bool flip = true;
					QAngle angle_for_yaw;
					static int counter = 0;
					static int motion = 0;
					int ServerTime = (float)Interfaces::Globals->interval_per_tick * hackManager.pLocal()->GetTickBase() * 1;
					int oldlowerbodyyaw = 0;
					if (Fast)
					{
						static bool f_flip = true;
						f_flip = !f_flip;

						if (f_flip)
						{
							pCmd->viewangles.y -= 170.00f;
							bSendPacket = false;
						}
						else if (!f_flip)
						{
							pCmd->viewangles.y -= 190.00f;
							bSendPacket = false;
						}
					}
					else
					{


						if (flip)
						{
							if (counter % 48 == 0)
								motion++;
							int value = ServerTime % 2;
							switch (value) {

							case 0:pCmd->viewangles.y += 180;
								bSendPacket = true;
							case 1:pCmd->viewangles.y += 90;
								bSendPacket = true;
							}
						}
					}
					Fast = !Fast;
				}
			}
		}
		pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() * hackManager.pLocal()->GetLowerBodyYaw() ? -90.f : 90.f;
	}*/

	void FakeBackJitterxD(CUserCmd* pCmd, bool& bSendPacket)
	{
		IClientEntity* pLocal = hackManager.pLocal();

		if (bSendPacket)
		{
			pCmd->viewangles.y += pLocal->GetLowerBodyYaw() + 35;
		}
		else
		{
				float change = 0;
				int random = rand() % 100;

				if (random < 98)

					pCmd->viewangles.y -= 179;

				if (random < 15)
				{
					float change = -40 + (rand() % (int)(140 + 1));
					pCmd->viewangles.y += change;
				}
				if (random == 69)
				{
					float change = -89 + (rand() % (int)(180 + 1));
					pCmd->viewangles.y += change;
				}
		}
	}
	void FakeTwoStep(CUserCmd* pCmd, bool& bSendPacket) {

		IClientEntity* pLocal = hackManager.pLocal();
		static bool bFlipYaw;
		float flInterval = Interfaces::Globals->interval_per_tick;
		float flTickcount = pCmd->tick_count;
		float flTime = flInterval * flTickcount;
		float server_time = pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;
		if (std::fmod(flTime, 10) == 4.f)
			bFlipYaw = !bFlipYaw;

		if (bSendPacket)
			pCmd->viewangles.y -= (float)(fmod(server_time / 10.45f * 39.0f, 180.0f));
		else
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + bFlipYaw ? -90.f : 90.f;
	}

	void testing(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool Fast = false;
		bool flip = true;
		QAngle angle_for_yaw;
		static int counter = 0;
		static int motion = 0;
		int ServerTime = (float)Interfaces::Globals->interval_per_tick * hackManager.pLocal()->GetTickBase() * 1;
		int oldlowerbodyyaw = 0;
		if (Fast)
		{
			static bool f_flip = true;
			f_flip = !f_flip;

			if (f_flip)
			{
				pCmd->viewangles.y -= pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + 30;
				bSendPacket = false;
			}
			else if (!f_flip)
			{
				pCmd->viewangles.y -= pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() - 30;
				bSendPacket = false;
			}
		}
		else
		{


			if (flip)
			{
				if (counter % 48 == 0)
					motion++;
				int value = ServerTime % 2;
				switch (value) {

				case 0:pCmd->viewangles.y += pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() * 60;
					bSendPacket = true;
				case 1:pCmd->viewangles.y += pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() * 30;
					bSendPacket = true;
				}
			}
		}
		Fast = !Fast;
	}


	void Jitter(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool ySwitch = false;
		int veloc = hackManager.pLocal()->GetVelocity().Length();
		if (ySwitch)
			pCmd->viewangles.y -= 90 * veloc;
		else
			pCmd->viewangles.y += 90 * veloc;

		ySwitch = !ySwitch;
	}

	void FadedJitter(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool ySwitch = false;
		int veloc = hackManager.pLocal()->GetVelocity().Length();
		if (bSendPacket)
		{
			if (ySwitch)
				pCmd->viewangles.y -= 70 * (veloc / 3);
			else
				pCmd->viewangles.y -= 110 * (veloc / 3);

			ySwitch = !ySwitch;
		}
		else
		{
			if (ySwitch)
				pCmd->viewangles.y += 70 * (veloc / 3);
			else
				pCmd->viewangles.y += 110 * (veloc / 3);

			ySwitch = !ySwitch;
		}
	}

	void FadedJitterAlt(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool ySwitch = false;
		int veloc = hackManager.pLocal()->GetVelocity().Length();
		if (bSendPacket)
		{
			if (ySwitch)
				pCmd->viewangles.y += 70 * (veloc / 3);
			else
				pCmd->viewangles.y += 110 * (veloc / 3);

			ySwitch = !ySwitch;
		}
		else
		{
			if (ySwitch)
				pCmd->viewangles.y -= 70 * (veloc / 3);
			else
				pCmd->viewangles.y -= 110 * (veloc / 3);

			ySwitch = !ySwitch;
		}
	}
	/*
	{
	static bool Fast = false;
	bool flip = true;
	QAngle angle_for_yaw;
	static int counter = 0;
	static int motion = 0;
	int ServerTime = (float)Interfaces::Globals->interval_per_tick * hackManager.pLocal()->GetTickBase() * 1;
	int oldlowerbodyyaw = 0;
	if (Fast)
	{
	static bool f_flip = true;
	f_flip = !f_flip;

	if (f_flip)
	{
	pCmd->viewangles.y -= 170.00f;
	bSendPacket = false;
	}
	else if (!f_flip)
	{
	pCmd->viewangles.y -= 190.00f;
	bSendPacket = false;
	}
	}
	else
	{


	if (flip)
	{
	if (counter % 48 == 0)
	motion++;
	int value = ServerTime % 2;
	switch (value) {

	case 0:pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() + 90.00f;
	bSendPacket = true;
	case 1:pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() - 100.00f;
	bSendPacket = true;
	}

	}
	}
	Fast = !Fast;
	}
	*/
	void Jitter1(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool Fast = false;
		bool flip = true;
		QAngle angle_for_yaw;
		static int counter = 0;
		static int motion = 0;
		int ServerTime = (float)Interfaces::Globals->interval_per_tick * hackManager.pLocal()->GetTickBase() * 1;
		int oldlowerbodyyaw = 0;
		if (Fast)
		{
			static bool f_flip = true;
			f_flip = !f_flip;

			if (f_flip)
			{
				pCmd->viewangles.y -= 170.00f;
				bSendPacket = false;
			}
			else if (!f_flip)
			{
				pCmd->viewangles.y -= 190.00f;
				bSendPacket = false;
			}
		}
		else
		{


			if (flip)
			{
				if (counter % 48 == 0)
					motion++;
				int value = ServerTime % 2;
				switch (value) {

				case 0:pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() + 90.00f;
					bSendPacket = true;
				case 1:pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() - 180.00f;
					bSendPacket = true;
				}

			}
		}
		Fast = !Fast;
	}

	void Autistic(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool f_flip = true;
		f_flip = !f_flip;

		if (f_flip)
		{
			bSendPacket = true; //fake angle
			int r1 = 90 - rand() % 10;
			static float current_y = pCmd->viewangles.y;
			current_y += r1;
			pCmd->viewangles.y = current_y;

		}
		else if (!f_flip)
		{
			bSendPacket = true;
			int r1 = 179 - rand() % 10;
			static float current_y = pCmd->viewangles.y;
			current_y += r1;
			pCmd->viewangles.y = current_y;

		}
	}
	void TickOverRide(CUserCmd *pCmd, bool &bSendPacket)
	{
		bool jitter = false;
		int Add = 0;

		if (bSendPacket)
		{
			if (jitter)
				Add = -90;
			else
				Add = 90;
		}
		else
		{
			float flRandom = rand() % 5 + 1.f;
			switch (pCmd->tick_count % 4)
			{
			case 0:
				Add = -170.f - flRandom;
				break;
			case 3:
			case 1:
				Add = 180.f;
				break;
			case 2:
				Add = 170.f + flRandom;
				break;
			}
		}
	}

	void SideJitterALT(CUserCmd *pCmd)
	{
		static bool Fast2 = false;
		if (Fast2)
		{
			pCmd->viewangles.y -= 75;
		}
		else
		{
			pCmd->viewangles.y -= 105;
		}
		Fast2 = !Fast2;
	}

	void SideJitter(CUserCmd *pCmd)
	{
		static bool Fast2 = false;
		if (Fast2)
		{
			pCmd->viewangles.y += 75;
		}
		else
		{
			pCmd->viewangles.y += 105;
		}
		Fast2 = !Fast2;
	}

	void NewBackJitter(CUserCmd *pCmd)
	{
		static bool Fast2 = false;
		if (Fast2)
		{
			pCmd->viewangles.y += 165;
		}
		else
		{
			pCmd->viewangles.y -= 165;
		}
		Fast2 = !Fast2;
	}

	void SlowSpin(CUserCmd *pCmd)
	{
		static int y2 = -179;
		int spinBotSpeedFast = Menu::Window.RageBotTab.SpinSpeed.GetValue() / 1.618033988749895f;

		y2 += spinBotSpeedFast;

		if (y2 >= 179)
			y2 = -179;

		pCmd->viewangles.y = y2;
	}

	void LowerbodyNew(CUserCmd *pCmd)
	{
		float randfloat = rand() % -180 + 360;
		IClientEntity *pLocal = hackManager.pLocal();

		if (NextLBYUpdate())
		{
			pCmd->viewangles.y += 90;
		}
		else
		{
			pCmd->viewangles.y -= 90;
		}
	}

	void Spinbot(CUserCmd* pCmd) {

		int random = rand() % 100;
		int random2 = rand() % 1000;

		static bool dir;
		static float current_y = pCmd->viewangles.y;

		if (random == 1) dir = !dir;

		if (dir)
			current_y += 100.9;
		else
			current_y -= 100.9;

		pCmd->viewangles.y = current_y;

		if (random == random2)
			pCmd->viewangles.y += random;
	}

	void LowerbodyNewALT(CUserCmd *pCmd)
	{
		float randfloat = rand() % -180 + 360;
		IClientEntity *pLocal = hackManager.pLocal();

		if (NextLBYUpdate())
		{
			pCmd->viewangles.y -= 90;
		}
		else
		{
			pCmd->viewangles.y += 90;
		}
	}

	void tolong(CUserCmd *pCmd, bool &bSendPacket)
	{
		static float StoredYaw = 0;
		IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
		static bool flip = false;
		static bool flip2 = false;
		float flip2angle = 0.f;
		if (pLocal->GetLowerBodyYaw() != StoredYaw) //If lowerbody updates and is different than previous change the antiaim
			flip2 = !flip2;
		if (flip2)
			flip2angle = 180.f;
		else
			flip2angle = 0.f;


		if (flip)
		{
			pCmd->viewangles.y += 90.0 + flip2angle;
			bSendPacket = false;
		}
		else
		{
			pCmd->viewangles.y += -90.0 + flip2angle;
			bSendPacket = true;
		}
		if (flip)
		{
			pCmd->viewangles.y += 100.0 + flip2angle;
			bSendPacket = false;
		}
		else
		{
			pCmd->viewangles.y += -100.0 + flip2angle;
			bSendPacket = true;
		}
		if (flip)
		{
			pCmd->viewangles.y += 110.0 + flip2angle;
			bSendPacket = false;
		}
		else
		{
			pCmd->viewangles.y += -110.0 + flip2angle;
			bSendPacket = true;
		}
		if (flip)
		{
			pCmd->viewangles.y += 120.0 + flip2angle;
			bSendPacket = false;
		}
		else
		{
			pCmd->viewangles.y += -120.0 + flip2angle;
			bSendPacket = true;
		}
		if (flip)
		{
			pCmd->viewangles.y += 130.0 + flip2angle;
			bSendPacket = false;
		}
		else
		{
			pCmd->viewangles.y += -130.0 + flip2angle;
			bSendPacket = true;
		}
		if (flip)
		{
			pCmd->viewangles.y += 310.0 + flip2angle;
			bSendPacket = false;
		}
		else
		{
			pCmd->viewangles.y += -310.0 + flip2angle;
			bSendPacket = true;
		}
		if (flip)
		{
			pCmd->viewangles.y += 300.0 + flip2angle;
			bSendPacket = false;
		}
		else
		{
			pCmd->viewangles.y += -300.0 + flip2angle;
			bSendPacket = true;
		}
		if (flip)
		{
			pCmd->viewangles.y += 290.0 + flip2angle;
			bSendPacket = false;
		}
		else
		{
			pCmd->viewangles.y += -290.0 + flip2angle;
			bSendPacket = true;
		}
		if (flip)
		{
			pCmd->viewangles.y += 280.0 + flip2angle;
			bSendPacket = false;
		}
		else
		{
			pCmd->viewangles.y += -280.0 + flip2angle;
			bSendPacket = true;
		}
		if (flip)
		{
			pCmd->viewangles.y += 270.0 + flip2angle;
			bSendPacket = false;
		}
		else
		{
			pCmd->viewangles.y += -270.0 + flip2angle;
			bSendPacket = true;
		}
		StoredYaw = pLocal->GetLowerBodyYaw();
		flip = !flip;
	}


}
bool CanFire()
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

void CRageBot::aimAtPlayer(CUserCmd *pCmd)
{
	IClientEntity* pLocal = hackManager.pLocal();

	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());

	if (!pLocal || !pWeapon)
		return;

	Vector eye_position = pLocal->GetEyePosition();

	float best_dist = pWeapon->GetCSWpnData()->range;

	IClientEntity* target = nullptr;

	for (int i = 0; i < Interfaces::Engine->GetMaxClients(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			if (Globals::TargetID != -1)
				target = Interfaces::EntList->GetClientEntity(Globals::TargetID);
			else
				target = pEntity;

			Vector target_position = target->GetEyePosition();

			float temp_dist = eye_position.DistTo(target_position);

			if (best_dist > temp_dist)
			{
				best_dist = temp_dist;
				CalcAngle(eye_position, target_position, pCmd->viewangles);
			}
		}
	}
}


static bool dir = false;
static bool back = false;
void DoRealAA(CUserCmd* pCmd, IClientEntity* pLocal, bool& bSendPacket)
{
	int y2 = 90;
	
	if (GetAsyncKeyState(Menu::Window.RageBotTab.keybasedaaleft.GetKey())) // right
	{
		dir = false; back = false; 	bigboi::indicator = 1;
	}
	else if (GetAsyncKeyState(Menu::Window.RageBotTab.keybasedaaright.GetKey())) // left
	{
		dir = true; back = false; 	bigboi::indicator = 2;
	}
	else if (GetAsyncKeyState(Menu::Window.RageBotTab.keybasedaaback.GetKey())) // back
	{
		back = true; 	bigboi::indicator = 3;
	}

	static bool switch2;
	Vector oldAngle = pCmd->viewangles;
	float oldForward = pCmd->forwardmove;
	float oldSideMove = pCmd->sidemove;
	AAYaw::ADAPTIVE_SIDE side = AAYaw::ADAPTIVE_UNKNOWN;
	static bool LBYUpdated = false;
	static int state = 0;
	float flCurTime = Interfaces::Globals->curtime;
	static float flTimeUpdate = 1.09f;
	static float flNextTimeUpdate = flCurTime + flTimeUpdate;
	if (flCurTime >= flNextTimeUpdate) {
		LBYUpdated = !LBYUpdated;
		state = 0;
	}

	if (Menu::Window.RageBotTab.BreakLBY.GetState())
	{
		if (NextLBYUpdate())
		{
			if (side == AAYaw::ADAPTIVE_LEFT) //prob what u are here for, ignore the adaptive shit, it was for testing. hf
				pCmd->viewangles.y += 90;
			else if (side == AAYaw::ADAPTIVE_RIGHT)
				pCmd->viewangles.y = 90;
			else
			{
				pCmd->viewangles.y -= 90;
			}
		}
	}

	switch (Menu::Window.RageBotTab.staticyaw.GetIndex())
	{
	case 0:
		break;
	case 1:
		pCmd->viewangles.y += 180;
		break;
	case 2:
		AAYaw::adaptive(pCmd, bSendPacket);
		break;
	case 3:
		// 180 Jitter
		AAYaw::NewBackJitter(pCmd);
		break;
	case 4:
		if (flipBool)
		{
			//AAYaw::LowerbodyNew(pCmd);
			AAYaw::AntiCorrectionALT(pCmd);
		}
		else if (!flipBool)
		{
			AAYaw::AntiCorrection(pCmd);
			//AAYaw::LowerbodyNewALT(pCmd);
		}
	break;	
	case 5:
	pCmd->viewangles.y += RandomNumber(-180, 180);
	break;
	case 6:
	{
		if (dir && !back)
		{
			back = false;
			pCmd->viewangles.y += 90;
			bigboi::indicator = 2;
		}
		else if (!dir && !back)
		{
			back = false;
			pCmd->viewangles.y -= 90;
			bigboi::indicator = 1;
		}
		else if (back)
		{
			pCmd->viewangles.y += 180;
			bigboi::indicator = 3;
		}
	}
	break;
	case 7:
		pCmd->viewangles.y += (Menu::Window.RageBotTab.SpinSpeed.GetValue() * 3) * Interfaces::Globals->curtime;
		break;
	}

	static auto RandomReal = false;
	if (RandomReal)
		pCmd->viewangles.y += Menu::Window.RageBotTab.AntiAimOffset.GetValue();
	else
		pCmd->viewangles.y -= Menu::Window.RageBotTab.AntiAimOffset.GetValue();
	RandomReal = !RandomReal;
}

void DoFakeAA(CUserCmd* pCmd, bool& bSendPacket, IClientEntity* pLocal)
{
	static bool switch2;
	Vector oldAngle = pCmd->viewangles;
	float oldForward = pCmd->forwardmove;
	float oldSideMove = pCmd->sidemove;

	switch (Menu::Window.RageBotTab.fakeyaw.GetIndex())
	{
	case 0:
		break;
	case 1:
		// Fast Spin
		pCmd->viewangles.y += 180;
		break;
	case 2:
		AAYaw::adaptive2(pCmd, bSendPacket);
		break;
	case 3:
		// 180 Jitter
		AAYaw::NewBackJitter(pCmd);
		break;
	case 4:
		if (flipBool)
		{
			//AAYaw::LowerbodyNew(pCmd);
			AAYaw::AntiCorrection(pCmd);
		}
		else if (!flipBool)
		{
			AAYaw::AntiCorrectionALT(pCmd);
			//AAYaw::LowerbodyNewALT(pCmd);
		}
		break;
	case 5:
		pCmd->viewangles.y += RandomNumber(-180, 180);
		break;
	case 6:
	{
		if (dir && !back)
		{
			back = false;
			pCmd->viewangles.y -= 90;
			bigboi::indicator = 2;
		}
		else if (!dir && !back)
		{
			back = false;
			pCmd->viewangles.y += 90;
			bigboi::indicator = 1;
		}
		else if (back)
		{
			pCmd->viewangles.y -= 180;
			bigboi::indicator = 3;
		}

	}
	break;
	case 7:
		pCmd->viewangles.y += (Menu::Window.RageBotTab.SpinSpeed.GetValue() * 3) * Interfaces::Globals->curtime;
		break;
	}
}

void CRageBot::DoAntiAim(CUserCmd *pCmd, bool &bSendPacket)
{
	IClientEntity* pLocal = hackManager.pLocal();

	if ((pCmd->buttons & IN_USE) || pLocal->GetMoveType() == MOVETYPE_LADDER)
		return;

	if (IsAimStepping || pCmd->buttons & IN_ATTACK)
		return;

	CBaseCombatWeapon* CSWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
	if (CSWeapon)
	{
		CSWeaponInfo* pWeaponInfo = CSWeapon->GetCSWpnData();
		if (!GameUtils::IsBallisticWeapon(CSWeapon))
		{
			if (Menu::Window.RageBotTab.AntiAimKnife.GetState())
			{
				if (!CanFire() || pCmd->buttons & IN_ATTACK2)
					return;
			}
			else
			{
				return;
			}
		}

	}
	if (Menu::Window.RageBotTab.AntiAimTarget.GetState())
	{
		aimAtPlayer(pCmd);
	}
	IClientEntity* LocalPlayer = hackManager.pLocal();
	
	//if (GetAsyncKeyState(VK_SPACE)) // back
//	{
//		pCmd->viewangles.x = 0;
//	}

	//32 espaço

	switch (Menu::Window.RageBotTab.AntiAimPitch.GetIndex())
	{
	case 0:
		break;
	case 1:
		pCmd->viewangles.x = 89.0000;
		break;

	case 2:
		pCmd->viewangles.x = -180;
		break;

	case 3:
	{
		static float Timer = 0;

		Timer++;

		if (!hackManager.pLocal()->IsAlive())
			Timer = 0;

		if (hackManager.pLocal()->GetFlags() & FL_ONGROUND)
		{
			if (Timer > 0 && Timer < 50)
			{
				if (bSendPacket)
				{
					pCmd->viewangles.x = 1080.f;
				}
				else
				{
					pCmd->viewangles.x = 1080.f;
				}
			}
			else
				pCmd->viewangles.x = 89.f;
		}
		else
		{
			Timer = 0;
			pCmd->viewangles.x = 89.f;
		}
	}
	break;

	case 4: 
	pCmd->viewangles.x = 89.0000;
	if (GetAsyncKeyState(82)) // back
	{
		pCmd->viewangles.x = 1080.f;
	}
	break;
}

	static int ChokedPackets = -1;
	ChokedPackets++;
	if (Menu::Window.MiscTab.FakeLagEnable.GetState() && Menu::Window.RageBotTab.AntiAimEnable.GetState())
	{
		if (bSendPacket)
		{
			DoFakeAA(pCmd, bSendPacket, pLocal);
		}
		else
		{
			DoRealAA(pCmd, pLocal, bSendPacket);
		}
	}
	else if (!Menu::Window.MiscTab.FakeLagEnable.GetState() && Menu::Window.RageBotTab.AntiAimEnable.GetState())
	{
		if (ChokedPackets < 1)
		{
			bSendPacket = true;
			DoFakeAA(pCmd, bSendPacket, pLocal);
		}
		else
		{
			bSendPacket = false;
			DoRealAA(pCmd, pLocal, bSendPacket);
			ChokedPackets = -1;
		}
	}

	//Sloppy way to put edge in here 

	//Ray_t ray;
	//trace_t tr;

	CTraceFilter traceFilter;
	traceFilter.pSkip = pLocal;

	bool bEdge = false;

	Vector angle;
	Vector eyePos = pLocal->GetVecOrigin() + pLocal->GetViewOffset();

	for (float i = 0; i < 360; i++)
	{
		Vector vecDummy(30.f, pCmd->viewangles.y, 0.f);
		vecDummy.y += i;

		Vector forward = vecDummy.Forward();

		//vecDummy.NormalizeInPlace();

		float flLength = ((16.f + 3.f) + ((16.f + 3.f) * sin(DEG2RAD(10.f)))) + 7.f;
		forward *= flLength;

		Ray_t ray;
		CGameTrace tr;

		ray.Init(eyePos, (eyePos + forward));
		Interfaces::Trace->EdgeTraceRay(ray, traceFilter, tr, true);

		if (tr.fraction != 1.0f)
		{
			Vector negate = tr.plane.normal;
			negate *= -1;

			Vector vecAng = negate.Angle();

			vecDummy.y = vecAng.y;

			//vecDummy.NormalizeInPlace();
			trace_t leftTrace, rightTrace;

			Vector left = (vecDummy + Vector(0, 45, 0)).Forward(); // or 45
			Vector right = (vecDummy - Vector(0, 45, 0)).Forward();

			left *= (flLength * cosf(rad(30)) * 2); //left *= (len * cosf(rad(30)) * 2);
			right *= (flLength * cosf(rad(30)) * 2); // right *= (len * cosf(rad(30)) * 2);

			ray.Init(eyePos, (eyePos + left));
			Interfaces::Trace->EdgeTraceRay(ray, traceFilter, leftTrace, true);

			ray.Init(eyePos, (eyePos + right));
			Interfaces::Trace->EdgeTraceRay(ray, traceFilter, rightTrace, true);

			if ((leftTrace.fraction == 1.f) && (rightTrace.fraction != 1.f))
			{
				vecDummy.y -= 45; // left
			}
			else if ((leftTrace.fraction != 1.f) && (rightTrace.fraction == 1.f))
			{
				vecDummy.y += 45; // right     
			}

			angle.y = vecDummy.y;
			angle.y += 360;
			bEdge = true;
		}
	}

	if (bEdge)
	{
		static bool turbo = true;

		switch (Menu::Window.RageBotTab.AntiAimEdge.GetIndex())
		{
		case 0:
			// Nothing
			break;
		case 1:
			// Regular
			pCmd->viewangles.y = angle.y;
			break;
		case 2:
		{
			if (pCmd->command_number % 2)
			{
				bSendPacket = false;
				pCmd->viewangles.y = angle.y;
			}
			else
			{
				bSendPacket = true;
				pCmd->viewangles.y = angle.y + 180.f;
			}
			break;
		case 3:
			// Fakehead meme
		{
			if (pCmd->command_number % 2)
			{
				bSendPacket = false;
				if (NextLBYUpdate())
				{
					pCmd->viewangles.y = angle.y + 90;
				}
				else
				{
					pCmd->viewangles.y = angle.y;
				}
			}
			else
			{
				bSendPacket = true;
				if (NextLBYUpdate())
				{
					pCmd->viewangles.y = angle.y + Menu::Window.RageBotTab.AntiAimOffsetFake.GetValue();
				}
				else
					pCmd->viewangles.y = angle.y;
			}
			break;
		}
		}
		}
	}
}