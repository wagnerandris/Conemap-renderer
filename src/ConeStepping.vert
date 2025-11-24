#version 330 core

// vertex attributes
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec3 inBitangent;
layout(location = 4) in vec2 inTexCoord;

// outputs to fragment shader
out vec2 texCoord;
out vec3 eyeSpaceVert;
out vec3 eyeSpaceTangent;
out vec3 eyeSpaceBitangent;
out vec3 eyeSpaceNormal;

// uniforms
uniform mat4 worldViewMatrix;
uniform mat4 projectionMatrix;

void main()
{
		// pass through texture coordinates
		texCoord = inTexCoord;

		// transform vertex position to eye space
		vec4 viewPos	= worldViewMatrix * vec4(inPosition, 1.0);
		eyeSpaceVert = viewPos.xyz;

		// compute world-space to eye-space transform for normals/tangents
		mat3 normalMatrix = mat3(worldViewMatrix);

		// transform normal, tangent, and binormal to eye space
		eyeSpaceNormal	 = normalize(normalMatrix * inNormal);
		eyeSpaceTangent	 = normalize(normalMatrix * inTangent);
		eyeSpaceBitangent = normalize(normalMatrix * inBitangent);

		// final clip-space position
		gl_Position = projectionMatrix * viewPos;
}
