// fragment shader
// Written by: Sean Brown
#version 120
#extension GL_ARB_gpu_shader5 : enable

uniform float radii;
uniform sampler2D sampler;

void main() {

	vec2 p = gl_PointCoord * 2.0 - vec2(1.0);
	if(dot(p, p) > radii)
		discard;

	gl_FragColor = texture2D(sampler, gl_PointCoord);

}
