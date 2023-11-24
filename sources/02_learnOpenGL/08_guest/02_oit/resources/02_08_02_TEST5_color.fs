#version 450

#define MAX_FRAGMENT_COUNT 128

struct Node
{
    vec4 color;
    float depth;
    uint next;
};

layout (r32ui, binding = 0) uniform uimage2D headIndexImage;
layout (binding = 1) buffer LinkedListSBO
{
    Node nodes[];
};

out vec4 FragColor;

void main()
{
    Node fragments[MAX_FRAGMENT_COUNT]; // 构成一个像素的所有片段
    int count = 0;

    uint nodeIdx = imageLoad(headIndexImage, ivec2(gl_FragCoord.xy)).r;

    // 读取数据
    while(nodeIdx != 0xFFFFFFFF && count < MAX_FRAGMENT_COUNT)
    {
        fragments[count] = nodes[nodeIdx];
        nodeIdx = fragments[count].next;
        ++count;
    }

    // 使用插入排序，深度值从大到小
    for(uint i = 1; i < count; ++i)
    {
        Node insert = fragments[i];
        uint j = i;
        while(j > 0 && insert.depth > fragments[j-1].depth)
        {
            fragments[j] = fragments[j-1];
            --j;
        }
        fragments[j] = insert;
    }

    // 混合处理
    vec4 color = vec4(0.f, 0.f, 0.f, 1.f); // 背景
    for(int i = 0; i < count; ++i)
    {
        color = mix(color, fragments[i].color, fragments[i].color.a);
    }

    FragColor = color;
}
