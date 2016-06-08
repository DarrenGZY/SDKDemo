#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include "Util/WICTextureLoader.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "TexVShader.h"
#include "TexPShader.h"

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

typedef struct _TEXVERTEX
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
}TEXVERTEX;

typedef struct _D3DCONTEXT
{
	ID3D11Device *device;						// Device
	ID3D11DeviceContext *devcontext;			// Device Context
}D3DCONTEXT;

// void SafeRelease(IUnknown *object)
// {
// 	if (object)
// 	{
// 		object->Release();
// 		object = nullptr;
// 	}
// }
