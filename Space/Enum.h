#pragma once
enum class KEY_TYPE
{
	Q, W, E, A, S, D, F,
	LBUTTON, RBUTTON, ESC,
	B1, B2, SPACE,
	END,
};
enum class KEY_STATE
{
	NONE,
	TAP,
	AWAY,
	HOLD,
};
enum class SCENE_TYPE
{
	SPACE,
	RENDER,
	SPRITE,
	SPH,
	END,
};
enum class IBL_TYPE
{
	SPECULAR,
	IRRADIANCE,
	LUT,
	END,
};

enum class TEXTURE_TYPE
{
	ALBEDO,
	NORMAL,
	AO,
	EMISSIVE,
	METAL,
	ROUGHNESS,
	HEIGHT,
	END,
};
enum class RSS_TYPE
{
	SOLID,
	SOLIDCCW,
	WIRE,
	WIRECCW,
	END,
};
enum class DSS_TYPE
{
	BASIC,
	MASK,
	DRAWMASK,
	END,
};