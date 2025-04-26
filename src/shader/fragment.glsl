#version 460 core

out vec4 FragColor;
in vec3 ourColor;
uniform vec4 timeValue;

void main()
{
    FragColor = vec4(ourColor * timeValue.xyz, 1.0);
}
