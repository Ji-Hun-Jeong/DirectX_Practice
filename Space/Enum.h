#pragma once
enum class KEY_TYPE
{
	Q,W,E,A,S,D,F,
	LBUTTON,RBUTTON,ESC,
	B1,B2,
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
	TEST,
	END,
};
enum class TEXTURE_TYPE
{
	SPECULAR,
	IRRADIANCE,
	LUT,
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