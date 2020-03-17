#version 130
in vec4 in_position;

uniform mat4 TVP;

void main()
{
    gl_Position = TVP * in_position;
}
