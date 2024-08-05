#pragma once
class SPHKernels
{
public:
	static float CubicSpline(float q);
	static float CubicSplineGrad(float q);
private:
	SPHKernels() {}
};

