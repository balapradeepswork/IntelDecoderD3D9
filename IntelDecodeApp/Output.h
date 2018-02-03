#pragma once
#include <d3d9.h>
#include "mfxstructures.h"

class OutputManager
{
private:
	CRITICAL_SECTION  m_critial;

	IDirect3D9 *m_pDirect3D9 = NULL;
	IDirect3DDevice9 *m_pDirect3DDevice = NULL;
	IDirect3DSurface9 *m_pDirect3DSurfaceRender = NULL;
	RECT m_rtViewport;

	RECT desktop;
public:
	//Methods
	OutputManager();
	~OutputManager();
	void Cleanup();
	int InitD3D(HWND hwnd, unsigned long lWidth, unsigned long lHeight);
	bool Render(mfxFrameSurface1* pSurface);
};