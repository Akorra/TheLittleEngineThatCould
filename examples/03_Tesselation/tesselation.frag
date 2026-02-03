#version 400 core
out vec4 FragColor;

in vec3 v_normal;
in vec3 v_fragPos;
in vec3 v_tessCoord;

uniform vec3 u_lightPos;
uniform vec3 u_viewPos;

void main() {
    // Color based on tessellation coordinates
    vec3 baseColor = vec3(0.2, 0.6, 0.3) + v_tessCoord * 0.3;
    
    // Basic lighting
    vec3 norm = normalize(v_normal);
    vec3 lightDir = normalize(u_lightPos - v_fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    
    vec3 ambient = 0.3 * baseColor;
    vec3 diffuse = diff * baseColor;
    
    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0);
}