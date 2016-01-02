#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;

out vec2 TexCoords;
out vec3 fragPosition;
out vec4 lightFragPosition;
out vec3 Normal;
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{

gl_Position = projection * view * model * vec4(position, 1.0f);
fragPosition = vec3(model * vec4(position, 1.0f));
lightFragPosition = projection * view * model * vec4(position, 1.0f);
Normal = mat3(transpose(inverse(model))) * normal;
TexCoords = texCoords;
}
