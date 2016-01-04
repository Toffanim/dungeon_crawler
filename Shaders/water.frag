#version 330 core
out vec4 color;
in vec2 TexCoords;

uniform sampler2D reflection_texture;

void main()
{   
    color = vec4(texture( reflection_texture, TexCoords));
    //vec3 projCoords = TexCoords.xyz / TexCoords.w;
    color = vec4(1.0);
}
