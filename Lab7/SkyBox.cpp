#include "SkyBox.h"

#include "SkyBox.h"
#include "DDSTextureLoader11.h"


UINT32 Up(UINT32 a, UINT32 b)
{
    return (a + b - 1) / b;
}
HRESULT SkyBox::CreateGeometry(ID3D11Device* m_pDevice)
{
    static std::vector<Vertex> Vertices;

    static std::vector<DWORD> Indices;

    int stackCount = 20;
    int sliceCount = 20;
    float phiStep = DirectX::XM_PI / stackCount;
    float thetaStep = 2.0f * DirectX::XM_PI / sliceCount;
    float radius = 0.5f;
    Vertices.push_back({ 0, radius, 0 });

    for (int i = 1; i < stackCount; i++)
    {
        float phi = i * phiStep;
        for (int j = 0; j <= sliceCount; j++)
        {
            float theta = j * thetaStep;
            Vertex v = { static_cast<float>(radius * sin(phi) * cos(theta)),
                                static_cast<float>(radius * cos(phi)),
                                static_cast<float>(radius * sin(phi) * sin(theta) )};
            Vertices.push_back(v);
        }
    }
    Vertices.push_back({ 0, -radius, 0 });

    for (int i = 1; i <= sliceCount; i++) {
        Indices.push_back(0);
        Indices.push_back(i + 1);
        Indices.push_back(i);
    }

    int baseIndex = 1;
    int ringVertexCount = sliceCount + 1;
    for (int i = 0; i < stackCount - 2; i++) {
        for (int j = 0; j < sliceCount; j++) {
            Indices.push_back(baseIndex + i * ringVertexCount + j);
            Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
            Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

            Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
            Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
            Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
        }
    }

    int southPoleIndex = static_cast<int>(Vertices.size() - 1);
    baseIndex = southPoleIndex - ringVertexCount;
    for (int i = 0; i < sliceCount; i++) {
        Indices.push_back(southPoleIndex);
        Indices.push_back(baseIndex + i);
        Indices.push_back(baseIndex + i + 1);
    }

    D3D11_SUBRESOURCE_DATA data;

    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = static_cast<UINT>(Vertices.size() * sizeof(Vertex));
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    data.pSysMem = Vertices.data();
    data.SysMemPitch = static_cast<UINT>(Vertices.size() * sizeof(Vertex));
    data.SysMemSlicePitch = 0;

    HRESULT result = m_pDevice->CreateBuffer(&desc, &data, &m_pVertextBuffer);

    if (SUCCEEDED(result))
    {
        numIndeces = static_cast<int>(Indices.size());
        desc = {};
        desc.ByteWidth = static_cast<UINT>(Indices.size() * sizeof(DWORD));
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        data.pSysMem = Indices.data();
        data.SysMemPitch = static_cast<UINT>(Indices.size() * sizeof(DWORD));
        data.SysMemSlicePitch = 0;

        result = m_pDevice->CreateBuffer(&desc, &data, &m_pIndexBuffer);
    }

    ID3D11Buffer* m_pSceneBuffer;

    if (SUCCEEDED(result))
    {
        desc = { 0 };
        desc.ByteWidth = sizeof(SceneBuffer);
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        result = m_pDevice->CreateBuffer(&desc, NULL, &m_pSceneBuffer);
    }

    if (SUCCEEDED(result))
    {
        constBuffers.push_back(m_pSceneBuffer);
    }

    ID3D11Buffer* m_pGeomBuffer;

    if (SUCCEEDED(result))
    {
        desc = { 0 };
        desc.ByteWidth = sizeof(GeomBuffer);
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        result = m_pDevice->CreateBuffer(&desc, NULL, &m_pGeomBuffer);
    }

    if (SUCCEEDED(result))
    {
        constBuffers.push_back(m_pGeomBuffer);
    }

    return result;
}

HRESULT SkyBox::CreateShaders(ID3D11Device* m_pDevice)
{
    static const D3D11_INPUT_ELEMENT_DESC InputDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    if (!vs.Initialize(m_pDevice, L"SkyBoxVS.hlsl", nullptr))
    {
        return S_FALSE;
    }

    HRESULT result = m_pDevice->CreateInputLayout(InputDesc, ARRAYSIZE(InputDesc), vs.GetBuffer()->GetBufferPointer(), vs.GetBuffer()->GetBufferSize(), &m_pInputLayout);
    if (!ps.Initialize(m_pDevice, L"SkyBoxPS.hlsl", nullptr))
    {
        return S_FALSE;
    }

    return result;
}
inline HRESULT SetResourceName(ID3D11DeviceChild* pDevice, const std::string& name)
{
    return pDevice->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)name.length(), name.c_str());
}


HRESULT SkyBox::CreateTextures(ID3D11Device* m_pDevice)
{

    HRESULT result;
    DXGI_FORMAT textureFmt;
    {
        D3D11_SAMPLER_DESC desc = {};
        desc.Filter = D3D11_FILTER_ANISOTROPIC;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.MinLOD = -FLT_MAX;
        desc.MaxLOD = FLT_MAX;
        desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 16;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 1.0f;
        result = m_pDevice->CreateSamplerState(&desc, &m_pTextureSampler);
        assert(SUCCEEDED(result));
        if (SUCCEEDED(result)) {
            result = SetResourceName(m_pTextureSampler, "TextureSampler");
        }
    }
    {
        const std::wstring TextureNames[6] = {
           L"cubemap/posx.dds", L"cubemap/negx.dds", L"cubemap/posy.dds",
        L"cubemap/negy.dds", L"cubemap/posz.dds", L"cubemap/negz.dds"
        };
        TextureDesc texDescs[6];
        bool ddsRes = true;
        for (int i = 0; i < 6 && ddsRes; i++)
        {
            ddsRes = LoadDDS(TextureNames[i].c_str(), texDescs[i]);
        }
        textureFmt = texDescs[0].fmt; // Assume all are the same
        D3D11_TEXTURE2D_DESC desc = {};
        desc.Format = textureFmt;
        desc.ArraySize = 6;
        desc.MipLevels = 1;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Height = texDescs[0].height;
        desc.Width = texDescs[0].width;
        UINT32 blockWidth = Up(desc.Width, 4u);
        UINT32 blockHeight = Up(desc.Height, 4u);
        UINT32 pitch = blockWidth * UINT32(GetBytesPerBlock(desc.Format));
        D3D11_SUBRESOURCE_DATA data[6];
        for (int i = 0; i < 6; i++) {
            data[i].pSysMem = texDescs[i].pData;
            data[i].SysMemPitch = pitch;
            data[i].SysMemSlicePitch = 0;
        }
        result = m_pDevice->CreateTexture2D(&desc, data, &m_pCubemapTexture);
        assert(SUCCEEDED(result));
        if (SUCCEEDED(result)) {
            result = SetResourceName(m_pCubemapTexture, "CubemapTexture");
        }
    }
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC desc;
        desc.Format = textureFmt;
        desc.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
        desc.TextureCube.MipLevels = 1;
        desc.TextureCube.MostDetailedMip = 0;

        result = m_pDevice->CreateShaderResourceView(m_pCubemapTexture, &desc, &m_pCubemapTextureView);
        assert(SUCCEEDED(result));
        resources.push_back(m_pCubemapTextureView);
    }

    if (SUCCEEDED(result))
    {
        samplers.push_back(m_pTextureSampler);
    }
    return result;

}

void SkyBox::Draw(const DirectX::XMMATRIX& vp, ID3D11DeviceContext* m_pDeviceContext)
{
    m_pDeviceContext->RSSetState(rasterizerState);

    model = DirectX::XMMatrixIdentity();
    model = scaleMatrix;
    geomBuffer.modelMatrix = model;
    geomBuffer.modelMatrix = DirectX::XMMatrixTranspose(geomBuffer.modelMatrix);
    geomBuffer.radius = DirectX::XMVectorSet(radius, 0.0f, 0.0f, 0.0f);
    scBuffer.vp = vp;
    scBuffer.vp = DirectX::XMMatrixTranspose(scBuffer.vp);
    scBuffer.cameraPos = camPos;
    m_pDeviceContext->UpdateSubresource(constBuffers[0], 0, nullptr, &scBuffer, 0, 0);
    m_pDeviceContext->UpdateSubresource(constBuffers[1], 0, nullptr, &geomBuffer, 0, 0);

    m_pDeviceContext->IASetInputLayout(m_pInputLayout);

    m_pDeviceContext->VSSetShader(vs.GetShader(), NULL, 0);
    m_pDeviceContext->PSSetShader(ps.GetShader(), NULL, 0);

    m_pDeviceContext->VSSetConstantBuffers(0, 2, constBuffers.data());

    if (!samplers.empty() && !resources.empty())
    {
        m_pDeviceContext->PSSetSamplers(0, 1, samplers.data());

        m_pDeviceContext->PSSetShaderResources(0, 1, resources.data());
    }

    m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertextBuffer, &stride, &offset);

    m_pDeviceContext->DrawIndexed(numIndeces, 0, 0);
}

void SkyBox::setCamPos(DirectX::XMVECTOR camPos)
{
    this->camPos = camPos;
}

void SkyBox::setRadius(const float& fov, const float& nearPlane, const float& width, const float& height)
{
    float halfW = tanf(fov / 2) * nearPlane;
    float halfH = float(height / width) * halfW;
    radius = sqrtf(nearPlane * nearPlane + halfH * halfH + halfW * halfW) * 1.1f;
}

HRESULT SkyBox::setRasterizerState(ID3D11Device* m_pDevice, D3D11_CULL_MODE cullMode)
{
    HRESULT result;
    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

    rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = cullMode;
    result = m_pDevice->CreateRasterizerState(&rasterizerDesc, &rasterizerState);

    return result;
}

void SkyBox::Clean()
{
    vs.Clean();
    ps.Clean();
    SAFE_RELEASE(m_pIndexBuffer);
    SAFE_RELEASE(m_pVertextBuffer);
    for (auto& buf : constBuffers)
    {
        SAFE_RELEASE(buf);
    }
    for (auto& res : resources)
    {
        SAFE_RELEASE(res);
    }
    for (auto& sampler : samplers)
    {
        SAFE_RELEASE(sampler);
    }
    SAFE_RELEASE(m_pInputLayout);
    SAFE_RELEASE(m_pIndexBuffer);
    SAFE_RELEASE(m_pIndexBuffer);
    SAFE_RELEASE(rasterizerState);
}
