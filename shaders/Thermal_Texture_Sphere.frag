/*

Thermal_Texture_Sphere.frag - Sean Cragg

*/

#version 420
#define PI 3.141592653589793
#define RADIANS30 0.523599
#define RADIANS60 1.0472

//inputs
in vec3 fNormal;
in vec3 fLightDir;
in float flocal_time;
in float latitude;
in vec3 fposition;

//Uniforms
uniform float global_time;
uniform int colourmode;
uniform sampler1D thermaltexture;

out vec4 outputColor;

//Function declarations
vec4 Colour_Greyscale(float temp, float min_val, float max_val);
vec4 Colour_False(float temp, float min_val, float max_val);
vec4 Colour_Rainbow(float temp, float min_val, float max_val);
float Latitude_Correction(float latitude, float azimuth, float local_slope);
float Latitude_TempCorr(float lat, vec3 temps);

//normalize input vectors
vec3 normal = normalize(fNormal);
vec3 L = normalize(fLightDir);
vec3 position = normalize(fposition);

void main()
{
	
	//float local global time
	float time;
	vec2 long_vec = vec2(position.x, position.y);
	float longitude = atan(long_vec.x, long_vec.y)/6.28318530718;
	time = global_time + longitude;
	if (time < 0)
		time += 1;
	else if (time > 1)
		time -= 1;

	//Calculations for local time
	float local_slope;
	local_slope = tan(acos(dot(normal, normalize(position))));

	//remove height and normalise
	vec3 absolute_normal = normal - normalize(position);
	vec2 heading_vec = vec2(normalize(absolute_normal).x, normalize(absolute_normal).z);
	normalize(heading_vec);

	//calcuate slope azimuth angle
	float azimuth = atan(heading_vec.x, heading_vec.y);
	if (azimuth < 0)
	{
		azimuth = 6.28318530718 + azimuth; // convert to range 0 - 2pi
	}

	//corrction equation
	float local_time = time + (0.5 / PI) * atan(local_slope * sin(azimuth));
	float lat = Latitude_Correction(latitude, azimuth, local_slope);

	//Used to clamp time to 0 - 1, not required as texture wraps around but may be useful in the future.
	/*
	vec3 forward_vec = vec3(0, 1, 0);
	vec3 right_vec = vec3(1, 0, 0);

	if (dot(normal, forward_vec) < 0)
	{	
		local_time += 0.5;
	}
	*/

	//correct value to range 0 - 1 and output to frag shader
	if (local_time < 0)
		local_time += 1;
	else if (local_time > 1)
		local_time -= 1;

	//Get temp from texture
	vec3 tempvec3 = texture(thermaltexture, local_time).rgb;

	//Output colour
	if (colourmode == 0)
		outputColor= Colour_Greyscale(Latitude_TempCorr(lat, tempvec3), 81.62370444, 390.4609914); 
	else if (colourmode == 1)
		outputColor= Colour_Rainbow(Latitude_TempCorr(lat, tempvec3), 0, 450); 
	else if (colourmode == 2)
		outputColor= Colour_False(Latitude_TempCorr(lat, tempvec3), 70, 450);

	/*
		Debug code - comment out the output colour on the line above and enable following lines to debug
	*/

	//check the time values of the bug lines 
	//if (temp > 170 && temp < 300) outputColor = vec4(1, local_time, 0, 1);

	//check time
	//if (local_time < 0 || local_time > 1) outputColor = vec4(1, 0, 0, 1);
	//else outputColor = vec4(local_time, local_time, local_time, local_time);

	//check colour scale
	//if (colourmode == 0)
		//outputColor= Colour_Greyscale(time, 0, 1); 
	//else if (colourmode == 1)
		//outputColor= Colour_Rainbow(time, 0, 1); 

	//check local slope
	//outputColor = vec4(clamp(local_slope, 0, 1), clamp(local_slope, 0, 1), clamp(local_slope, 0, 1), 1);

	//check azimuth
	//outputColor = vec4(azimuth/6.28318530718, azimuth/6.28318530718, azimuth/6.28318530718, 1);

	//check latitude
	//if (lat < 0) outputColor = vec4(1, 0, 0, 1);
	//else if (lat > 1.57079632679) outputColor = vec4(0, 1, 0, 1);
	//else outputColor = vec4(lat/1.57079632679, lat/1.57079632679, lat/1.57079632679, 1);

	//check longitude
	//if (time < 0 || time > 1) outputColor = vec4(1, 0, 0, 1);
	//outputColor = vec4(time, time, time,1);

	
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

vec4 Colour_Rainbow(float temp, float min_val, float max_val)
{
	float value = clamp((temp - min_val)/(max_val - min_val), 0, 1);

	float R, G, B;

	//Calculate Red values
	if (value < 0.222) R = -(value / 0.222) + 1;
	else if (value < 0.333) R = 0;
	else if (value < 0.556) R = (value-0.333) / 0.222;
	else if (value < 0.667) R = 1;
	else if (value < 0.889) R = -0.5*((value-0.667) / 0.222) + 1;
	else R = 0.5*((value - 0.889) / 0.111) + 0.5;

	//Calculate Green values
	if (value < 0.1667) G = -0.33*(value / 0.1667) + 0.33;
	else if (value < 0.2778) G = (value - 0.1667) / 0.1111;
	else if (value < 0.444) G = 1;
	else if (value < 0.667) G = -((value - 0.444) / 0.223) + 1;
	else if (value < 0.889) G = 0;
	else G = (value - 0.889)/0.111;

	//Calculate Blue values
	if (value < 0.222) B = 1;
	else if (value < 0.444) B = -((value - 0.222) / 0.222) + 1;
	else if (value < 0.889) B = 0;
	else B = (value - 0.889)/0.111;

	return vec4(R,G,B,1);
}

float Latitude_Correction(float latitude, float azimuth, float local_slope)
{
	float lat;
	lat = abs(latitude + atan(local_slope * cos(azimuth)));

	return lat;
		
	
}

float Latitude_TempCorr(float lat, vec3 temps)
{
	if (lat > RADIANS60) return temps.b;
	else if (lat > RADIANS30) return mix(temps.g, temps.b, (lat - RADIANS30)/RADIANS30);
	else if (lat > 0) return mix(temps.r, temps.g, lat/RADIANS30);
	else return temps.r;
}
