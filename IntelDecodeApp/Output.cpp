#include "Output.h"

OutputManager::OutputManager()
{
}

OutputManager::~OutputManager()
{
}

void OutputManager::Cleanup()
{
	if (&m_critial)
	{
		EnterCriticalSection(&m_critial);
		if (m_pDirect3DSurfaceRender)
			m_pDirect3DSurfaceRender->Release();
		if (m_pDirect3DDevice)
			m_pDirect3DDevice->Release();
		if (m_pDirect3D9)
			m_pDirect3D9->Release();
		LeaveCriticalSection(&m_critial);
	}
}


int OutputManager::InitD3D(HWND hwnd, unsigned long lWidth, unsigned long lHeight)
{
	HRESULT lRet;
	InitializeCriticalSection(&m_critial);
	Cleanup();

	m_pDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pDirect3D9 == NULL)
		return -1;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	GetClientRect(hwnd, &m_rtViewport);

	lRet = m_pDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &m_pDirect3DDevice);
	if (FAILED(lRet))
		return -1;

	desktop.left = 0;
	desktop.right = lWidth;
	desktop.bottom = lHeight;
	desktop.top = 0;

	lRet = m_pDirect3DDevice->CreateOffscreenPlainSurface(
		lWidth, lHeight,
		(D3DFORMAT)MAKEFOURCC('N', 'V', '1', '2'),
		D3DPOOL_DEFAULT,
		&m_pDirect3DSurfaceRender,
		NULL);


	if (FAILED(lRet))
		return -1;

	return 0;
}

bool OutputManager::Render(mfxFrameSurface1* pSurface)
{
	HRESULT lRet;

	if (m_pDirect3DSurfaceRender == NULL)
		return -1;
	if (m_pDirect3DDevice == NULL)
		return -1;
	//m_pDirect3DDevice->UpdateSurface((IDirect3DSurface9*)pSurface->Data.MemId, NULL, m_pDirect3DSurfaceRender, NULL);

	D3DLOCKED_RECT d3d_rect;
	lRet = m_pDirect3DSurfaceRender->LockRect(&d3d_rect, NULL, D3DLOCK_DONOTWAIT);
	if (FAILED(lRet))
		return -1;

	byte * pDest = (BYTE *)d3d_rect.pBits;
	int stride = d3d_rect.Pitch;

	//memcpy(pDest, pSurface->Data.Y, pSurface->Info.Height*stride*3/2);

	for (int i = 0; i < pSurface->Info.CropH; i++)
	{
		//mfxU8* y = pData->Y + (pInfo->CropY * pData->Pitch + pInfo->CropX) + i * pData->Pitch;
		memcpy(pDest + stride * i, pSurface->Data.Y + pSurface->Data.Pitch * i, pSurface->Info.CropW);
	}

	for (int i = 0; i < pSurface->Info.CropH / 2; i++)
	{
		memcpy(pDest + stride * pSurface->Info.CropH + stride * i, pSurface->Data.UV + pSurface->Data.Pitch * i, pSurface->Info.CropW);
	}

	lRet = m_pDirect3DSurfaceRender->UnlockRect();
	if (FAILED(lRet))
		return -1;

	

	m_pDirect3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	m_pDirect3DDevice->BeginScene();
	IDirect3DSurface9 * pBackBuffer = NULL;

	m_pDirect3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	m_pDirect3DDevice->StretchRect(m_pDirect3DSurfaceRender, NULL, pBackBuffer, &m_rtViewport, D3DTEXF_LINEAR);
	m_pDirect3DDevice->EndScene();
	m_pDirect3DDevice->Present(NULL, NULL, NULL, NULL);
	pBackBuffer->Release();

	return true;
}