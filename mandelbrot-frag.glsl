#version 330

#define MAX 2.0

out vec4 outColor;

uniform int uDepth = 32;

in vec2 fragPos;

uniform vec2 uCenter = vec2(0.0, 0.0);
uniform vec2 uScale = vec2(0.5, 1.0);

bool inMandelbrot(vec2 start, vec2 number, int depth)
{
	vec2 current = start;
	for(int i = 0; i < depth; i++)
	{
		if(current.x * current.x + current.y * current.y > MAX * MAX)
			return false;

		vec2 temp;
		temp.x = current.x * current.x - current.y * current.y + number.x;
		temp.y = current.x * current.y * 2.0 + number.y;

		current = temp;
	}

	return true;
}

void main()
{
	outColor = vec4(0.0, 1.0, 0.0, 1.0);

	if(inMandelbrot(vec2(0.0, 0.0), vec2(fragPos.x / uScale.x, fragPos.y / uScale.y) + uCenter, uDepth))
		outColor = vec4(0.0, 0.0, 0.0, 1.0);
}
