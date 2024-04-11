#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
out mat3 tbnMatrix;

void main()
{
    vec3 T = normalize(vec3(model*vec4(aTangent, 0.0)));
    vec3 N = normalize(vec3(model*vec4(aNormal, 0.0)));
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T, B, N);

    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    tbnMatrix = TBN;
}