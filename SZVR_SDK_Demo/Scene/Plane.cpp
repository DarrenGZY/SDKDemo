#include "Plane.h"


#define CUBE_INDICES 36
Plane::Plane(D3DCONTEXT context)
:
m_ibuffer(nullptr),
m_vbuffer(nullptr),
m_inputLayout(nullptr),
m_SRV(nullptr),
m_vShader(nullptr),
m_pShader(nullptr)
{
	m_context = context;
}

void Plane::Create(float x1, float y1, float z1, float x2, float y2, float z2, const wchar_t *filename)
{
	if (x1 > x2)
	{
		float tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if (y1 > y2)
	{
		float tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	if (z1 > z2)
	{
		float tmp = z1;
		z1 = z2;
		z2 = tmp;
	}

	D3D11_INPUT_ELEMENT_DESC Layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT NumElements = ARRAYSIZE(Layout);
	HRESULT hr = m_context.device->CreateInputLayout(Layout, NumElements, g_TexVS, ARRAYSIZE(g_TexVS), &m_inputLayout);

	TEXVERTEX vertices[] =
	{
		{ XMFLOAT3(x1, y1, z2), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },	// front side
		{ XMFLOAT3(x2, y1, z2), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(x1, y2, z2), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(x2, y2, z2), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },

		{ XMFLOAT3(x1, y1, z1), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },	// back side
		{ XMFLOAT3(x1, y2, z1), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(x2, y1, z1), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(x2, y2, z1), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },

		{ XMFLOAT3(x1, y2, z1), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },	// top side
		{ XMFLOAT3(x1, y2, z2), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(x2, y2, z1), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(x2, y2, z2), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },

		{ XMFLOAT3(x1, y1, z1), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },	// bottom side
		{ XMFLOAT3(x2, y1, z1), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(x1, y1, z2), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(x2, y1, z2), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },

		{ XMFLOAT3(x2, y1, z1), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },	// right side
		{ XMFLOAT3(x2, y2, z1), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(x2, y1, z2), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(x2, y2, z2), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },

		{ XMFLOAT3(x1, y1, z1), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },	// left side
		{ XMFLOAT3(x1, y1, z2), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(x1, y2, z1), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(x1, y2, z2), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
	};


	D3D11_BUFFER_DESC BufferDesc;
	RtlZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.ByteWidth = sizeof(TEXVERTEX)* ARRAYSIZE(vertices);
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	RtlZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	m_context.device->CreateBuffer(&BufferDesc, &InitData, &m_vbuffer);

	DWORD indices[CUBE_INDICES] =
	{
		0, 1, 2,    // front side
		2, 1, 3,
		4, 5, 6,    // back side
		6, 5, 7,
		8, 9, 10,    // top side
		10, 9, 11,
		12, 13, 14,    // bottom side
		14, 13, 15,
		16, 17, 18,    // right side
		18, 17, 19,
		20, 21, 22,    // left side
		22, 21, 23,
	};

	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.ByteWidth = sizeof(DWORD)* CUBE_INDICES;
	BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	BufferDesc.CPUAccessFlags = 0;

	RtlZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices;

	m_context.device->CreateBuffer(&BufferDesc, &InitData, &m_ibuffer);

	hr	= CreateWICTextureFromFile(m_context.device, m_context.devcontext, filename, nullptr, &m_SRV, NULL);
	
}


void Plane::Render()
{
	
	m_context.devcontext->IASetInputLayout(m_inputLayout);
	UINT stride = sizeof(TEXVERTEX);
	UINT offset = 0;
	m_context.devcontext->IASetVertexBuffers(0, 1, &m_vbuffer, &stride, &offset);
	m_context.devcontext->IASetIndexBuffer(m_ibuffer, DXGI_FORMAT_R32_UINT, 0);
	m_context.devcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_context.devcontext->VSSetShader(m_vShader, nullptr, NULL);
	m_context.devcontext->PSSetShader(m_pShader, nullptr, NULL);
	m_context.devcontext->PSSetShaderResources(0, 1, &m_SRV);
	m_context.devcontext->DrawIndexed(CUBE_INDICES, 0, 0);

}

void Plane::SetShaders(ID3D11VertexShader* vs, ID3D11PixelShader* ps)
{
	m_pShader = ps;
	m_vShader = vs;
}

Plane::~Plane()
{
}
