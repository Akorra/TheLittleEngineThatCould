// Simple vertex and fragment shaders
const char* vertexShaderSrc = R"(
#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in vec4 a_Color;

uniform mat4 u_MVP;
uniform mat4 u_Model;

out vec3 v_Normal;
out vec4 v_Color;

void main()
{
    gl_Position = u_MVP * vec4(a_Position, 1.0);
    v_Normal = mat3(u_Model) * a_Normal;
    v_Color = a_Color;
}
)";

const char* fragmentShaderSrc = R"(
#version 330 core

in vec3 v_Normal;
in vec4 v_Color;

out vec4 FragColor;

uniform vec4 u_Color;

void main()
{
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(normalize(v_Normal), lightDir), 0.0);
    vec3 ambient = vec3(0.3);
    vec3 diffuse = vec3(diff);
    
    vec3 lighting = ambient + diffuse;
    FragColor = vec4(u_Color.rgb * lighting, u_Color.a);
}
)";