// Copyright (c) 2010-2018 University of Dundee

// This is the minimum required version for the camera model.
#version 130

// Fragment shader implementing the Hapke BRDF from the 1981 paper
// 'Bidirectional Reflectance Spectroscopy I and II' by Hapke.
//
// Original version by Mark McCrum.
// Bug fix in H(u) denominator and div-by-zero avoidance by Martin Dunstan.
// Adapted to implement the apply_material() API by Martin Dunstan.
// Further Inf/NaN avoidance by Martin Dunstan.
// Replaced uniform w with uniforms hapke_w2 and hapke_w3.

// Various constants associated with pi.
const float M_PI        = 3.14159265358979323846;   // pi
const float M_PI_BY_2   = 1.57079632679489661923;   // pi/2
const float M_PI_BY_10  = 0.314159265358979323846;  // pi/10

// Parameters to the model. As an optimisation we accept w2 and w3 in
// place of the Hapke w average particle single scattering albedo.
uniform float hapke_w2;	// sqrt(1.0 - w)
uniform float hapke_w3;	// w/4 (must not include 1/pi factor)
uniform float hapke_B0;	// Hapke parameter B_0
uniform float hapke_h;	// Hapke parameter h
uniform float hapke_s;	// Scale factor for brightness compensation
uniform float hapke_L;  // Hapke parameter L (Lambert)

//===========================================================================

// Hapke backscatter function B.
float B_hapke(in float absg, in float h, in float B0)
{
	float result = 0.0;
	if (absg < M_PI_BY_2)
	{
		float tanabsg  = tan(absg);
		float etanabsg = exp(-h/tanabsg);
		float term1 = 0.5*tanabsg/h;
		result = B0*(1.0 - term1*(3.0 - etanabsg)*(1.0 - etanabsg));
	}
	return result;
}


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

// Implements the Hapke-1981 material BRDF.
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
		// Hapke terms.
		float lommel   = cosi/(cosi + cose);
		float surge    = B_hapke(absg, hapke_h, hapke_B0) + 1.0;
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
