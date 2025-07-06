#version 430 core

// Structs.
struct Transform
{
    ivec2 atlasOffset;
    ivec2 spriteSize;
    vec2 pos;
    vec2 size;
};

// Input.
layout (std430, binding = 0) buffer TransformSBO
{
    Transform transforms[];
};

uniform vec2 screenSize;
uniform mat4 orthoProjection;

// Output.
layout (location = 0) out vec2 outTexCoords;

void main()
{
    Transform transform = transforms[gl_InstanceID];

    vec2 vertices[6];
    vertices[0] = transform.pos;
    vertices[1] = vec2(transform.pos + vec2(0.0, transform.size.y));
    vertices[2] = vec2(transform.pos + vec2(transform.size.x, 0.0));
    vertices[3] = vec2(transform.pos + vec2(transform.size.x, 0.0));
    vertices[4] = vec2(transform.pos + vec2(0.0, transform.size.y));
    vertices[5] = transform.pos + transform.size;

    float left = transform.atlasOffset.x;
    float top = transform.atlasOffset.y;
    float right = transform.atlasOffset.x + transform.spriteSize.x;
    float bottom = transform.atlasOffset.y + transform.spriteSize.y;

    vec2 texCoords[6];
    texCoords[0] = vec2(left, top);
    texCoords[1] = vec2(left, bottom);
    texCoords[2] = vec2(right, top);
    texCoords[3] = vec2(right, top);
    texCoords[4] = vec2(left, bottom);
    texCoords[5] = vec2(right, bottom);


    // Normalize position.
    {
        vec2 vertPos = vertices[gl_VertexID];
        // vertPos.y = -vertPos.y + screenSize.y;
        // vertPos = 2.0 * (vertPos / screenSize) - 1.0;
        gl_Position = orthoProjection * vec4(vertPos, 0.0, 1.0);
    }
    outTexCoords = texCoords[gl_VertexID];
}