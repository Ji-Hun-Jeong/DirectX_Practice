#pragma once
class GeometryGenerator
{
public:
	static MeshData MakeSphere(float radius, UINT numOfStack, UINT numOfSlice, const string& textureName="");
	static MeshData MakeDisc(float insideRadius, float outsideRadius, UINT numOfDevide, const string& textureName = "");
	static MeshData MakeSquare();
	static MeshData MakeTriangle();
private:
	GeometryGenerator() {}
};

