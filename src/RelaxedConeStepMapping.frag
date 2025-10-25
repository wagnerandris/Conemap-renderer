#version 330 core

uniform float depth;
uniform int steps;
uniform int display_mode;
uniform bool show_convergence;

in vec2 texCoord;
in vec3 eyeSpaceVert;
in vec3 eyeSpaceTangent;
in vec3 eyeSpaceBinormal;
in vec3 eyeSpaceNormal;

uniform sampler2D stepmap; // (height, cone half-angle tangent, df/dx, df/dy)
uniform sampler2D texmap;

void main(void) {
	ivec2 texsize = textureSize(stepmap, 0);

	// viewing ray direction in texture space
	vec3 eyeSpaceDir = normalize(eyeSpaceVert);
	vec3 dir = normalize(vec3(
				dot(eyeSpaceDir, eyeSpaceTangent),
				dot(eyeSpaceDir, eyeSpaceBinormal),
				dot(eyeSpaceDir, -eyeSpaceNormal) / depth // larger depth <-> smaller steps downward
				));

	// horizontal length of dir
	float hl = sqrt(1.0f - dir.z * dir.z);

	float ss; // step size

	// minimum feature size parameter
	float mfs = 1.0f / max(texsize.x, texsize.y);

	float dist = 0.0f;

	// texture at starting coordinates
	vec4 t = texture(stepmap, texCoord);


// Cone stepping
	while (1.0f - dir.z * dist > t.r) // while above the surface
	{
		// set step size (see documentation)
		ss = (1.0f - dir.z * dist - t.r) / (dir.z + hl / (t.g * t.g))
					+ mfs; // at least mfs

		dist += ss; // increase distance

		// find the new location and height
		t = texture(stepmap, texCoord + dir.xy * dist);
	}

	bool converged = 1.0f - dir.z * (dist - mfs) > t.r; // for if steps == 0


// Binary search (with mfs accuracy)
	for (int i = steps; i > 0; --i) {
		// if not within mfs, take half the previous step size in the right direction
		if (1.0f - dir.z * (dist - mfs) < t.r) {
			ss *= 0.5f;
			dist -= ss;
			t = texture(stepmap, texCoord + dir.xy * dist);
		} else
		if (1.0f - dir.z * (dist + mfs) > t.r) {
			ss *= 0.5f;
			dist += ss;
			t = texture(stepmap, texCoord + dir.xy * dist);
		} else {
			// return the vector length needed to hit the height-map
			converged = true;
			break;
		}
	}

	vec2 uv = texCoord + dir.xy * dist;


// Output color
	if (show_convergence && !converged) {
		gl_FragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
		return;
	}

	switch (display_mode) {
		case 0: // Color texture
			gl_FragColor = texture(texmap, uv);
			break;
		case 1: // Heights
			t = texture(stepmap, uv);
			gl_FragColor = vec4(vec3(t.r), 1.0f);
			break;
		case 2: // Cones
			t = texture(stepmap, uv);
			gl_FragColor = vec4(vec3(t.g), 1.0f);
			break;
		case 3: // Normals
			// blue = df/dx
			// alpha = df/dy
			t = texture(stepmap, uv);

			// scale normals to reflect displayed geometry
			t.xy = t.ba - 0.5f;
			t.x = -t.x * depth * texsize.x;
			t.y = -t.y * depth * texsize.y;
			t.z = 1.0f;

			gl_FragColor = vec4(normalize(t.xyz), 1.0f);
			break;
	}
}
