#version 420
#define PI 3.141592653589793
#define RADIANS30 0.523599
#define RADIANS60 1.0472

//inputs
in vec3 fNormal;
in vec3 fLightDir;
in float flocal_time;
in float latitude;

//Uniforms
uniform float global_time;
uniform sampler1D thermaltexture;

out vec4 outputColor;

//Function declarations
vec4 Colour_Greyscale(float temp, float min_val, float max_val);
vec4 Colour_False(float temp, float min_val, float max_val);
float Latitude_TempCorr(float lat, vec3 temps);

//normalize input vectors
vec3 normal = normalize(fNormal);
vec3 L = normalize(fLightDir);

void main()
{
	
	//Calculations for local time
	float local_slope;
	local_slope = tan(acos(dot(normalize(normal), vec3(0,1,0))));

	//remove height and normalise
	vec2 heading_vec = vec2(normalize(normal).x, normalize(normal).z);
	normalize(heading_vec);

	//calcuate slope azimuth angle
	float azimuth = atan(heading_vec.x, heading_vec.y);
	if (azimuth < 0)
	{
		azimuth = 6.28318530718 + azimuth; // convert to range 0 - 2pi
	}

	//corrction equation
	float local_time = global_time + (0.5 / PI) * atan(local_slope * sin(azimuth));

	vec3 forward_vec = vec3(0, 1, 0);
	vec3 right_vec = vec3(1, 0, 0);

	if (dot(normal, forward_vec) < 0)
	{	
		local_time += 0.5;
	}

	//correct value to range 0 - 1 and output to frag shader
	/*
	if (local_time < 0)
		local_time += 1;
	else if (local_time > 1)
		local_time -= 1;
	*/

	//Get temp from texture
	vec3 tempvec3 = texture(thermaltexture, local_time).rgb;

	//Output colour
	outputColor= Colour_False(Latitude_TempCorr(latitude, tempvec3), 70, 440); 

	/*
		Debug code - comment out the output colour on the line above and enable following lines to debug
	*/

	//check the time values of the bug lines 
	//if (temp > 170 && temp < 300) outputColor = vec4(1, local_time, 0, 1);

	//check time
	//if (local_time < 0 || local_time > 1) outputColor = vec4(1, 0, 0, 1);
	//else outputColor = vec4(local_time, local_time, local_time, local_time);

	//check local slope
	//outputColor = vec4(clamp(local_slope, 0, 1), clamp(local_slope, 0, 1), clamp(local_slope, 0, 1), 1);

	//check azimuth
	//outputColor = vec4(azimuth/6.28318530718, azimuth/6.28318530718, azimuth/6.28318530718, 1);

	//check latitude
	//if (latitude < 0) outputColor = vec4(1, 0, 0, 1);
	//else if (latitude > 1.57079632679) outputColor = vec4(0, 1, 0, 1);
	//else outputColor = vec4(latitude/1.57079632679, latitude/1.57079632679, latitude/1.57079632679, 1);

	
}

//Function definitions

//Function used to transform calculated temperature to a linear greyscale colour where <= min_val shows black and >= max_val shows white
vec4 Colour_Greyscale(float temp, float min_val, float max_val)
{
	float value = clamp((temp - min_val)/(max_val - min_val), 0, 1);

	return vec4(value, value, value, 1);
}

//Function used to transfor calculated temperature to a false colour output from black to purple to red to orange to yellow to white. temp of <= min_val
//is displayed as black, temp of >= max_val displayed as white.
vec4 Colour_False(float temp, float min_val, float max_val)
{
	float value = clamp((temp - min_val)/(max_val - min_val), 0, 1);

	float R, G, B;

	//Calculate Red Values
	if (value >= 0.05) R = (1 / 0.25) * (value - 0.05);
	else if(value > 0.3) R = 1;
	else R = 0;

	//Calculate Green Values
	if (value < 0.6) G = 0;
	else if (value <= 0.8) G = (1/0.2) * (value - 0.6);
	else G = 1;

	//Calculate Blue values
	if (value < 0.3) B = (0.5/0.3) * value;
	else if (value <= 0.6) B = (0.5/0.3) * (0.3 - (value - 0.3));
	else if (value >= 0.75) B = (1/0.25) * (value - 0.75);
	else B = 0;

	return vec4(R,G,B,1);
}

float Latitude_TempCorr(float lat, vec3 temps)
{
	if (lat > RADIANS60) return temps.b;
	else if (lat > RADIANS30) return mix(temps.g, temps.b, (lat - RADIANS30)/RADIANS30);
	else if (lat > 0) return mix(temps.r, temps.g, lat/RADIANS30);
	else return temps.r;
}
