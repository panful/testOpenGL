﻿#version 330 core

out vec4 FragColor;

void main()
{
    // 如果片段进入了片段着色器就会在屏幕上绘制一个红色片段
    // 如果没有进入，即没有在屏幕上绘制一个红色片段
    // 则说明提前深度测试将片段在没有进入片段着色器之前就已经丢弃了
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
