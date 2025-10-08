#version 330 core

uniform float depth;
uniform int steps;
uniform int display_mode;

in vec2 texCoord;
in vec3 eyeSpaceVert;
in vec3 eyeSpaceTangent;
in vec3 eyeSpaceBinormal;
in vec3 eyeSpaceNormal;

uniform sampler2D stepmap;
uniform sampler2D texmap;

float intersect_relaxed_cone_exact(vec2 dp, vec3 ds, ivec2 texsize)
{	
	// minimum feature size parameter
	float w = 1.0 / max(texsize.x, texsize.y);

	// the "not Z" component of the direction vector
	// (requires that the vector ds was normalized!)
	float iz = sqrt(1.0 - ds.z*ds.z); // horizontal length of normalized view vector (ds)
	// my starting location (is at z=1,
	// and moving down so I don't have
	// to invert height maps)

	// texture lookup
	vec4 t;

	// scaling distance along vector ds
	float sc=0.0;

	// step size
	float ss;

	// the ds.z component is positive!
	// (headed the wrong way, since
	// I'm using heightmaps)
	// find the starting location and height
	t=texture(stepmap,dp);
	while (1.0 - ds.z*sc > t.r)
	{
		// right, I need to take one step.
		// I use the current height above the texture,
		// and the information about the cone-ratio
		// to size a single step. So it is fast and
		// precise! (like a coneified version of
		// "space leaping", but adapted from voxels)
		ss = (1.0 - ds.z*sc - t.r) / (ds.z + iz / (t.g*t.g));
		sc += w + ss;
		// find the new location and height
		t=texture(stepmap,dp+ds.xy*sc);
	}
	// back off one step
	sc -= w;

	// binary search
	for (int i = steps; i > 0; --i) {
		if (1.0 - ds.z * (sc - w) < t.r) {
			ss *= 0.5f;
			sc -= ss;
			t=texture(stepmap, dp + ds.xy * sc);
		} else
		if (1.0 - ds.z * (sc + w) > t.r) {
			ss *= 0.5f;
			sc += ss;
			t=texture(stepmap, dp + ds.xy * sc);
		} else {
			// return the vector length needed to hit the height-map
			break;
		}
	}
	return sc;
}

void main(void) {
	vec4 t,c;
	vec3 p,v,l,s;
	vec2 uv;
	float d,a;

	ivec2 texsize = textureSize(stepmap, 0);

	// ray intersect in view direction
	p = eyeSpaceVert;
	v = normalize(p);
	a = dot(eyeSpaceNormal,-v)/depth; // TODO depth factor as setting
	s = normalize(vec3(dot(v,eyeSpaceTangent),dot(v,eyeSpaceBinormal),a));
	
	d = intersect_relaxed_cone_exact(texCoord, s, texsize);
	uv=texCoord+s.xy*d;

	switch (display_mode) {
		case 0: // Color texture
			gl_FragColor = texture(texmap,uv);
			break;
		case 1: // Heights
			t=texture(stepmap,uv);
			gl_FragColor = vec4(vec3(t.x), 1.0);
			break;
		case 2: // Cones
			t=texture(stepmap,uv);
			gl_FragColor = vec4(vec3(t.y), 1.0);
			break;
		case 3: // Normals
			// expand normal from normal map in local polygon space
			// blue = df/dx
			// alpha = df/dy
			// note: I _need_ the texture size to scale the normal properly!
			t=texture(stepmap,uv);
			t.xy=t.ba-0.5;
			t.x = -t.x * depth * texsize.x;
			t.y = -t.y * depth * texsize.y;
			t.z = 1.0;
			t.w = 1.0;
			t.xyz = normalize(t.xyz);

			gl_FragColor = t;
			break;
	}
}


// TODO remove all
// // slowest, but best quality
// float intersect_cone_exact(in vec2 dp, in vec3 ds)
// {
// 	// minimum feature size parameter
// 	float w = 1.0 / texsize;
// 	// the "not Z" component of the direction vector
// 	// (requires that the vector ds was normalized!)
// 	float iz = sqrt(1.0-ds.z*ds.z);
// 	// my starting location (is at z=1,
// 	// and moving down so I don't have
// 	// to invert height maps)
// 	// texture lookup
// 	vec4 t;
// 	// scaling distance along vector ds
// 	float sc=0.0;
//
// 	// the ds.z component is positive!
// 	// (headed the wrong way, since
// 	// I'm using heightmaps)
// 	// find the starting location and height
// 	t=texture(stepmap,dp);
// 	while (1.0-ds.z*sc > t.r)
// 	{
// 		// right, I need to take one step.
// 		// I use the current height above the texture,
// 		// and the information about the cone-ratio
// 		// to size a single step. So it is fast and
// 		// precise! (like a coneified version of
// 		// "space leaping", but adapted from voxels)
// 		sc += w + (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));
// 		// find the new location and height
// 		t=texture(stepmap,dp+ds.xy*sc);
// 	}
// 	// back off one step
// 	sc -= w;
//
// 	// return the vector length needed to hit the height-map
// 	return (sc);
// }
//
// // the super fast version
// // (change number of iterations at run time)
// float intersect_cone_fixed(in vec2 dp, in vec3 ds)
// {
// 	// the "not Z" component of the direction vector
// 	// (requires that the vector ds was normalized!)
// 	float iz = sqrt(1.0-ds.z*ds.z);
// 	// my starting location (is at z=1,
// 	// and moving down so I don't have
// 	// to invert height maps)
// 	// texture lookup (and initialized to starting location)
// 	vec4 t;
// 	// scaling distance along vector ds
// 	float sc;
//
// 	// the ds.z component is positive!
// 	// (headed the wrong way, since
// 	// I'm using heightmaps)
//
// 	// find the initial location and height
// 	t=texture(stepmap,dp);
// 	// right, I need to take one step.
// 	// I use the current height above the texture,
// 	// and the information about the cone-ratio
// 	// to size a single step. So it is fast and
// 	// precise! (like a coneified version of
// 	// "space leaping", but adapted from voxels)
// 	sc = (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));
//
// 	// and repeat a few (4x) times
// 	t=texture(stepmap,dp+ds.xy*sc);
// 	sc += (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));
// 	t=texture(stepmap,dp+ds.xy*sc);
// 	sc += (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));
// 	t=texture(stepmap,dp+ds.xy*sc);
// 	sc += (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));
// 	t=texture(stepmap,dp+ds.xy*sc);
// 	sc += (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));
//
// 	// and another five!
// 	t=texture(stepmap,dp+ds.xy*sc);
// 	sc += (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));
// 	t=texture(stepmap,dp+ds.xy*sc);
// 	sc += (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));
// 	t=texture(stepmap,dp+ds.xy*sc);
// 	sc += (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));
// 	t=texture(stepmap,dp+ds.xy*sc);
// 	sc += (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));
// 	t=texture(stepmap,dp+ds.xy*sc);
// 	sc += (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));
//
// 	// return the vector length needed to hit the height-map
// 	return (sc);
// }
//
// // pretty fast version
// // (and you can do LOD by changing "steps" based on size/distance, etc.)
// float intersect_cone_loop(in vec2 dp, in vec3 ds)
// {
// 	// the "not Z" component of the direction vector
// 	// (requires that the vector ds was normalized!)
// 	float iz = sqrt(1.0-ds.z*ds.z);
// 	// my starting location (is at z=1,
// 	// and moving down so I don't have
// 	// to invert height maps)
// 	// texture lookup (and initialized to starting location)
// 	vec4 t;
// 	// scaling distance along vector ds
// 	float sc=0.0;
//
// 	//t=texture(stepmap,dp);
// 	//return (max(0.0,-(t.b-0.5)*ds.x-(t.a-0.5)*ds.y));
// 	// the ds.z component is positive!
// 	// (headed the wrong way, since
// 	// I'm using heightmaps)
//
// 	// adaptive (same speed as it averages the same # steps)
// 	//for (int i = int(float(steps)*(0.5+iz)); i > 0; --i)
// 	// fixed
// 	for (int i = steps; i > 0; --i)
// 	{
// 		// find the new location and height
// 		t=texture(stepmap,dp+ds.xy*sc);
// 		// right, I need to take one step.
// 		// I use the current height above the texture,
// 		// and the information about the cone-ratio
// 		// to size a single step. So it is fast and
// 		// precise! (like a coneified version of
// 		// "space leaping", but adapted from voxels)
// 		sc += (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));
// 	}
//
// 	// return the vector length needed to hit the height-map
// 	return (sc);
// }
