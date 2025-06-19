#version 330 core

in vec2 gStartPos;  // 直线起始点（所有片段都是同一个值），屏幕坐标系
in vec2 gScreenPos; // 当前片段（每个片段的值都不同），屏幕坐标系

out vec4 fragColor;

void main()
{
    float repeat = 15.0;
    float dash = 10.0;
    float length = distance(gStartPos, gScreenPos);
    if (mod(length, repeat) > dash) {
        discard;
    }

    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
