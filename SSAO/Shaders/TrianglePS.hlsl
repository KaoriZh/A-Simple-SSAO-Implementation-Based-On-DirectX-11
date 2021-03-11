#include "TriangleTest.hlsli"

float4 main(VertexOut pIn) : SV_TARGET
{
    return pIn.color;
}