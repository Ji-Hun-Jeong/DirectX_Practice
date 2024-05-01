#pragma once
struct Vertex
{
	Vector3 position;
	Vector3 normal;
	Vector2 uv;
};
struct MeshData
{
	vector<Vertex> vertices;
	vector<uint32_t> indices;
	string textureName;
};
struct VertexConstantData
{
	Matrix model;
	Matrix view;
	Matrix projection;
	Matrix invTranspose;
};
struct Light	// 32
{
	Vector3 lightPos = Vector3(0.0f);
	float fallOfStart = 1.0f;
	Vector3 lightStrength = Vector3(1.0f);
	float fallOfEnd = 1000.0f;
};
struct Material	// 48
{
	Vector3 ambient = Vector3(0.5f);
	float dummy1;
	Vector3 diffuse = Vector3(0.5f);
	float dummy2;
	Vector3 specular = Vector3(0.3f);
	float	shiness = 2.0f;
};	
struct PixelConstantData	// 96
{
	Vector3 eyePos;
	int isSun = 0;
	Light light;
	Material mat;
};
struct NormalConstantData : public VertexConstantData	// 48
{
	float normalSize = 1.0f;
	float dummy[3];
};
static_assert(sizeof(VertexConstantData) % 16 == 0,"VertexConstantData Size Check");
static_assert(sizeof(PixelConstantData) % 16 == 0, "PixelConstantData Size Check");
static_assert(sizeof(NormalConstantData) % 16 == 0, "GeometryConstantData Size Check");
