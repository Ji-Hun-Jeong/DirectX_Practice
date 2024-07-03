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
	string textureName;
};
struct MyRay
{
	Vector3 startPos;
	Vector3 rayDir;
};
struct Light	// 32
{
	Vector3 lightPos = Vector3(0.0f);
	float fallOfStart = 1.0f;
	Vector3 lightStrength = Vector3(1.0f);
	float fallOfEnd = 10.0f;
};
struct Material	// 48
{
	Vector3 ambient = Vector3(0.1f);
	float dummy1 = 0.0f;
	Vector3 diffuse = Vector3(0.03f);
	int selected = false;
	Vector3 specular = Vector3(0.1f);
	float	shiness = 10.0f;
};
struct Bloom
{
	float threshold = 0.1f;
	float dx = 0.0f;
	float dy = 0.0f;
	float bloomStrength = 0.0f;
};
struct Rim
{
	float rimStrength = 13.0f;
	float rimPower = 1.8f;
	float dummy = 0.0f;
	int useRim = true;
};

struct VertexConstantData
{
	Matrix model;
	Matrix view;
	Matrix projection;
	Matrix invTranspose;
	int useHeight = false;
	float heightScale = 1.0f;
	float dummy[2];
};

struct PixelConstantData	// 96
{
	Vector3 eyePos;
	int isSun = 0;
	Light light;
	Material mat;
	Bloom bloom;
	Rim rim;
	int useAlbedo = false;
	int useNormal = false;
	int useAO = false;
	int useRoughness = false;

	int useMetallic = false;
	float exposure = 1.0f;
	float gamma = 1.0f;
	float dummy;
};
struct NormalConstantData	// 48
{
	Matrix model;
	Matrix view;
	Matrix projection;
	Matrix invTranspose;
	float normalSize = 1.0f;
	float dummy[3] = { 0.0f };
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
static_assert(sizeof(VertexConstantData) % 16 == 0, "VertexConstantData Size Check");
static_assert(sizeof(PixelConstantData) % 16 == 0, "PixelConstantData Size Check");
static_assert(sizeof(NormalConstantData) % 16 == 0, "NormalConstantData Size Check");
static_assert(sizeof(DirArrowConstantData) % 16 == 0, "DirArrowConstantData Size Check");
