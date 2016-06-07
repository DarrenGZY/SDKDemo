cbuffer ConstantBuffer
{
	float4x4 final;
};

/*struct VS_INPUT
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};
*/

struct VS_INPUT
{
	float4 Pos : POSITION;
	float4 Col : COLOR;
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 Col : COLOR;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	output.Pos = mul(final, input.Pos);
	//output.Tex = input.Tex;
	output.Col = input.Col;
	return output;
}