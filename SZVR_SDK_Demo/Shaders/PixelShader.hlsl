Texture2D tx : register(t0);
SamplerState samLinear : register(s0);

/*struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};
*/

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float4 Col : COLOR;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
	//return tx.Sample(samLinear, input.Tex);
	return input.Col;
}