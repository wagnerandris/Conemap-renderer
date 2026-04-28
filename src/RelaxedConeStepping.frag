#version 330 core

uniform float depth;
uniform int cone_steps;
uniform int binary_steps;
uniform int display_mode;
uniform bool cell_max_trace;
uniform bool show_convergence;

in vec2 texCoord;
in vec3 eyeSpaceVert;
in vec3 eyeSpaceTangent;
in vec3 eyeSpaceBitangent;
in vec3 eyeSpaceNormal;

uniform sampler2D stepmap; // (height, cone half-angle tangent, df/dx, df/dy)
uniform sampler2D texmap;

void main(void) {
	vec3 dir = // viewing ray direction in texture space
		normalize(eyeSpaceVert * // viewing ray
							mat3(eyeSpaceTangent * depth,
					 	 	 	 	 eyeSpaceBitangent * depth,
					 	 	 	 	-eyeSpaceNormal)); // dir.z inverted to be a positive length
	// greater depth <-> direction less downward

	// horizontal length of dir
	float l = sqrt(abs(1.0f - dir.z * dir.z));
	// abs needed to avoid negatives from float inprecision

	// minimum feature size
	ivec2 texsize = textureSize(stepmap, 0);
	float mfs = 1.0f / max(texsize.x, texsize.y);
	
	// texture at starting coordinates
	vec4 t = texture(stepmap, texCoord);

// Cone stepping
	float dist = 0.0f;
	float s; // step length
	int step_count = 0;

	if (cell_max_trace) {
		// conservative step to cell border adapted from Robust Cone Step Mapping by Bán et al.
		vec2 itexsize = 1.0f / texsize;
		vec2 idir = 1.0f / dir.xy;
		vec2 dirSign = vec2(dir.x < 0 ? -1 : 1, dir.y < 0 ? -1 : 1) * 0.5 * itexsize;
		while (1.0f - dir.z * dist > t.r && step_count < cone_steps) // while above the surface
		{
			// set step size (see documentation)
			float tng = t.g * t.g;
			s = (1.0f - dir.z * dist - t.r) * tng / (l + dir.z * tng);
			vec2 p = texCoord + dir.xy * (dist + s);
      vec2 cellCenter = (floor(p*texsize - 0.5f) + 1) * itexsize;
      vec2 wall = cellCenter + dirSign;
      vec2 stepToCellBorder = (wall - p) * idir;
      float	w = min(stepToCellBorder.x, stepToCellBorder.y) + 1e-5;
      s += w;
			dist += s; // increase distance

			// find the new location and height
			t = texture(stepmap, texCoord + dir.xy * dist);
			step_count++;
		}
	} else {
		while (1.0f - dir.z * dist > t.r && step_count < cone_steps) // while above the surface
		{
			// set step size (see documentation)
			float tng = t.g * t.g;
			s = (1.0f - dir.z * dist - t.r) * tng / (l + dir.z * tng)
				+ mfs; // correct by minimum feature size
			dist += s; // increase distance

			// find the new location and height
			t = texture(stepmap, texCoord + dir.xy * dist);
			step_count++;
		}
	}

// Binary search (with mfs accuracy)
	for (int i = 0; i < binary_steps; ++i) {
		// if not within mfs, take half the previous step size in the right direction
		if (1.0f - dir.z * (dist - mfs) < t.r) {
			s *= 0.5f;
			dist -= s;
		} else
		if (1.0f - dir.z * (dist + mfs) > t.r) {
			s *= 0.5f;
			dist += s;
		} else {
			// we are within mfs
			break;
		}
		t = texture(stepmap, texCoord + dir.xy * dist);
	}

	// return the vector length needed to hit the height-map
	vec2 uv = texCoord + dir.xy * dist;

// Output color
	if (show_convergence && !(1.0f - dir.z * (dist - mfs) > t.r && 1.0f - dir.z * (dist + mfs) < t.r)) {
		gl_FragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
		return;
	}

	switch (display_mode) {
		case 0: // Color texture
			gl_FragColor = texture(texmap, uv);
			break;
		case 1: // Heights
			gl_FragColor = vec4(vec3(t.r), 1.0f);
			break;
		case 2: // Cones
			gl_FragColor = vec4(vec3(t.g * t.g), 1.0f);
			break;
		case 3: // Normals
			// blue = df/dx
			// alpha = df/dy

			// scale normals to reflect displayed geometry
			t.xy = t.ba * 2.0f - vec2(1.0f);
			t.x = -t.x * depth * texsize.x;
			t.y = -t.y * depth * texsize.y;
			t.z = 1.0f;
			t.xyz = normalize(t.xyz);
			t.xy = t.xy / 2.0f + vec2(0.5f);

			gl_FragColor = vec4(t.xyz, 1.0f);
			break;
	}
}
