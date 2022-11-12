#version 450 core

in vec4 vertColor;
in vec2 textCoordV;

uniform sampler2D tex;
uniform vec3 nearFarMaxFog;
uniform vec4 fogColor;

out vec4 finalColor;

void main(void)
{
    //gl_FragColor = vertColor;
    //finalColor = vertColor;
    float near = nearFarMaxFog.x;
    float far = nearFarMaxFog.y;
    float MaxFrog = nearFarMaxFog.z;
    float realz = (near * far) / (far + (near - far) * gl_FragCoord.z);

    float fogRatio = 1.0;
    if(realz < MaxFrog)
    {
        fogRatio = (realz - near)/(MaxFrog - near);
    }

    const float ratio = 0.2;
    vec4 objColor =mix(texture(tex, textCoordV), vertColor, ratio);
    
    finalColor = mix(objColor, fogColor, fogRatio);
}