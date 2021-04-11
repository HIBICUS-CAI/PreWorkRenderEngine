// 光照辅助

// 最多16个光源
#define MaxLights (16)

// 相关含义查看对应的cpp声明
struct Light
{
    float3 Strength;
    float FalloffStart;
    float3 Direction;
    float FalloffEnd;
    float3 Position;
    float SpotPower;
};

struct Material
{
    float4 DiffuseAlbedo;   // 漫反射反射率
    float3 FresnelR0;       // 折射率
    float Shininess;        // 表面光滑度（用于镜面反射计算）
};

// 线性衰减因子计算
float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

// 用于逼近菲涅尔方程的石里克近似计算
float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
    float cosIncidentAngle = saturate(dot(normal, lightVec));
    float f0 = 1.0f - cosIncidentAngle;
    float3 reflectPercent = R0 + (1.0f - R0) * (f0 * f0 * f0 * f0 * f0);

    return reflectPercent;
}

// 镜面光照与漫反射光照总和计算
float3 BlinnPhong(float3 lightStr, float3 lightVec, float3 normal, float3 toEye, Material mat)
{
    const float m = mat.Shininess * 256.0f;
    float3 halfVec = normalize(toEye + lightVec);
    float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
    float3 fresnelFactor = SchlickFresnel(mat.FresnelR0, halfVec, lightVec);
    float3 specAlbedo = fresnelFactor * roughnessFactor;
    specAlbedo = specAlbedo / (specAlbedo + 1.0f);

    return (mat.DiffuseAlbedo.rgb + specAlbedo) * lightStr;
}

// 平行光源计算
float3 ComputeDirectionalLight(Light l, Material mat, float3 normal, float3 toEye)
{
    float3 lightVec = -l.Direction;
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStr = l.Strength * ndotl;

    return BlinnPhong(lightStr, lightVec, normal, toEye, mat);
}

// 点光源计算
float3 ComputePointLight(Light l, Material mat, float3 pos, float3 normal, float3 toEye)
{
    float3 lightVec = l.Position - pos;
    float d = length(lightVec);
    
    if (d > l.FalloffEnd)
    {
        return 0.0f;
    }

    lightVec /= d;
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStr = l.Strength * ndotl;
    float att = CalcAttenuation(d, l.FalloffStart, l.FalloffEnd);
    lightStr *= att;

    return BlinnPhong(lightStr, lightVec, normal, toEye, mat);
}

// 聚光灯光源计算
float3 ComputeSpotLight(Light l, Material mat, float3 pos, float3 normal, float3 toEye)
{
    float3 lightVec = l.Position - pos;
    float d = length(lightVec);

    if (d > l.FalloffEnd)
    {
        return 0.0f;
    }

    lightVec /= d;
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStr = l.Strength * ndotl;
    float att = CalcAttenuation(d, l.FalloffStart, l.FalloffEnd);
    lightStr *= att;
    float spotFactor = pow(max(dot(-lightVec, l.Direction), 0.0f), l.SpotPower);
    lightStr *= spotFactor;

    return BlinnPhong(lightStr, lightVec, normal, toEye, mat);
}

// 光源叠加结果计算
float4 ComputeLighting(Light gLights[MaxLights], Material mat, float3 pos, float3 normal, float3 toEye, float3 shadowFactor)
{
    float3 result = 0.0f;
    int i = 0;

#if (NUM_DIR_LIGHTS > 0)
    for (i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        result += shadowFactor[i] * ComputeDirectionalLight(gLights[i], mat, normal, toEye);
    }
#endif

#if (NUM_POINT_LIGHTS > 0)
    for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; ++i)
    {
        result += ComputePointLight(gLights[i], mat, pos, normal, toEye);
    }
#endif

#if (NUM_SPOT_LIGHTS > 0)
    for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    {
        result += ComputeSpotLight(gLights[i], mat, pos, normal, toEye);
    }
#endif 

    return float4(result, 0.0f);
}
