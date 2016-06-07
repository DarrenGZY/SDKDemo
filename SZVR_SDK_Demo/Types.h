#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include "Util/WICTextureLoader.h"
#include "PixelShader.h"
#include "VertexShader.h"

using namespace DirectX;

typedef struct _CBUFFER
{
	XMMATRIX final;
}CBUFFER;

typedef struct _VERTEX
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
}VERTEX;

typedef struct _SOLIDVERTEX
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
}SOLIDVERTEX;

typedef struct _D3DCONTEXT
{
	ID3D11Device *device;						// Device
	ID3D11DeviceContext *devcontext;			// Device Context
	ID3D11VertexShader *vertexShader;			// Vertex Shader
	ID3D11PixelShader *pixelShader;			// Pixel Shader
}D3DCONTEXT;

// void SafeRelease(IUnknown *object)
// {
// 	if (object)
// 	{
// 		object->Release();
// 		object = nullptr;
// 	}
// }
