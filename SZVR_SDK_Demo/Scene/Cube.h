#pragma once

#include "../Types.h"
class Cube
{
public:
	Cube(D3DCONTEXT context);

	void Create(float x1, float y1, float z1, float x2, float y2, float z2, XMFLOAT4 color);
	void Render();
	void Cube::SafeRelease(IUnknown *object);

	~Cube();

private:
	D3DCONTEXT m_context;

	ID3D11Buffer *m_ibuffer;					// Index Buffer
	ID3D11Buffer *m_vbuffer;					// Vertex Buffer
	ID3D11InputLayout *m_inputLayout;			// Input Layout
};

