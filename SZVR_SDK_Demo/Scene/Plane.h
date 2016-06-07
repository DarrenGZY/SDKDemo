#pragma once
class Plane
{
public:
	Plane();

	Create(float x1, float y1, float z1, float x2, float y2, float z2, const wchar_t *filename);
	Render();

	~Plane();

private:
	D3DCONTEXT m_context;

	ID3D11Buffer *m_ibuffer;					// Index Buffer
	ID3D11Buffer *m_vbuffer;					// Vertex Buffer
	ID3D11InputLayout *m_inputLayout;			// Input Layout
	ID3D11ShaderResourceView *m_SRV;			// Shader Resource View
};

