struct PSInput
{
	float4 pos : SV_Position;
	float2 uv : TEXCOORD;
};

cbuffer PostProcBuffer : register (b0)
{
	float4 contrastAlpha;
	int4 postProcOper;
};

Texture2D colorTexture : register(t0);
SamplerState colorSampler : register(s0);

float4 main(PSInput input) : SV_Target0
{
	float3 color = colorTexture.Sample(colorSampler, input.uv).xyz;

	if (postProcOper[0] == 1)
	{
		color[0] = (color[0] * 0.393f) + (color[1] * 0.769f) + (color[2] * 0.189f);
		color[1] = (color[0] * 0.349f) + (color[1] * 0.686f) + (color[2] * 0.168f);
		color[2] = (color[0] * 0.272f) + (color[1] * 0.534f) + (color[2] * 0.131f);
	}

	if (postProcOper[1] == 1)
	{
		color[0] = (color[0] * contrastAlpha);
		color[1] = (color[1] * contrastAlpha);
		color[2] = (color[2] * contrastAlpha);
	}
    int RADIUS = 3;
    int VIEWSIZE = 200;
    if (postProcOper[2] == 1)
    {
        float3 mean[4] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} };
        float3 sigma[4] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} };

        float2 offsets[4] = { {-RADIUS, -RADIUS}, {-RADIUS, 0}, {0, -RADIUS}, {0, 0} };

        float2 pos;
        float3 col;

        for (int i = 0; i < 4; i++)
        {

            for (int j = 0; j <= RADIUS; j++)
            {

                for (int k = 0; k <= RADIUS; k++)
                {

                    pos = float2(j, k) + offsets[i];
                    float2 uvpos = input.uv + pos / VIEWSIZE;

                    col = colorTexture.Sample(colorSampler, uvpos).xyz;

                    mean[i] += col;
                    sigma[i] += col * col;

                }

            }

        }

        float n = pow(RADIUS + 1, 2);
        float sigma_f;
        float min = 1;

        for (int i = 0; i < 4; i++)
        {

            mean[i] /= n;
            sigma[i] = abs(sigma[i] / n - mean[i] * mean[i]);
            sigma_f = sigma[i].r + sigma[i].g + sigma[i].b;

            if (sigma_f < min)
            {
                min = sigma_f;
                col = mean[i];
            }
        }

        color = col;
    }

	return float4(color, 1.0f);
}