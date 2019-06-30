// Fragment shader
//Started off from the lab3 shader and moved lighting from vertex to frag as shown in the lab

#version 420

#define PI 3.1415926538

in vec4 fcolour;
in float fU;
in float fUo;
in float fg;

out vec4 outputColor;

float H(float u, float w);

void main()
{

//Hapke parameters
const float w = 0.33; //from pangu video...
const float Bo = 1.0; //estimate from paper
float h = 0.4; //from paper looks like a guesstimate
float e = acos(fU);

//Average phase
float pg = ((4 * PI) / 5 ) * ( (sin(fg) + (PI - fg) * cos(fg)) / PI) + (pow((1 - cos(fg)), 2) / 10);
//float pg = 1.0f;

//Back scattering component
float Bg;
if (fg < 1)
	Bg = Bo * (1 - 3 * fg / 2 * h);
else if (fg <= PI/2)
	{
		Bg = Bo * (1 - (tan(fg) / (2 * h)) * (3 - pow(e, (-h/tan(fg)))) * (1 - pow(e, (-h/tan(fg)))));
	}
else
	Bg = 0;


//first part: w/4pi
float first = w / (4 * PI);
//second part: Uo/(Uo+U)
float second = fU / (fUo + fU);
//third part: {[i + B(g)]P(g) + H(Uo)H(u) -1)
float third = (1 + Bg) * pg + H(fUo, w) * H(fU, w) - 1;

float BRDF = first * second * third;

//Outputs
outputColor = fcolour * BRDF ;

}

float H(float u, float w)
{
	float top = 1 + 2 * u;
	float bottom = 1 + 2 * pow((1 - 2), (1/2)) * u;
	return top/bottom;
}
