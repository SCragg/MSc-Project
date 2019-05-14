// Fragment shader
//Started off from the lab3 shader and moved lighting from vertex to frag as shown in the lab

#version 420

#define PI 3.1415926538

in vec4 fcolour;
in vec3 fV;
in vec3 fN;
in vec3 fL;

out vec4 outputColor;

float H(float u, float w);

vec3 V = normalize(fV);
vec3 N = normalize(fN);
vec3 L = normalize(fL);

//Angle parameters for Hapke brdf
float Uo = dot(N, L); //cos i
float U = dot(N, V); //cos e
float g = acos(clamp(dot(L, V), -1.0, 1.0)); //phase angle

//Hapke parameters
const float w = 0.33; //from pangu video
const float Bo = 1.0; //estimate from paper
float h = 1; //from paper looks like a guesstimate
float e = acos(U);

void main()
{

//Average phase
float pg = ((4 * PI) / 5 ) * ( (sin(g) + (PI - g) * cos(g)) / PI) + (pow((1 - cos(g)), 2) / 10);
//float pg = 1.0f;

//Back scattering component
float Bg;
if (g < 1)
	Bg = Bo * (1 - 3 * g / 2 * h);
else if (g <= PI/2)
	{
		Bg = Bo * (1 - (tan(g) / (2 * h)) * (3 - pow(e, (-h/tan(g)))) * (1 - pow(e, (-h/tan(g)))));
	}
else
	Bg = 0;


//first part: w/4pi
float first = w / (4 * PI);
//second part: Uo/(Uo+U)
float second = U / (Uo + U);
//third part: {[i + B(g)]P(g) + H(Uo)H(u) -1)
float third = (1 + Bg) * pg + H(Uo, w) * H(U, w) - 1;

float BRDF = first * second * third;

//Outputs
outputColor = fcolour * BRDF;

}

float H(float u, float w)
{
	float top = 1 + 2 * u;
	float bottom = 1 + 2 * pow((1 - 2), (1/2)) * u;
	return top/bottom;
}
