#version 430

//Some performance options
#define DO_SECOND_PASS 1
#define MAX_IT_SECOND_PASS 200.0 

#define MAX 2.0
#define IN_MANDELBROT -1

out vec4 outColor;

uniform int uDepth = 128;

in vec2 fragPos;
uniform dvec2 uCenter = dvec2(0.0, 0.0);
uniform dvec2 uScale = dvec2(0.5, 1.0);

uniform dvec2 uNumber = dvec2(0.0, 0.0);

int inMandelbrot(dvec2 number, int depth)
{
	dvec2 current = number;
	for(int i = 0; i < depth; i++)
	{
		if(current.x * current.x + current.y * current.y > MAX * MAX)
			return i;
		
		dvec2 temp;
		temp.x = current.x * current.x - current.y * current.y + uNumber.x;
		temp.y = current.x * current.y * 2.0 + uNumber.y;

		current = temp;
	}

	return IN_MANDELBROT;
}

void main()
{
	outColor = vec4(1.0, 1.0, 1.0, 1.0);

	float steps = float(inMandelbrot(dvec2(double(fragPos.x) / uScale.x, double(fragPos.y) / uScale.y) + uCenter, uDepth));	

	if(steps == IN_MANDELBROT)
		outColor = vec4(0.0, 0.0, 0.0, 1.0);
	else
	{
		if(steps < uDepth / 2)
			outColor = vec4(0.0, 1.0, 0.0, 1.0) * (steps) / float(uDepth) * 2.0 + vec4(1.0, 0.0, 0.0, 1.0) * (uDepth / 2 - steps) / float(uDepth) * 2.0;
		else if(steps > uDepth / 2)	
			outColor = vec4(0.0, 0.0, 1.0, 1.0) * (steps) / float(uDepth) * 2.0 + vec4(0.0, 1.0, 0.0, 1.0) * (uDepth - steps) / float(uDepth) * 2.0;

		//Average out the color to make the image more appealing
		if(DO_SECOND_PASS == 1 && uDepth < MAX_IT_SECOND_PASS)
		{
			dvec2 offset = dvec2(1.0 / double(uScale.x) * 1.0 / 1000.0, 1.0 / double(uScale.y) * 1.0 / 1000.0); 
			float avgSteps = 0.0;
			float total = 0.0;
			for(double x = -1.0; x <= 1.0; x++)
			{
				for(double y = -1.0; y <= 1.0; y++)
				{
					float stepVal = inMandelbrot(dvec2(double(fragPos.x) / uScale.x, double(fragPos.y) / uScale.y) + uCenter + dvec2(offset.x * x, offset.y * y), uDepth);
					if(stepVal == IN_MANDELBROT)
						continue;

					total++;	
					avgSteps += float(stepVal);
				}	
			}	
			avgSteps /= total;

			if(avgSteps < float(uDepth / 2))
				outColor = vec4(0.0, 1.0, 0.0, 1.0) * float(avgSteps) / float(uDepth) * 2.0 + vec4(1.0, 0.0, 0.0, 1.0) * float(uDepth / 2 - avgSteps) / float(uDepth) * 2.0;
			else if(avgSteps > float(uDepth / 2))	
				outColor = vec4(0.0, 0.0, 1.0, 1.0) * float(avgSteps) / float(uDepth) * 2.0 + vec4(0.0, 1.0, 0.0, 1.0) * float(uDepth - avgSteps) / float(uDepth) * 2.0;
		}	
	}
}
