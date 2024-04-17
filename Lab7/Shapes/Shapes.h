#pragma once

#include "../Shaders.h"

#include <d3d11.h>
#include <DirectXMath.h>

#include <vector>

#pragma comment(lib, "d3d11")

#define SAFE_RELEASE(p) if (p != NULL) { p->Release(); p = NULL; }
#define DivUp(x, y) (x % y) ? x / y + 1 : x / y

class InstanceShape
{
protected:
	struct SceneBuffer
	{
		DirectX::XMMATRIX vp;
		DirectX::XMVECTOR cameraPos;
		DirectX::XMVECTOR frustum[6];
	};
public:
	virtual HRESULT CreateGeometry(ID3D11Device* m_pDevice) = 0;
	virtual HRESULT CreateShaders(ID3D11Device* m_pDevice) = 0;
	virtual HRESULT CreateTextures(ID3D11Device* m_pDevice) = 0;
	virtual void Draw(const DirectX::XMMATRIX& projMatrix, const DirectX::XMMATRIX& viewMatrix,
		ID3D11DeviceContext* m_pDeviceContext) = 0;

	HRESULT setRasterizerState(ID3D11Device* m_pDevice, D3D11_CULL_MODE cullMode);
	void addInstance();
	void addHundredInstances();

	void Translate(DirectX::XMMATRIX translateMatrix, int ind);
	void Scale(DirectX::XMMATRIX scaleMatrix, int ind);
	void Rotate(DirectX::XMMATRIX rotateMatrix, int ind);

	int getNumInstances();

	void Clean();
	int numInstances = 0;
protected:
	ID3D11Buffer* m_pIndexBuffer = NULL;
	ID3D11Buffer* m_pVertextBuffer = NULL;

	ID3D11Buffer* m_pIndirectArgsSrc;
	ID3D11Buffer* m_pIndirectArgs;
	ID3D11UnorderedAccessView* m_pIndirectArgsUAV;

	ID3D11Buffer* m_pGeomBufferInstVisGPU;
	ID3D11Buffer* m_pGeomBufferInstVis;
	ID3D11UnorderedAccessView* m_pGeomBufferInstVisGPU_UAV;

	ID3D11Query* m_queries[10];

	ID3D11RasterizerState* rasterizerState;

	VertexShader vs;
	PixelShader ps;
	ComputeShader cs;
	ID3D11InputLayout* m_pInputLayout = NULL;

	std::vector<ID3D11Buffer*> constBuffers;
	std::vector<ID3D11SamplerState*> samplers;
	std::vector<ID3D11ShaderResourceView*> resources;

	std::vector<DirectX::XMMATRIX> translateMatrices;
	std::vector<DirectX::XMMATRIX> scaleMatrices;
	std::vector<DirectX::XMMATRIX> rotateMatrices;

	SceneBuffer scBuffer;

	int maxInstancesNum = 1200;
};