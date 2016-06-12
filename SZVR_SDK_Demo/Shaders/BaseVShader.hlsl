cbuffer ConstantBuffer
{
	float4x4 final;
	float4 lightvec;
	float4 lightcol;
	float4 ambientcol;
};

struct VS_INPUT
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	output.Pos = mul(final, input.Pos);
	output.Tex = input.Tex;

	return output;
}