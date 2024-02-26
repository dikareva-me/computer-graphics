#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <directxmath.h>
#include <string>
#include "camera.h"
#include "input.h"

using namespace DirectX;

struct Vertex {
    float x, y, z;
    COLORREF color;
};

struct WorldMatrixBuffer {
    XMMATRIX mWorldMatrix;
};

struct SceneMatrixBuffer {
    XMMATRIX mViewProjectionMatrix;
};

class Renderer {
public:
    bool Init(HINSTANCE hInstance, HWND hWnd);
    bool Frame(); 
    bool Render();
    void Cleanup();
    bool Resize(UINT width, UINT height);

private:
    HRESULT SetupBackBuffer();
    HRESULT InitScene();
    void HandleMovementInput();

    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;
    IDXGISwapChain* m_pSwapChain = nullptr;
    ID3D11RenderTargetView* m_pBackBufferRTV = nullptr;
    UINT m_width = 0;
    UINT m_height = 0;

    ID3D11Buffer* m_pWorldMatrixBuffer = nullptr;
    ID3D11Buffer* m_pSceneMatrixBuffer = nullptr;
    ID3D11RasterizerState* m_pRasterizerState = nullptr;
    ID3D11Buffer* m_pVertexBuffer = nullptr;
    ID3D11Buffer* m_pIndexBuffer = nullptr;
    ID3D11InputLayout* m_pInputLayout = nullptr;
    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11PixelShader* m_pPixelShader = nullptr;

    Camera* m_pCamera = nullptr;
    Input* m_pInput = nullptr;
};