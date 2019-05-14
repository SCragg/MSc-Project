// Copyright (c) 2010-2018 University of Dundee

// This is the minimum required version for the camera model.
#version 130

// This is f_diffuse_hapke_1981 specialised for h=0. It removes the bright
// Sun-like reflection at opposition while maintaining the general back-scatter
// effect at opposition.

// Various constants associated with pi.
const float M_PI        = 3.14159265358979323846;   // pi
const float M_PI_BY_10  = 0.314159265358979323846;  // pi/10

// Parameters to the model. As an optimisation we accept w2 and w3 in
// place of the Hapke w average particle single scattering albedo.
uniform float hapke_w2;	// sqrt(1.0 - w)
uniform float hapke_w3;	// w/4 (must not include 1/pi factor)
uniform float hapke_s;	// Scale factor for brightness compensation
uniform float hapke_L;  // Hapke factor L (Lambert)

//===========================================================================

// Hapke single scattering particle phase function P.
float P_hapke(in float sing, in float cosg, in float g)
{
	float pterm = 1.0-cosg;
	return 0.8*((sing + (M_PI - g)*cosg) + M_PI_BY_10*pterm*pterm);
}


// Hapke function H.
float H_hapke(in float u, in float w2)
	{ return (2.0*u + 1.0)/(2.0*u*w2 + 1.0); }

//===========================================================================

// Implements the Hapke-1981 material BRDF for h=0.
//
// PANGU BRDF conventions:
// - the 1/pi normalisation factor must NEVER be included.
// - the cosi Lambertian term MUST be included.
vec3
apply_diffuse
(
	in    vec3 P,	// Fragment position
	in    vec3 N,	// Surface normal
	in    vec3 V,	// Direction of viewer/eye
	in    vec3 L,	// Light direction
	in    vec3 H 	// Halfway vector
)
{
	// Compute the cosine of the angle of incidence, the cosine of the
	// angle of emission and the cosine of the phase angle.
	float cosi = dot(N, L);
	float cose = dot(N, V);
	float cosg = dot(L, V);

	// Extract the phase angle taking care to avoid possible numerical
	// errors in the L.V product above and compute related values.
	float g    = acos(clamp(cosg, -1.0, 1.0));
	float absg = abs(g);
	float sing = sin(g);

	// Compute the reflectance.
	float r = 0.0;
	if (cosi > 0.0 && cose > 0.0)
	{
		// Hapke terms. Experiments show that B(g) tends to 0 as h
		// tends to 0. Which means we don't need to know B0.
		float lommel   = cosi/(cosi + cose);
		float surge    = 1.0;
		float single   = P_hapke(sing, cosg, g);
		float multiple = H_hapke(cosi,hapke_w2)*H_hapke(cose,hapke_w2);

		// Combine for the reflectance.
		r = hapke_s*hapke_w3*lommel*(surge*single + multiple - 1.0);
	}

	// Return the resulting Hapke contribution linearly combined with
	// the Lambert contribution.
	float lambert = max(0.0,cosi);
	return vec3(mix(r,lambert,hapke_L));
}
