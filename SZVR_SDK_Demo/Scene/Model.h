#pragma once

#include "../Types.h"
class Model
{
public:
	Model();
	
	virtual void SetShaders(ID3D11VertexShader *vs, ID3D11PixelShader *ps) = 0;
	virtual void Render() = 0;
	virtual void Create(float x1, float y1, float z1, float x2, float y2, float z2, XMFLOAT4 color);
	virtual void Create(float x1, float y1, float z1, float x2, float y2, float z2, const wchar_t *filename);
	~Model();
};

