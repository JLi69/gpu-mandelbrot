#version 430

layout(location=0) in vec4 pos;

out vec2 fragPos;

void main()
{
	gl_Position = vec4(float(pos.x), float(pos.y), 0.0, 1.0); 
	fragPos = pos.xy;
}
