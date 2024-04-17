#include "Scene.hlsli"

cbuffer CullParams : register (b1)
{
	uint4 numShapes;
	float4 bbMin[1200];
	float4 bbMax[1200];
};

RWStructuredBuffer<uint> indirectArgs : register(u0);
RWStructuredBuffer<uint4> objectIds : register(u1);

bool IsBoxInside(in float4 frustum[6], in float3 bbMin, in float3 bbMax)
{
	for (int i = 0; i < 6; i++)
	{
		const float3 norm = frustum[i].xyz;
		float4 p = float4(norm.x > 0 ? bbMin.x : bbMax.x, norm.y > 0 ? bbMin.y : bbMax.y, norm.z > 0 ? bbMin.z : bbMax.z, 1.0);
		float s = dot(p, frustum[i]);
		if (s > 0.0f)
		{
			return false;
		}
	}

	return true;
}

[numthreads(64, 1, 1)]
void main(uint3 globalThreadId : SV_DispatchThreadID)
{
	if (globalThreadId.x >= numShapes.x) {
		return;
	}

	if (IsBoxInside(frustum, bbMin[globalThreadId.x].xyz, bbMax[globalThreadId.x].xyz)) {
		uint id = 0;
		InterlockedAdd(indirectArgs[1], 1, id);
		objectIds[id] = uint4(globalThreadId.x, 0, 0, 0);
	}
}