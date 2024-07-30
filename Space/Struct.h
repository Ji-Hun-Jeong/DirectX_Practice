#pragma once
struct Vertex
{
	Vector3 position;
	Vector3 normal;
	Vector2 uv;
	Vector3 tangent;
};
struct MeshData
{
	vector<Vertex> vertices;
	vector<uint32_t> indices;
};
struct MyRay
{
	Vector3 startPos;
	Vector3 rayDir;
};
struct LightConst	// 32
{
	Vector3 lightPos = Vector3(0.0f);
	float fallOfStart = 1.0f;
	Vector3 lightStrength = Vector3(1.0f);
	float fallOfEnd = 10.0f;
	Vector3 lightDir = Vector3(0.0f);
	float spotFactor = 1.0f;
	Vector3 haloFactor = Vector3(1.0f);
	float radius = 1.0f;
	Matrix lightViewProj;
};
struct Rim
{
	float rimStrength = 13.0f;
	float rimPower = 1.8f;
	float dummy = 0.0f;
	int useRim = true;
};

// 구조체 크기를 256바이트로 맞춤
__declspec(align(256)) struct MeshConstData
{
	Matrix world;
	Matrix worldIT;	// Inverse Transpose
};

__declspec(align(256)) struct MaterialConstData
{
	Vector3 albedoFactor = Vector3(1.0f);
	float metallicFactor = 0.7f;
	Vector3 emissionFactor = Vector3(0.0f);
	float roughnessFactor = 0.5f;

	int useAlbedo = true;
	int useAO = true;
	int useEmissive = true;
	int useRoughness = true;

	int useMetallic = true;
	int isLight = false;
	float ambientFactor = 1.0f;
	float dummy = 0.0f;
};

__declspec(align(256)) struct CommonConstData
{
	int useHeight = true;
	float heightScale = 1.0f;
	int useNormal = true;
	float normalSize = 1.0f;

	int mode = 0;
	float depthStrength = 1.0f;
	float fogStrength = 0.0f;
	float dummy = 0.0f;
};

__declspec(align(256)) struct GlobalConstData
{
	Matrix view;
	Matrix proj;
	Matrix viewProj;
	Matrix invProj;

	Vector3 eyePos;
	float strengthIBL = 1.0f;

	LightConst light;
	float exposure = 1.0f;
	float gamma = 1.0f;
	float dummy[2] = { 0.0f };
};

struct DirArrowConstantData 
{
	Matrix model;
	Matrix view;
	Matrix projection;
	Matrix invTranspose;
	Vector3 viewDir;
	float arrowSize = 0.2f;
	Vector3 upDir;
	float dummy1 = 0.0f;
	Vector3 rightDir;
	float dummy2 = 0.0f;
};

struct Particle
{
	Vector3 pos;
	Vector3 color;
};

static_assert(sizeof(MeshConstData) % 16 == 0, "MeshConstData Size Check");
static_assert(sizeof(MaterialConstData) % 16 == 0, "MaterialConstData Size Check");
static_assert(sizeof(GlobalConstData) % 16 == 0, "GlobalConstData Size Check");
static_assert(sizeof(DirArrowConstantData) % 16 == 0, "DirArrowConstantData Size Check");