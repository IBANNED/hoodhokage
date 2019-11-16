/*
- hoodhokage by ibanned -
*/

#include "Interfaces.h"
#include "Menu.h"
#include "Resolver.h"
#include "AnimFixKnife.h"

RecvVarProxyFn oSmokeEffectTickBegin = NULL;
RecvVarProxyFn oFlashMaxAlpha = NULL;
RecvVarProxyFn oDidSmokeEffect = NULL;
// - credits by ibanned -

void RecvProxy_FlashMaxAlpha(CRecvProxyData *pData, void *pStruct, void *pOut)
{
	float *value = &pData->m_Value.m_Float;

	if (Menu::Window.VisualsTab.OtherNoFlash.GetState())
	{
		*value = 0.0f;
	}
	else
	{
		*value = 255.0f;
	}

	oFlashMaxAlpha(pData, pStruct, pOut);
}

void AnimationFixHook()
{
	for (ClientClass* pClass = Interfaces::Client->GetAllClasses(); pClass; pClass = pClass->m_pNext) {
		if (!strcmp(pClass->m_pNetworkName, "CBaseViewModel")) {
			// Search for the 'm_nModelIndex' property.
			RecvTable* pClassTable = pClass->m_pRecvTable;

			for (int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++) {
				RecvProp* pProp = &pClassTable->m_pProps[nIndex];

				if (!pProp || strcmp(pProp->m_pVarName, "m_nSequence"))
					continue;

				// Store the original proxy function.
				fnSequenceProxyFn = static_cast<RecvVarProxyFn>(pProp->m_ProxyFn);

				// Replace the proxy function with our sequence changer.
				pProp->m_ProxyFn = static_cast<RecvVarProxyFn>(SetViewModelSequence);

				break;
			}

			break;
		}
	}
}

void AnimationFixUnhook()
{
	for (ClientClass* pClass = Interfaces::Client->GetAllClasses(); pClass; pClass = pClass->m_pNext) {
		if (!strcmp(pClass->m_pNetworkName, "CBaseViewModel")) {
			// Search for the 'm_nModelIndex' property.
			RecvTable* pClassTable = pClass->m_pRecvTable;

			for (int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++) {
				RecvProp* pProp = &pClassTable->m_pProps[nIndex];

				if (!pProp || strcmp(pProp->m_pVarName, "m_nSequence"))
					continue;

				// Replace the proxy function with our sequence changer.
				pProp->m_ProxyFn = fnSequenceProxyFn;

				break;
			}

			break;
		}
	}
}

void ApplyNetVarsHooks()
{
	AnimationFixHook();

	ClientClass *pClass = Interfaces::Client->GetAllClasses();

	while (pClass)
	{
		const char *pszName = pClass->m_pRecvTable->m_pNetTableName;

		for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
		{
			RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
			const char *name = pProp->m_pVarName;

			/*if (!strcmp(pszName, "DT_BaseViewModel"))
			{
				// Knives
				if (!strcmp(name, "m_nModelIndex"))
				{
					oRecvModelIndex = (RecvVarProxyFn)pProp->m_ProxyFn;
					pProp->m_ProxyFn = RecvProxy_Viewmodel;
				}
			}*/
			/*if (!strcmp(pszName, "DT_SmokeGrenadeProjectile"))
			{
				if (!strcmp(name, "m_nSmokeEffectTickBegin"))
				{
					oSmokeEffectTickBegin = (RecvVarProxyFn)pProp->m_ProxyFn;
					pProp->m_ProxyFn = RecvProxy_SmokeEffectTickBegin;
				}
				else if (!strcmp(name, "m_bDidSmokeEffect"))
				{
					oDidSmokeEffect = (RecvVarProxyFn)pProp->m_ProxyFn;
					pProp->m_ProxyFn = RecvProxy_DidSmokeEffect;
				}
			}*/
			if (!strcmp(pszName, "DT_CSPlayer"))
			{
				if (!strcmp(name, "m_flFlashMaxAlpha"))
				{
					oFlashMaxAlpha = (RecvVarProxyFn)pProp->m_ProxyFn;
					pProp->m_ProxyFn = RecvProxy_FlashMaxAlpha;
				}
			}
		}

		pClass = pClass->m_pNext;
	}
}

void RemoveNetVarsHooks()
{
	AnimationFixUnhook();
	ClientClass *pClass = Interfaces::Client->GetAllClasses();

	while (pClass)
	{
		const char *pszName = pClass->m_pRecvTable->m_pNetTableName;

		for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
		{
			RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
			const char *name = pProp->m_pVarName;

			/*if (!strcmp(pszName, "DT_BaseViewModel"))
			{
				// Knives
				if (!strcmp(name, "m_nModelIndex"))
				{
					pProp->m_ProxyFn = oRecvnModelIndex;
				}
			}
			else if (!strcmp(pszName, "DT_SmokeGrenadeProjectile"))
			{
				if (!strcmp(name, "m_nSmokeEffectTickBegin"))
				{
					pProp->m_ProxyFn = oSmokeEffectTickBegin;
				}
				else if (!strcmp(name, "m_bDidSmokeEffect"))
				{
					pProp->m_ProxyFn = oDidSmokeEffect;
				}
			}*/
			if (!strcmp(pszName, "DT_CSPlayer"))
			{
				if (!strcmp(name, "m_flFlashMaxAlpha"))
				{
					pProp->m_ProxyFn = oFlashMaxAlpha;
				}


				pClass = pClass->m_pNext;
			}
		}
	}
}

// Shad0ws Yaw fix
// (FIX ME UP LATER)
void FixY(const CRecvProxyData *pData, void *pStruct, void *pOut)
{
	

	static Vector vLast[65];
	static bool bShotLastTime[65];
	static bool bJitterFix[65];

	float *flPitch = (float*)((DWORD)pOut - 4);
	float flYaw = pData->m_Value.m_Float;
	bool bHasAA;
	bool bSpinbot;

	
	*(float*)(pOut) = flYaw;
}

// Simple fix for some Fake-Downs
void FixX(const CRecvProxyData* pData, void* pStruct, void* pOut) // Clamp other player angles to fix fakedown or lisp
{
	float flPitch = pData->m_Value.m_Float;
	static float time = 1;
	time++;
	if (Menu::Window.RageBotTab.AccuracyResolverPitch.GetState())
	{
		if (time < 180)
		{
			flPitch = time;
		}
		else 
		{
			time = 1;
		}
	}
	*(float*)pOut = flPitch;
}


RecvVarProxyFn oRecvnModelIndex;

void Hooked_RecvProxy_Viewmodel(CRecvProxyData *pData, void *pStruct, void *pOut)
{
	// Get the knife view model id's
	int default_t = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_default_t.mdl");
	int default_ct = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
	int iBayonet = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
	int iButterfly = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
	int iFlip = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_flip.mdl");
	int iGut = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_gut.mdl");
	int iKarambit = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_karam.mdl");
	int iM9Bayonet = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
	int iHuntsman = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_tactical.mdl");
	int iFalchion = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
	int iDagger = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_push.mdl");
	int iBowie = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");
	int iGunGame = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_gg.mdl");

	int Navaja = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_gypsy_jackknife.mdl");
	int Stiletto = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_stiletto.mdl");
	int Ursus = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_ursus.mdl");
	int Talon = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_widowmaker.mdl");

	// Get local player (just to stop replacing spectators knifes)
	auto pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (Menu::Window.SkinTab.SkinEnable.GetState() && pLocal)
	{
		// If we are alive and holding a default knife(if we already have a knife don't worry about changing)
		if (pLocal->IsAlive() && (
			pData->m_Value.m_Int == default_t ||
			pData->m_Value.m_Int == default_ct ||
			pData->m_Value.m_Int == iBayonet ||
			pData->m_Value.m_Int == iFlip ||
			pData->m_Value.m_Int == iGunGame ||
			pData->m_Value.m_Int == iGut ||
			pData->m_Value.m_Int == iKarambit ||
			pData->m_Value.m_Int == iM9Bayonet ||
			pData->m_Value.m_Int == iHuntsman ||
			pData->m_Value.m_Int == iBowie ||
			pData->m_Value.m_Int == iButterfly ||
			pData->m_Value.m_Int == iFalchion ||
			pData->m_Value.m_Int == iDagger ||
			pData->m_Value.m_Int == Navaja ||
			pData->m_Value.m_Int == Stiletto ||
			pData->m_Value.m_Int == Ursus ||
			pData->m_Value.m_Int == Talon))
		{
			// Set whatever knife we want
			if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 0)
				pData->m_Value.m_Int = iBayonet;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 9)
				pData->m_Value.m_Int = iBowie;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 6)
				pData->m_Value.m_Int = iButterfly;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 7)
				pData->m_Value.m_Int = iFalchion;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 1)
				pData->m_Value.m_Int = iFlip;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 2)
				pData->m_Value.m_Int = iGut;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 5)
				pData->m_Value.m_Int = iHuntsman;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 3)
				pData->m_Value.m_Int = iKarambit;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 4)
				pData->m_Value.m_Int = iM9Bayonet;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 8)
				pData->m_Value.m_Int = iDagger;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 10)
				pData->m_Value.m_Int = Navaja;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 11)
				pData->m_Value.m_Int = Stiletto;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 12)
				pData->m_Value.m_Int = Ursus;
			else if (Menu::Window.SkinTab.KnifeModel.GetIndex() == 13)
				pData->m_Value.m_Int = Talon;
		}
	}
	// Carry on the to original proxy
	oRecvnModelIndex(pData, pStruct, pOut);
}

void ApplyAAAHooks()
{
	ClientClass *pClass = Interfaces::Client->GetAllClasses();
	while (pClass)
	{
		const char *pszName = pClass->m_pRecvTable->m_pNetTableName;
		if (!strcmp(pszName, "DT_CSPlayer"))
		{
			for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
			{
				RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
				const char *name = pProp->m_pVarName;

				// Pitch Fix
				if (!strcmp(name, "m_angEyeAngles[0]"))
				{
					pProp->m_ProxyFn = FixX;
				}

				// Yaw Fix
				if (!strcmp(name, "m_angEyeAngles[1]"))
				{
					Utilities::Log("Yaw Fix Applied");
					pProp->m_ProxyFn = FixY;
				}
			}
		}
		else if (!strcmp(pszName, "DT_BaseViewModel"))
		{
			for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
			{
				RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
				const char *name = pProp->m_pVarName;

				// Knives
				if (!strcmp(name, "m_nModelIndex"))
				{
					oRecvnModelIndex = (RecvVarProxyFn)pProp->m_ProxyFn;
					pProp->m_ProxyFn = Hooked_RecvProxy_Viewmodel;
				}
			}
		}
		pClass = pClass->m_pNext;
	}
}