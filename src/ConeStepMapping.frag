#version 330 core

uniform vec4 ambient;
uniform vec4 diffuse;
uniform float depth;
uniform float texsize;
uniform int conesteps;

in vec2 texCoord;
in vec3 eyeSpaceVert;
in vec3 eyeSpaceTangent;
in vec3 eyeSpaceBinormal;
in vec3 eyeSpaceNormal;
in vec3 eyeSpaceLight;

uniform sampler2D stepmap;
uniform sampler2D texmap;

float intersect_cone_fixed(in vec2 dp,in vec3 ds);
float intersect_cone_loop(in vec2 dp,in vec3 ds);
float intersect_cone_exact(in vec2 dp,in vec3 ds);

void main(void)
{

	vec4 t,c;
	vec3 p,v,l,s;
	vec2 uv;
	float d,a;

	// ray intersect in view direction
	p = eyeSpaceVert;
	v = normalize(p);
	a = dot(eyeSpaceNormal,-v)/depth;
	s = normalize(vec3(dot(v,eyeSpaceTangent),dot(v,eyeSpaceBinormal),a));

	// pick _one_ of the following variations
	//d = intersect_cone_fixed(texCoord,s);
	//d = intersect_cone_loop(texCoord,s);
	d = intersect_cone_exact(texCoord,s);

	// get rm and color texture points
	uv=texCoord+s.xy*d;
	c=texture(texmap,uv);

	// expand normal from normal map in local polygon space
	// blue = df/dx
	// alpha = df/dy
	// note: I _need_ the texture size to scale the normal properly!
	t=texture(stepmap,uv);
	t.xy=t.ba-0.5;
	t.x = -t.x * depth * texsize;
	t.y = -t.y * depth * texsize;
	t.z = 1.0;
	t.w = 0.0;
	t.xyz=normalize(t.x*eyeSpaceTangent+t.y*eyeSpaceBinormal+t.z*eyeSpaceNormal);

	// compute light direction
	p += v*d*a;
	l=normalize(p-eyeSpaceLight.xyz);

	gl_FragColor = vec4(
											ambient.xyz*c.xyz+
											c.xyz*diffuse.xyz*max(0.0,dot(-l,t.xyz)),
											1.0);
}

// slowest, but best quality
float intersect_cone_exact(in vec2 dp, in vec3 ds)
{
	// minimum feature size parameter
	float w = 1.0 / texsize;
	// the "not Z" component of the direction vector
	// (requires that the vector ds was normalized!)
	float iz = sqrt(1.0-ds.z*ds.z);
	// my starting location (is at z=1,
	// and moving down so I don't have
	// to invert height maps)
	// texture lookup
	vec4 t;
	// scaling distance along vector ds
	float sc=0.0;

	// the ds.z component is positive!
	// (headed the wrong way, since
	// I'm using heightmaps)
	// find the starting location and height
	t=texture(stepmap,dp);
	while (1.0-ds.z*sc > t.r)
	{
		// right, I need to take one step.
		// I use the current height above the texture,
		// and the information about the cone-ratio
		// to size a single step. So it is fast and
		// precise! (like a coneified version of
		// "space leaping", but adapted from voxels)
		sc += w + (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));
		// find the new location and height
		t=texture(stepmap,dp+ds.xy*sc);
	}
	// back off one step
	sc -= w;

	// return the vector length needed to hit the height-map
	return (sc);
}

// the super fast version
// (change number of iterations at run time)
float intersect_cone_fixed(in vec2 dp, in vec3 ds)
{
	// the "not Z" component of the direction vector
	// (requires that the vector ds was normalized!)
	float iz = sqrt(1.0-ds.z*ds.z);
	// my starting location (is at z=1,
	// and moving down so I don't have
	// to invert height maps)
	// texture lookup (and initialized to starting location)
	vec4 t;
	// scaling distance along vector ds
	float sc;

	// the ds.z component is positive!
	// (headed the wrong way, since
	// I'm using heightmaps)

	// find the initial location and height
	t=texture(stepmap,dp);
	// right, I need to take one step.
	// I use the current height above the texture,
	// and the information about the cone-ratio
	// to size a single step. So it is fast and
	// precise! (like a coneified version of
	// "space leaping", but adapted from voxels)
	sc = (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));

	// and repeat a few (4x) times
	t=texture(stepmap,dp+ds.xy*sc);
	sc += (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));
	t=texture(stepmap,dp+ds.xy*sc);
	sc += (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));
	t=texture(stepmap,dp+ds.xy*sc);
	sc += (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));
	t=texture(stepmap,dp+ds.xy*sc);
	sc += (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));

	// and another five!
	t=texture(stepmap,dp+ds.xy*sc);
	sc += (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));
	t=texture(stepmap,dp+ds.xy*sc);
	sc += (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));
	t=texture(stepmap,dp+ds.xy*sc);
	sc += (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));
	t=texture(stepmap,dp+ds.xy*sc);
	sc += (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));
	t=texture(stepmap,dp+ds.xy*sc);
	sc += (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));

	// return the vector length needed to hit the height-map
	return (sc);
}

// pretty fast version
// (and you can do LOD by changing "conesteps" based on size/distance, etc.)
float intersect_cone_loop(in vec2 dp, in vec3 ds)
{
	// the "not Z" component of the direction vector
	// (requires that the vector ds was normalized!)
	float iz = sqrt(1.0-ds.z*ds.z);
	// my starting location (is at z=1,
	// and moving down so I don't have
	// to invert height maps)
	// texture lookup (and initialized to starting location)
	vec4 t;
	// scaling distance along vector ds
	float sc=0.0;
	
	//t=texture(stepmap,dp);
	//return (max(0.0,-(t.b-0.5)*ds.x-(t.a-0.5)*ds.y));
	// the ds.z component is positive!
	// (headed the wrong way, since
	// I'm using heightmaps)

	// adaptive (same speed as it averages the same # steps)
	//for (int i = int(float(conesteps)*(0.5+iz)); i > 0; --i)
	// fixed
	for (int i = conesteps; i > 0; --i)
	{
		// find the new location and height
		t=texture(stepmap,dp+ds.xy*sc);
		// right, I need to take one step.
		// I use the current height above the texture,
		// and the information about the cone-ratio
		// to size a single step. So it is fast and
		// precise! (like a coneified version of
		// "space leaping", but adapted from voxels)
		sc += (1.0-ds.z*sc-t.r) / (ds.z + iz/(t.g*t.g));
	}
	
	// return the vector length needed to hit the height-map
	return (sc);
}
