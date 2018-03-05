//--------------------------------------------------------------------------------------
// File: EmptyProject11.cpp
//
// Empty starting point for new Direct3D 9 and/or Direct3D 11 applications
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "DXUTcamera.h"
#include "SDKmisc.h"
#include "LoopSubdivisionStruct.h"

using namespace std;
#pragma comment(lib, "legacy_stdio_definitions.lib")




struct MatrixBufferType
{
	D3DXMATRIX world;
	D3DXMATRIX view;
	D3DXMATRIX projection;
};
vector<Triangle*>           triangleList;
vector<Edge*>               edgeList;
vector<VertexType*>         tempVertexBuffer;
ID3D11VertexShader*         g_pVertexShader = NULL;
ID3D11PixelShader*          g_pPixelShader = NULL;
ID3D11InputLayout*          g_layout;
ID3D11Buffer                *g_vertexBuffer;
ID3D11Buffer                *g_indexBuffer;
ID3D11Buffer                *g_matrixBuffer;
CModelViewerCamera          g_Camera;
int                         g_indexCount;
D3D11_FILL_MODE             g_fillMode = D3D11_FILL_WIREFRAME;
int                         g_iterTimes = 8;//建议本数值小于8


void initVertexBuffers() {
	triangleList.clear();
	edgeList.clear();
	tempVertexBuffer.clear();

	VertexType *vertex1, *vertex2, *vertex3, *vertex4;
	vertex1 = new VertexType; vertex2 = new VertexType; vertex3 = new VertexType; vertex4 = new VertexType;
	
	vertex1->position = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
	vertex1->color = myDefaultColor;
	tempVertexBuffer.push_back(vertex1);

	vertex2->position = D3DXVECTOR3(0.0f, 2.0f, 0.0f);
	vertex2->color = myDefaultColor;
	tempVertexBuffer.push_back(vertex2);

	vertex3->position = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	vertex3->color = myDefaultColor;
	tempVertexBuffer.push_back(vertex3);

	vertex4->position = D3DXVECTOR3(0.0f, 0.0f, -2.0f);
	vertex4->color = myDefaultColor;
	tempVertexBuffer.push_back(vertex4);

	Edge *e1, *e2, *e3, *e4, *e5, *e6;
	e1 = new Edge; e2 = new Edge; e3 = new Edge; e4 = new Edge; e5 = new Edge; e6 = new Edge;;
	e1->v1 = 0;
	e1->v2 = 1;//左上边

	e2->v1 = 0;
	e2->v2 = 2;//前下边

	e3->v1 = 1;
	e3->v2 = 2;//右上边

	e4->v1 = 0;
	e4->v2 = 3;//左下边

	e5->v1 = 3;
	e5->v2 = 2;//右下边

	e6->v1 = 3;
	e6->v2 = 1;//后边
	edgeList.push_back(e1);
	edgeList.push_back(e2);
	edgeList.push_back(e3);
	edgeList.push_back(e4);
	edgeList.push_back(e5);
	edgeList.push_back(e6);

	Triangle *t1, *t2, *t3, *t4;
	t1 = new Triangle; t2 = new Triangle; t3 = new Triangle; t4 = new Triangle;;
	t1->e1 = e1;
	t1->e2 = e2;
	t1->e3 = e3;//前面

	t2->e1 = e1;
	t2->e2 = e4;
	t2->e3 = e6;//左后面

	t3->e1 = e3;
	t3->e2 = e5;
	t3->e3 = e6;

	t4->e1 = e2;
	t4->e2 = e4;
	t4->e3 = e5;
	triangleList.push_back(t1);
	triangleList.push_back(t2);
	triangleList.push_back(t3);
	triangleList.push_back(t4);
	for (int i = 0; i < g_iterTimes; i++) {
		doLoopSubdivision(&triangleList, &edgeList, &tempVertexBuffer);
	}
}


//--------------------------------------------------------------------------------------
// Reject any D3D11 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
                                       DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D9 or D3D11 device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                      void* pUserContext )
{
	ID3DBlob* pErrorBlob;
	ID3DBlob* pVertexShaderBuffer = NULL;
	ID3DBlob* pPixelShaderBuffer = NULL;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc, matrixBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	VertexType *finalVertexBuffer;
	unsigned int *finalIndexBuffer;
	int i = 0;

	initVertexBuffers();

	finalVertexBuffer = new VertexType[tempVertexBuffer.size()];
	for (i = 0; i < tempVertexBuffer.size(); i++) {
		finalVertexBuffer[i].position = tempVertexBuffer[i]->position;
		finalVertexBuffer[i].color = tempVertexBuffer[i]->color;
	}
	g_indexCount = 3 * triangleList.size();
	finalIndexBuffer = new unsigned int[g_indexCount];
	unsigned int index1, index2, index3;
	i = 0;
	for (auto trianIter = triangleList.begin(); trianIter != triangleList.end(); trianIter++) {
		index1 = (*trianIter)->e1->v1;
		index2 = (*trianIter)->e1->v2;
		if ((*trianIter)->e2->v1 == index1 || (*trianIter)->e2->v1 == index2) {
			index3 = (*trianIter)->e2->v2;
		}
		else {
			index3 = (*trianIter)->e2->v1;
		}
		finalIndexBuffer[i++] = index1;
		finalIndexBuffer[i++] = index2;
		finalIndexBuffer[i++] = index3;
	}


	// find the file
	WCHAR str[MAX_PATH];
	DXUTFindDXSDKMediaFileCch(str, MAX_PATH, L"VertexShader.hlsl");
	D3DX11CompileFromFile(str, NULL, NULL, "VertexShaderMain","vs_5_0",
		D3DCOMPILE_DEBUG, 0, NULL, &pVertexShaderBuffer, &pErrorBlob, NULL);
	DXUTFindDXSDKMediaFileCch(str, MAX_PATH, L"PixelShader.hlsl");
	D3DX11CompileFromFile(str, NULL, NULL, "PixelShaderMain", "ps_5_0",
		 D3DCOMPILE_DEBUG, 0, NULL, &pPixelShaderBuffer, &pErrorBlob, NULL);

	pd3dDevice->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(), NULL, &g_pVertexShader);
	pd3dDevice->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(),
		pPixelShaderBuffer->GetBufferSize(), NULL, &g_pPixelShader);

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)*tempVertexBuffer.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	vertexData.pSysMem = finalVertexBuffer;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int)*g_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = finalIndexBuffer;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	pd3dDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &g_vertexBuffer);
	pd3dDevice->CreateBuffer(&indexBufferDesc, &indexData, &g_indexBuffer);
	pd3dDevice->CreateBuffer(&matrixBufferDesc, NULL, &g_matrixBuffer);

	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	pd3dDevice->CreateInputLayout(layout, ARRAYSIZE(layout), pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(), &g_layout);

	delete[] finalVertexBuffer;
	delete[] finalIndexBuffer;

	D3DXVECTOR3 vecEye(0.0f, 0.0f, -5.0f);
	D3DXVECTOR3 vecAt(0.0f, 0.0f, 0.0f);
	g_Camera.SetViewParams(&vecEye, &vecAt);
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                          const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	// Setup the camera's projection parameters
	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
	g_Camera.SetProjParams(D3DX_PI / 3, fAspectRatio, 0.5f, 10.0f);
	g_Camera.SetWindow(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	g_Camera.FrameMove(fElapsedTime);
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
                                  double fTime, float fElapsedTime, void* pUserContext )
{
    // Clear render target and the depth stencil 
    float ClearColor[4] = { 0.176f, 0.196f, 0.667f, 0.0f };
	D3DXMATRIX mWorld;
	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	MatrixBufferType* dataPtr;

	mProj = *g_Camera.GetProjMatrix();
	mView = *g_Camera.GetViewMatrix();
	mWorld = *g_Camera.GetWorldMatrix();


    ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView();
    ID3D11DepthStencilView* pDSV = DXUTGetD3D11DepthStencilView();
    pd3dImmediateContext->ClearRenderTargetView( pRTV, ClearColor );
    pd3dImmediateContext->ClearDepthStencilView( pDSV, D3D11_CLEAR_DEPTH, 1.0, 0 );

	D3D11_RASTERIZER_DESC rsDesc;
	ID3D11RasterizerState *IrsDesc;
	rsDesc.AntialiasedLineEnable = false;
	rsDesc.CullMode = D3D11_CULL_NONE; // 设置两面都画
	rsDesc.FillMode = g_fillMode;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0.0f;
	rsDesc.DepthClipEnable = true;
	rsDesc.FrontCounterClockwise = false;
	rsDesc.MultisampleEnable = false;
	rsDesc.ScissorEnable = false;
	rsDesc.SlopeScaledDepthBias = 0.0f;
	pd3dDevice->CreateRasterizerState(&rsDesc, &IrsDesc);
	pd3dImmediateContext->RSSetState(IrsDesc);

	pd3dImmediateContext->Map(g_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	dataPtr = (MatrixBufferType*)MappedResource.pData;
	D3DXMatrixTranspose(&dataPtr->world, &mWorld);
	D3DXMatrixTranspose(&dataPtr->view, &mView);
	D3DXMatrixTranspose(&dataPtr->projection, &mProj);
	pd3dImmediateContext->Unmap(g_matrixBuffer, 0);

	pd3dImmediateContext->VSSetConstantBuffers(0, 1, &g_matrixBuffer);
	pd3dImmediateContext->IASetInputLayout(g_layout);
	pd3dImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
	pd3dImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);






	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;
	pd3dImmediateContext->IASetVertexBuffers(0, 1, &g_vertexBuffer, &stride, &offset);
	pd3dImmediateContext->IASetIndexBuffer(g_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dImmediateContext->DrawIndexed(g_indexCount, 0, 0);
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext )
{
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice( void* pUserContext )
{
}


//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                          bool* pbNoFurtherProcessing, void* pUserContext )
{
	g_Camera.HandleMessages(hWnd, uMsg, wParam, lParam);
    return 0;
}


//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
	if (bKeyDown)
	{
		switch (nChar)
		{
		case VK_F1:
			g_fillMode = D3D11_FILL_WIREFRAME;
			break;
		case VK_F2:
			g_fillMode = D3D11_FILL_SOLID;
			break;
		}


	}
}


//--------------------------------------------------------------------------------------
// Handle mouse button presses
//--------------------------------------------------------------------------------------
void CALLBACK OnMouse( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
                       bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
                       int xPos, int yPos, void* pUserContext )
{
}


//--------------------------------------------------------------------------------------
// Call if device was removed.  Return true to find a new device, false to quit
//--------------------------------------------------------------------------------------
bool CALLBACK OnDeviceRemoved( void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // DXUT will create and use the best device (either D3D9 or D3D11) 
    // that is available on the system depending on which D3D callbacks are set below

    // Set general DXUT callbacks
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackKeyboard( OnKeyboard );
    DXUTSetCallbackMouse( OnMouse );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
    DXUTSetCallbackDeviceRemoved( OnDeviceRemoved );


    // Set the D3D11 DXUT callbacks. Remove these sets if the app doesn't need to support D3D11
    DXUTSetCallbackD3D11DeviceAcceptable( IsD3D11DeviceAcceptable );
    DXUTSetCallbackD3D11DeviceCreated( OnD3D11CreateDevice );
    DXUTSetCallbackD3D11SwapChainResized( OnD3D11ResizedSwapChain );
    DXUTSetCallbackD3D11FrameRender( OnD3D11FrameRender );
    DXUTSetCallbackD3D11SwapChainReleasing( OnD3D11ReleasingSwapChain );
    DXUTSetCallbackD3D11DeviceDestroyed( OnD3D11DestroyDevice );

    // Perform any application-level initialization here


    DXUTInit( true, true, NULL ); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"EmptyProject11" );

    // Only require 10-level hardware
    DXUTCreateDevice( D3D_FEATURE_LEVEL_10_0, true, 640, 480 );
    DXUTMainLoop(); // Enter into the DXUT ren  der loop

    // Perform any application-level cleanup here

    return DXUTGetExitCode();
}


