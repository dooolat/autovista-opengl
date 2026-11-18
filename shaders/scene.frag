#version 330 core
in vec3 vWorldPos;
in vec3 vWorldNormal;
in vec2 vTexCoord;
out vec4 FragColor;

#define MAX_LIGHTS 3

struct Light {
    vec3  position;
    vec3  color;
    float intensity;
    int   enabled;
};

uniform vec3  uColor;
uniform sampler2D uFloorTexture;
uniform int uUseTexture;
uniform vec3  uViewPos;
uniform float uShininess;
uniform float uSpecularStrength;
uniform Light uLights[MAX_LIGHTS];

void main() {
    vec3 N = normalize(vWorldNormal);
    vec3 V = normalize(uViewPos - vWorldPos);

    vec3 baseColor = uUseTexture != 0 ? texture(uFloorTexture, vTexCoord).rgb : uColor;
    vec3 result = 0.15 * baseColor; // ambient

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (uLights[i].enabled == 0) continue;

        vec3  toLight = uLights[i].position - vWorldPos;
        float dist    = length(toLight);
        vec3  L       = toLight / dist;
        vec3  H       = normalize(L + V);

        float diff = max(dot(N, L), 0.0);
        float spec = (diff > 0.0) ? pow(max(dot(N, H), 0.0), uShininess) : 0.0;
        float atten = 1.0 / (1.0 + 0.045 * dist + 0.0075 * dist * dist);

        vec3 lightCol = uLights[i].color * uLights[i].intensity;
        result += atten * lightCol * (diff * baseColor + uSpecularStrength * spec);
    }

    FragColor = vec4(result, 1.0);
}
