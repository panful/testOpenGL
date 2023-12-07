#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform bool hdr;
uniform bool exposureSwitch;
uniform float exposure;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    if(hdr)
    {
        vec3 result;
        if(exposureSwitch)
        {
            // exposure
            result = vec3(1.0) - exp(-hdrColor * exposure);
        }
        else
        {
            // reinhard
            result = hdrColor / (hdrColor + vec3(1.0));
        }

        // gamma校正  
        result = pow(result, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }
    else
    {
        vec3 result = pow(hdrColor, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }
}