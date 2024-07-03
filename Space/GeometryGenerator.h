#pragma once
class GeometryGenerator
{
public:
	static MeshData MakeSphere(float radius, UINT numOfStack, UINT numOfSlice);
	static MeshData MakeDisc(float insideRadius, float outsideRadius, UINT numOfDevide);
	static MeshData MakeSquare();
	static MeshData MakeTriangle();
private:
	GeometryGenerator() {}
};

