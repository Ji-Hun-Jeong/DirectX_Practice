#include "pch.h"
#include "SPHKernels.h"

float SPHKernels::CubicSpline(float q)
{
	assert(q >= 0);
    q *= 2.0f;
    float value = 3.0f / (2.0f * 3.141592f);
    if (q < 1.0f)
        value *= 2.0f / 3.0f - q * q + q * q * q / 2.0f;
    else if (q < 2.0f)
        value *= (2.0f - q) * (2.0f - q) * (2.0f - q) / 6.0f;
    else
        value *= 0.0f;

    return value;
}

float SPHKernels::CubicSplineGrad(float q)
{
	assert(q >= 0.0f);
	q *= 2.0f;

	float value = 3.0f / (2.0f * XM_PI);

	if (q < 1.0f)
		value *= (-2.0f * q + 1.5f * q * q);

	else if (q < 2.0f)
		value *= -0.5f * (2.0f - q) * (2.0f - q);

	else 
		value *= 0.0f;

	return value;
}
