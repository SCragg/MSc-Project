// Fragment shader
//Started off from the lab3 shader and moved lighting from vertex to frag as shown in the lab

#version 420

#define PI 3.1415926538

in vec4 fcolour;
in vec3 fV;
in vec3 fN;
in vec3 fL;

out vec4 outputColor;

float Hapke_H(float u, float w);
float Hapke_B (float absg, float h); 

vec3 V = normalize(fV);
vec3 N = normalize(fN);
vec3 L = normalize(fL);


//Angle parameters for Hapke brdf
float Uo = dot(N, L); //cos i
float U = dot(N, V); //cos e
float g = acos(clamp(dot(L, V), -1.0, 1.0)); //phase angle
float absg = abs(g);

//Hapke parameters
const float w = 0.33; //from pangu video
const float Bo = 0.95; //estimate from paper
float h = 0.05; //from paper looks like a guesstimate
float e = acos(U);

void main()
{

//Average phase
float pg = ((4 * PI) / 5 ) * ( (sin(g) + (PI - g) * cos(g)) / PI) + (pow((1 - cos(g)), 2) / 10);

float Hapke = 0.0;
if (U > 0.0 && Uo > 0.0)
{
//first part: w/4pi
float first = w / (4 * PI);
//second part: Uo/(Uo+U)
float second = Uo / (Uo + U);
//float second = 1;
//third part: {[i + B(g)]P(g) + H(Uo)H(u) -1)
float third = (1 + Hapke_B(absg, h)) * pg + Hapke_H(Uo, w) * Hapke_H(U, w) - 1;

Hapke = first * second * third;
}
//Outputs
float lambert = max(0.0,Uo);
outputColor = fcolour * mix(lambert, Hapke, 1);

}

float Hapke_H(float u, float w)
{
	float top = 1 + 2 * u;
	float bottom = 1 + 2 * pow((1 - w), (1/2)) * u;
	return top/bottom;
}

//Backscatter function
float Hapke_B (float absg, float h)
{
if (absg < 1)
	return Bo * (1 - 3 * absg / 2 * h);

else if (absg <= PI/2)
	return Bo * (1 - (tan(absg) / (2 * h)) * (3 - pow(e, (-h/tan(absg)))) * (1 - pow(e, (-h/tan(absg)))));
	//return Bo * (1 - (tan(absg) / (2 * h)) * (3 - exp(-h/tan(absg))) * (1 - exp(-h/tan(absg))));

else
	return 0;
}