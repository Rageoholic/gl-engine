#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aCol;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out vec3 FragPos;
out vec3 FragCol;

void main()
{
    gl_Position = proj * view * model * vec4(aPos, 1.0);
    FragPos = aPos;
    FragCol = aCol;
}
