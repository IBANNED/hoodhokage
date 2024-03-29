
#pragma once

#include "MiscDefinitions.h"
#include "ClientRecvProps.h"
#include "offsets.h"
#include "Vector.h"
#include "MiscClasses.h"
#include "Vector2D.h"


class IMaterial
{
public:
	const char* GetName()
	{
		typedef const char*(__thiscall* oGetName)(PVOID);
		return call_vfunc< oGetName >(this, Offsets::VMT::Material_GetName)(this);
	}

	void SetMaterialVarFlag(MaterialVarFlags_t flag, bool value)
	{
		typedef void(__thiscall* oSetMatFlag)(PVOID, MaterialVarFlags_t, bool);
		return call_vfunc< oSetMatFlag >(this, Offsets::VMT::Material_SetMaterialVarFlag)(this, flag, value);
	}

	bool GetMaterialVarFlag(MaterialVarFlags_t flag)
	{
		typedef bool(__thiscall* oGetMatFlag)(PVOID, MaterialVarFlags_t);
		return call_vfunc< oGetMatFlag >(this, Offsets::VMT::Material_GetMaterialVarFlag)(this, flag);
	}

	void AlphaModulate(float a)
	{
		typedef void(__thiscall* oAlphaModulate)(PVOID, float);
		return call_vfunc< oAlphaModulate >(this, Offsets::VMT::Material_AlphaModulate)(this, a);
	}

	void ColorModulate(float r, float g, float b)
	{
		typedef void(__thiscall* oColorModulate)(PVOID, float, float, float);
		return call_vfunc< oColorModulate >(this, Offsets::VMT::Material_ColorModulate)(this, r, g, b);
	}

	void IncrementReferenceCount(void)
	{
		typedef void(__thiscall* oIncrementReferenceCount)(PVOID);
		return call_vfunc< oIncrementReferenceCount >(this, Offsets::VMT::Material_IncrementReferenceCount)(this);
	}
};

class CMaterialSystem
{
public:
	IMaterial* FindMaterial(char const* pMaterialName, const char *pTextureGroupName, bool complain = true, const char *pComplainPrefix = NULL)
	{
		typedef IMaterial*(__thiscall* oFindMaterial)(PVOID, char const*, char const*, bool, char const*);
		return call_vfunc< oFindMaterial >(this, Offsets::VMT::MaterialSystem_FindMaterial)(this, pMaterialName, pTextureGroupName, complain, pComplainPrefix);
	}

	IMaterial*	CreateMaterial(const char *pMaterialName, KeyValues *pVMTKeyValues)
	{
		typedef IMaterial* (__thiscall* oCreateMaterial)(PVOID, const char *, KeyValues*);
		return call_vfunc<oCreateMaterial>(this, Offsets::VMT::MaterialSystem_CreateMaterial)(this, pMaterialName, pVMTKeyValues);
	}
};

class IVModelRender
{
public:
	void ForcedMaterialOverride(IMaterial *material, OverrideType_t type = OVERRIDE_NORMAL, int idk = NULL)
	{
		typedef void(__thiscall* Fn)(void*, IMaterial*, OverrideType_t, int);
		return call_vfunc<Fn>(this, Offsets::VMT::ModelRender_ForcedMaterialOverride)(this, material, type, idk);
	}//crashed here

};

template <typename Fn> __forceinline Fn GetVirtualFunction(void* pClassBase, int nFunctionIndex) {
	return (Fn)((PDWORD)*(PDWORD*)pClassBase)[nFunctionIndex];
}

class CModelInfo
{
public:
	inline void* GetModel(int Index)
	{
		return GetVirtualFunction<void*(__thiscall *)(void*, int)>(this, 1)(this, Index);
	}
	int	GetModelIndex(const char *name)
	{
		typedef int (__thiscall* oGetModelName)(PVOID, const char *);
		return call_vfunc< oGetModelName >(this, 2)(this, name);
	}
	inline const char* GetModelName(const void* Model) {
		return GetVirtualFunction<const char*(__thiscall *)(void*, const void*)>(this, 3)(this, Model);
	}
	const char *GetModelName(const model_t *mod)
	{
		typedef const char *(__thiscall* oGetModelName)(PVOID, const model_t*);
		return call_vfunc< oGetModelName >(this, Offsets::VMT::ModelInfo_GetModelName)(this, mod);
	}

	void GetModelMaterials(const model_t *model, int count, IMaterial** ppMaterial)
	{
		typedef void *(__thiscall* oGetModelMaterials)(PVOID, const model_t*, int, IMaterial**);
		call_vfunc<oGetModelMaterials>(this, 19)(this, model, count, ppMaterial);
	}

	studiohdr_t *GetStudiomodel(const model_t* Model)
	{
		typedef studiohdr_t*(__thiscall* Fn)(void*, const model_t*);
		return call_vfunc<Fn>(this, 32)(this, Model);//30
	}
};

class CVRenderView
{
public:
	void SetBlend(float alpha)
	{
		typedef void(__thiscall* oDrawModelExecute)(PVOID, float);
		return call_vfunc< oDrawModelExecute >(this, Offsets::VMT::RenderView_SetBlend)(this, alpha);
	}

	void SetColorModulation(float const* colors)
	{
		typedef void(__thiscall* oDrawModelExecute)(PVOID, float const*);
		return call_vfunc< oDrawModelExecute >(this, Offsets::VMT::RenderView_SetColorModulation)(this, colors);
	}

};