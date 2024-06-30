#include "pch.h"
#include "GeometryGenerator.h"

MeshData GeometryGenerator::MakeSphere(float radius, UINT numOfStack, UINT numOfSlice, const string& textureName)
{
	MeshData meshData;
	auto& vertices = meshData.vertices;
	Vector3 startPos = Vector3{ 0.0f,-radius,0.0f };
	const float xTheta = -XM_2PI / float(numOfSlice);
	const float yTheta = -XM_PI / float(numOfStack);
	for (UINT i = 0; i <= numOfStack; ++i)
	{
		Vector3 startOfRotation =
			Vector3::Transform(startPos, Matrix::CreateRotationZ(yTheta * i));
		for (UINT j = 0; j <= numOfSlice; ++j)
		{
			Vertex v;
			v.position =
				Vector3::Transform(startOfRotation, Matrix::CreateRotationY(xTheta * j));
			v.normal = v.position;
			v.normal.Normalize();
			v.uv = Vector2{ float(j) / numOfSlice, 1.0f - float(i) / numOfStack };
			vertices.push_back(v);
		}
	}
	auto& indices = meshData.indices;
	int yOffSet = numOfSlice + 1;
	for (UINT i = 0; i < numOfStack; ++i)
	{
		int startPoint = i * yOffSet;
		for (UINT j = 0; j < numOfSlice; ++j)
		{
			indices.push_back(startPoint + j);
			indices.push_back(startPoint + yOffSet + j);
			indices.push_back(startPoint + yOffSet + j + 1);

			indices.push_back(startPoint + j);
			indices.push_back(startPoint + yOffSet + j + 1);
			indices.push_back(startPoint + j + 1);
		}
	}
	meshData.textureName = textureName;
	return meshData;
}

MeshData GeometryGenerator::MakeDisc(float insideRadius, float outsideRadius, UINT numOfDevide, const string& textureName)
{
	MeshData result;
	vector<Vertex>& vertices = result.vertices;
	const float theta = XM_2PI / numOfDevide;
	Vector3 startPos = Vector3{ 0.0f,0.0f,-insideRadius };
	for (UINT i = 0; i <= numOfDevide; ++i)
	{
		Vertex v;
		v.position = Vector3::Transform(startPos, Matrix::CreateRotationY(i * theta));
		v.normal = Vector3(0.0f, 1.0f, 0.0f);
		v.uv = Vector2{ float(i) / numOfDevide ,0.0f };
		vertices.push_back(v);
	}
	startPos = Vector3{ 0.0f,0.0f,-outsideRadius };
	for (UINT i = 0; i <= numOfDevide; ++i)
	{
		Vertex v;
		v.position = Vector3::Transform(startPos, Matrix::CreateRotationY(i * theta));
		v.normal = Vector3(0.0f, 1.0f, 0.0f);
		v.uv = Vector2{ float(i) / numOfDevide,1.0f };
		vertices.push_back(v);
	}
	vector<uint32_t>& indices = result.indices;
	int offset = numOfDevide + 1;
	for (UINT i = 0; i < numOfDevide; ++i)
	{
		indices.push_back(i);
		indices.push_back(i + offset);
		indices.push_back(i + 1);

		indices.push_back(i + 1);
		indices.push_back(i + offset);
		indices.push_back(i + 1 + offset);
	}
	result.textureName = textureName;
	return result;
}

MeshData GeometryGenerator::MakeSquare()
{
	MeshData result;
	result.textureName = "";
	vector<Vertex>& v = result.vertices;
	Vertex vertex;
	vertex.position = Vector3{ -1.0f,-1.0f,0.0f };
	vertex.normal = Vector3{ 0.0f,0.0f,-1.0f };
	vertex.uv = Vector2{ 0.0f,1.0f };
	vertex.tangent = Vector3{ 1.0f,0.0f,0.0f };
	v.push_back(vertex);

	vertex.position = Vector3{ -1.0f,1.0f,0.0f };
	vertex.normal = Vector3{ 0.0f,0.0f,-1.0f };
	vertex.uv = Vector2{ 0.0f,0.0f };
	vertex.tangent = Vector3{ 1.0f,0.0f,0.0f };
	v.push_back(vertex);

	vertex.position = Vector3{ 1.0f,1.0f,0.0f };
	vertex.normal = Vector3{ 0.0f,0.0f,-1.0f };
	vertex.uv = Vector2{ 1.0f,0.0f };
	vertex.tangent = Vector3{ 1.0f,0.0f,0.0f };
	v.push_back(vertex);

	vertex.position = Vector3{ 1.0f,-1.0f,0.0f };
	vertex.normal = Vector3{ 0.0f,0.0f,-1.0f };
	vertex.uv = Vector2{ 1.0f,1.0f };
	vertex.tangent = Vector3{ 1.0f,0.0f,0.0f };
	v.push_back(vertex);

	vector<uint32_t>& i = result.indices;
	i.push_back(0);
	i.push_back(1);
	i.push_back(2);

	i.push_back(0);
	i.push_back(2);
	i.push_back(3);
	return result;
}

MeshData GeometryGenerator::MakeTriangle()
{
	MeshData result;
	result.textureName = "";

	vector<Vertex>& v = result.vertices;
	Vertex vertex;

	vertex.position = Vector3{ -0.5f,-0.5f,0.0f };
	vertex.normal = Vector3{ 0.0f,0.0f,-1.0f };
	vertex.uv = Vector2(0.0f);
	vertex.tangent = Vector3{ 1.0f,0.0f,0.0f };
	v.push_back(vertex);

	vertex.position = Vector3{ 0.0f,0.5f,0.0f };
	vertex.normal = Vector3{ 0.0f,0.0f,-1.0f };
	vertex.uv = Vector2(0.0f);
	vertex.tangent = Vector3{ 1.0f,0.0f,0.0f };
	v.push_back(vertex);

	vertex.position = Vector3{ 0.5f,-0.5f,0.0f };
	vertex.normal = Vector3{ 0.0f,0.0f,-1.0f };
	vertex.uv = Vector2(0.0f);
	vertex.tangent = Vector3{ 1.0f,0.0f,0.0f };
	v.push_back(vertex);

	vector<uint32_t>& i = result.indices;
	i.push_back(0);
	i.push_back(1);
	i.push_back(2);

	return result;
}
