#version 450

layout (location = 0) in vec3 inPos;
layout (location = 0) out vec3 outColor;

out gl_PerVertex 
{
  vec4 gl_Position;   
};

void main() {
  outColor = vec3(abs(inPos.y), max(0.0, sign(inPos.x)), max(0.0, -sign(inPos.x)));
  gl_Position = vec4(inPos.xyz, 1.0);
}
