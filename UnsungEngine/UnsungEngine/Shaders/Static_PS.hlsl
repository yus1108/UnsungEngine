struct INPUT_PIXEL
{
	float4 coordinate		: SV_POSITION;
	float4 color			: COLOR;
	float2 tex				: TEXCOORD0;
	float3 normal			: NORMAL;
	float3 worldPosition	: POSITION;
	float3 tangent			: TANGENT;
	float3 binormal			: BINORMAL;
};

cbuffer DLIGHT : register(b2)
{
	float4 dLightColor;
	float3 lightDirection;
	float dAmbient;
};
cbuffer PLIGHT : register(b3)
{
	float4 pLightColor;
	float3 pLightPos;
	float lightRadius;
};
cbuffer SLIGHT : register(b4)
{
	float4 sLightColor;
	float3 sLightPos;
	float coneRatioIn;
	float3 sConeDir;
	float coneRatioOut;
};
cbuffer LIGHTINFO : register(b5)
{
	float4 specConstant;
	float4 diffuse;
	float4 ambient;
	float4 specular;
	float4 emissive;
};
texture2D baseTexture : register(t0); // first texture
texture2D specTexture : register(t1); // specular texture
texture2D emissiveTexture : register(t2); // emissive texture
texture2D normalTexture : register(t3); // normal texture

SamplerState filters : register(s0); // filter 0 using CLAMP, filter 1 using WRAP
float4 main(INPUT_PIXEL input) : SV_TARGET
{
	// calculate normal mapping
	float4 bumpMap = normalTexture.Sample(filters, input.tex);
	bumpMap = (bumpMap * 2.0f) - 1.0f;
	float3 bumpNormal = -bumpMap.x * input.tangent + -bumpMap.y * input.binormal + input.normal;
	input.normal = normalize(bumpNormal);

	float dRatio = saturate(dot(-lightDirection, bumpNormal));
	dRatio = max(dRatio, dAmbient);

	float3 pLightDir = normalize(pLightPos - input.worldPosition);
	float pRatio = saturate(dot(pLightDir, input.normal));
	float attenuation = 1 - saturate(length(pLightPos - input.worldPosition) / lightRadius);
	attenuation *= attenuation;
	pRatio = saturate(pRatio * attenuation);

	float3 sLightDir = normalize(sLightPos - input.worldPosition);
	float surfaceRatio = saturate(dot(-sLightDir, normalize(sConeDir)));
	float spotFactor = (surfaceRatio > coneRatioOut) ? 1 : 0;
	float sRatio = saturate(dot(sLightDir, input.normal));
	float sAtt = 1 - saturate((coneRatioIn - surfaceRatio) / (coneRatioIn - coneRatioOut));
	sAtt *= sAtt;
	sRatio = saturate(sRatio * sAtt);

	float4 lightColor;
	lightColor.xyz = saturate(dRatio * dLightColor.xyz
		+ pRatio * pLightColor.xyz
		+ spotFactor * sRatio * sLightColor.xyz)
		* input.color.xyz;
	lightColor.w = input.color.w;
	lightColor *= float4(diffuse.xyz * diffuse.w, 1);

	float4 diffuseColor = baseTexture.Sample(filters, input.tex) * lightColor; // get base color

	float4 finalSpecColor = specTexture.Sample(filters, input.tex);
	if (all(finalSpecColor))
	{
		float3 reflectionDirection = reflect(lightDirection, input.normal);
		float3 directionToCamera = normalize(specConstant.xyz - input.worldPosition);
		float specularPower = specConstant.w;
		float specScale = specular.w *
			pow(saturate(dot(reflectionDirection, directionToCamera)), specularPower);
		float4 dSpecLight = float4(specScale * specular.xyz, 1);

		reflectionDirection = reflect(-sLightDir, input.normal);
		specScale = specular.w *
			pow(saturate(dot(reflectionDirection, directionToCamera)), specularPower);
		float4 sSpecLight = float4(saturate(specScale * sAtt) * specular.xyz * sLightColor.xyz, 1);

		reflectionDirection = reflect(-pLightDir, input.normal);
		specScale = specular.w *
			pow(saturate(dot(reflectionDirection, directionToCamera)), specularPower);
		float4 pSpecLight = float4(saturate(specScale * attenuation) * specular.xyz * pLightColor.xyz, 1);
		finalSpecColor *= saturate(dSpecLight + sSpecLight + pSpecLight);
	}

	float4 emissiveColor = emissiveTexture.Sample(filters, input.tex);
	if (all(emissiveColor))
		emissiveColor *= float4(emissive.xyz * emissive.w, 1);

	return saturate(diffuseColor + finalSpecColor + emissiveColor); // return a transition based on the detail alpha
}