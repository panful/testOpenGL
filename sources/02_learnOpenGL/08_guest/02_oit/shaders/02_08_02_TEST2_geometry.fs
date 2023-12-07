#version 450

layout (early_fragment_tests) in;

struct Node
{
    vec4 color;
    float depth;
    uint next;
};

layout (binding = 1) buffer geometrySBO
{
    uint count;
    uint maxNodeCount;
};

layout (r32ui, binding = 2) uniform uimage2D headIndexImage;

layout (binding = 3) buffer LinkedListSBO
{
    Node nodes[];
};

uniform vec4 uColor;

void main()
{
    // 增加节点数
    uint nodexIdx = atomicAdd(count, 1);

    // 判断链表是否满溢
    if(nodexIdx < maxNodeCount)
    {
        // 交换新的链表头索引和以前的链表头
        uint prevHeadIdx = imageAtomicExchange(headIndexImage, ivec2(gl_FragCoord.xy), nodexIdx);

        // 存储数据至链表
        nodes[nodexIdx].color = uColor;
        nodes[nodexIdx].depth = gl_FragCoord.z;
        nodes[nodexIdx].next  = prevHeadIdx;
    }
}

