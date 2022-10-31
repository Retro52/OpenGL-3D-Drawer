#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

//void main()
//{
//    vec3 col = 1.0 - texture(screenTexture, TexCoords).rgb;
//
//    FragColor = vec4(col, 1.0);
//}

void main()
{
    const float gamma    = 0.45;
    const float exposure = 5.1;
    vec3 hdrColor = texture(screenTexture, TexCoords).rgb;

    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

    // gamma correction
    mapped = pow(mapped, vec3(1.0 / gamma));

    FragColor = vec4(mapped, 1.0);
}

