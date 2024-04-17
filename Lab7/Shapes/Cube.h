#pragma once

#include "Shapes.h"
#include <DirectXCollision.h>

class Cube : public InstanceShape
{
private:
	struct GeomBuffer
	{
		DirectX::XMMATRIX modelMatrix;
		DirectX::XMMATRIX normalTransform;
	};

	struct VisibleIndexes
	{
		DirectX::XMUINT4 idx;
	};

	struct TextureNum
	{
		DirectX::XMINT4 i;
	};

	struct CullParams
	{
		DirectX::XMINT4 numShapes;
		DirectX::XMFLOAT4 bbMin[1200];
		DirectX::XMFLOAT4 bbMax[1200];
	};

	struct Vertex
	{
		float x, y, z;
		float tx, ty, tz;
		float nx, ny, nz;
		float u, v;
	};
public:
	HRESULT CreateGeometry(ID3D11Device* m_pDevice) final;
	HRESULT CreateShaders(ID3D11Device* m_pDevice) final;
	HRESULT CreateTextures(ID3D11Device* m_pDevice) final;
	void Draw(const DirectX::XMMATRIX& projMatrix, const DirectX::XMMATRIX& viewMatrix,
		ID3D11DeviceContext* m_pDeviceContext) final;
	void RenderImGUI();

	void setDebug();
	int drawMode = 2;
	bool startAnalyzing = 0;
private:
	DirectX::BoundingFrustum frustum;
	bool debugMode = false;
	void ReadQueries(ID3D11DeviceContext* m_pDeviceContext);
	std::vector<GeomBuffer> geomBuffers;
	std::vector<VisibleIndexes> visibleIndBuffer;
	std::vector<TextureNum> texNumBuffers;
	std::vector<float> rotateSpeed;
	std::vector<float> rotateAngle;
	int visibleObjectNum = 0;
	int m_curFrame = 0;
	int m_lastCompletedFrame = 0;

	CullParams clParams;
};