#version 330 core
struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    float shininess;
    }; 
/* Note: because we now use a material struct again you want to change your
mesh class to bind all the textures using material.texture_diffuseN instead of
texture_diffuseN. */

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 1

in VS_OUT {
    vec4 TexCoords2;
    vec2 TexCoords;
    vec3 FragPos;
    vec4 LightFragPosition;
    vec3 Normal;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

out vec4 color;

uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;
uniform sampler2D reflection_texture;
uniform bool useNormalMapping;

// Function prototypes
vec4 CalcPointLight(PointLight light, Material mat, vec3 normal, 
                    vec3 fragPos, vec4 lightFragPos, vec3 viewDir, vec3 lightDir);


vec3 calculateAmbient(vec3 ambientLight, vec3 materialAmbient)
{
	return ambientLight * materialAmbient;
}


vec3 calculateDiffuse(vec3 diffuseLight, vec3 materialDiffuse, vec3 normal, vec3 directionToLight)
{
	return diffuseLight * materialDiffuse * max(0, dot(normal, directionToLight));
}


vec3 calculateSpecular(vec3 specularLight, vec3 materialSpecular, float materialShininess, 
                       vec3 normal, vec3 directionToLight, vec3 directionFromEye)
{
	vec3 h = normalize(directionToLight - directionFromEye);
	float normalizationFactor = ((materialShininess + 2.0)/8.0);
	return specularLight * materialSpecular * pow(max(0,dot(h, normal)),materialShininess) * normalizationFactor; 
}

vec4 calculateFresnel(vec3 materialSpecular, vec3 normal, vec3 directionFromEye)
{
        float fresnel = clamp(1.0-dot(directionFromEye, normal), 0.0, 1.0);
	return vec4(materialSpecular, fresnel); 
}

void main()
{    
    vec4 result;
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 lightDir = normalize(pointLights[0].position - fs_in.FragPos);
    vec3 norm = normalize(fs_in.Normal);

    if(useNormalMapping)
    {
        norm = texture(material.texture_normal1, fs_in.TexCoords).rgb;
        norm = normalize(norm * 2.0 - 1.0);
        viewDir = normalize( fs_in.TangentViewPos - fs_in.TangentFragPos );
        lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    }
     
     for(int i = 0; i < NR_POINT_LIGHTS; i++)
         result += CalcPointLight(pointLights[i], material, norm, fs_in.FragPos, 
                              fs_in.LightFragPosition, viewDir, lightDir);

    color = result;
}

// Calculates the color when using a point light.
vec4 CalcPointLight(PointLight light, Material mat, vec3 normal, vec3 fragPos, 
                    vec4 lightFragPos, vec3 viewDir, vec3 lightDir)
{
     
     // Attenuation
     float distance = length(light.position - fragPos);
     float attenuation = 1.0f / (light.constant + 
                                  light.linear * distance 
                                 + light.quadratic * (distance * distance));   

    vec2 texcoords = fs_in.TexCoords2.xy / fs_in.TexCoords2.w *0.5 + 0.5;
    texcoords = vec2( texcoords.x, texcoords.y -0.05);
    vec3 specularColor = vec3(texture( reflection_texture, texcoords)); 
    vec4 lighting =  (
        calculateFresnel(specularColor, normal, viewDir)
    );
    return (lighting);
}

