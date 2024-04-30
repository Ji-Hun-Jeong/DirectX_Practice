#pragma once
class GeometryGenerator
{
public:
	static MeshData MakeSphere(float radius, UINT numOfStack, UINT numOfSlice, const string& textureName="");
	static MeshData MakeNormal(const MeshData& meshData);
	static MeshData MakeDisc(float insideRadius, float outsideRadius, UINT numOfDevide, const string& textureName = "");
private:
	GeometryGenerator() {}
};

