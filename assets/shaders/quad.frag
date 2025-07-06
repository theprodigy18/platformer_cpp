#version 430 core

// Input.
layout (location = 0) in vec2 textureCoords;

// Output.
layout (location = 0) out vec4 outColor;

// Bindings.
layout (location = 0) uniform sampler2D textureAtlas;

void main()
{
    vec4 texColor = texelFetch(textureAtlas, ivec2(textureCoords), 0);

    if (texColor.a == 0.0)
    {
        discard;
    }

    outColor = texColor;
}