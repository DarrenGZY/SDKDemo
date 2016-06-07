#include "Plane.h"


Plane::Plane(D3DCONTEXT context)
{
	m_context = context;
	m_ibuffer = nullptr;
	m_vbuffer = nullptr;
	m_inputLayout = nullptr;
	m_SRV = nullptr;
}


Plane::~Plane()
{
}
