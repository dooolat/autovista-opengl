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
uniform bool uHeadlightsEnabled;
uniform vec3 uHeadlightPositions[2];
uniform vec3 uHeadlightDirection;
uniform bool uPainted;
uniform float uShininess;
uniform float uSpecularStrength;
uniform Light uLights[MAX_LIGHTS];

void main() {
    vec3 N = normalize(vWorldNormal);
    vec3 V = normalize(uViewPos - vWorldPos);

    vec3 baseColor = uUseTexture != 0 ? texture(uFloorTexture, vTexCoord).rgb : uColor;
    vec3 result = 0.15 * baseColor; // ambient

    float shininess = uPainted ? uShininess : 24.0;
    float specularStrength = uPainted ? uSpecularStrength : 0.16;

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (uLights[i].enabled == 0) continue;

        vec3  toLight = uLights[i].position - vWorldPos;
        float dist    = length(toLight);
        vec3  L       = toLight / dist;
        vec3  H       = normalize(L + V);

        float diff = max(dot(N, L), 0.0);
        float spec = (diff > 0.0) ? pow(max(dot(N, H), 0.0), shininess) : 0.0;
        float atten = 1.0 / (1.0 + 0.045 * dist + 0.0075 * dist * dist);

        vec3 lightCol = uLights[i].color * uLights[i].intensity;
        result += atten * lightCol * (diff * baseColor + specularStrength * spec);
    }

    if (uHeadlightsEnabled) {
        for (int i = 0; i < 2; ++i) {
            vec3 ray = vWorldPos - uHeadlightPositions[i];
            float distanceSquared = max(dot(ray, ray), 0.0001);
            vec3 direction = ray * inversesqrt(distanceSquared);
            float cone = smoothstep(cos(radians(22.0)), cos(radians(12.0)),
                                    dot(direction, uHeadlightDirection));
            float diffuse = max(dot(N, -direction), 0.0);
            result += baseColor * vec3(1.0, 0.95, 0.82) * cone * diffuse
                      * (5.0 / (1.0 + 0.3 * distanceSquared));
        }
    }
    FragColor = vec4(result, 1.0);
}
