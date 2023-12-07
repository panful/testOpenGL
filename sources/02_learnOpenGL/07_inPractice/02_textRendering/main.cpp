/*
 * 1. FreeType的使用示例
 */

#define TEST1

#ifdef TEST1

#include "common.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H
#include <map>

// 字形的freetype信息
struct Character
{
    GLuint TextureID;   // 字形纹理的ID
    glm::ivec2 Size;    // 字形大小
    glm::ivec2 Bearing; // 从基准线到字形左部/顶部的偏移值
    FT_Pos Advance;     // 原点距下一个字形原点的距离
};

// 字形以及字形的freetype信息
std::map<GLchar, Character> Characters;

constexpr GLuint WIDTH { 800 }, HEIGHT { 600 };
GLuint VAO { 0 }, VBO { 0 };

// xy    字符左下角在窗口中的位置
// scale 字符的缩放比例
// color 字符的颜色
void RenderText(const ShaderProgram& program, const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

int main()
{
    InitOpenGL initOpenGL("textRendering", WIDTH, HEIGHT);
    auto window = initOpenGL.GetWindow();

    ShaderProgram program("shaders/02_07_02_TEST1.vs", "shaders/02_07_02_TEST1.fs");

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 窗口左下角(0,0)，左上角(WIDTH,HEIGHT)，点(0,0)会在窗口的左下角而不是窗口的正中间
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(WIDTH), 0.0f, static_cast<GLfloat>(HEIGHT));
    program.Use();
    program.SetUniformMat4("projection", projection);

    // 初始化FreeType
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library\n";
    }

    // 将字体信息保存到FT_Face
    FT_Face face;
    if (FT_New_Face(ft, "shaders/arial.ttf", 0, &face))
    {
        std::cerr << "ERROR::FREETYPE: Failed to load font\n";
    }

    // 设置字体的大小
    FT_Set_Pixel_Sizes(face, 0, 48);

    // 禁用字节对齐限制
    // OpenGL要求所有的纹理都是4字节对其的，即纹理的大小永远是4字节的倍数
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // 生成ASCII字符集的前128个字符
    for (GLubyte c = 0; c < 128; c++)
    {
        // 加载字符的字形
        // 通过FT_LOAD_RENDER标记设置字形信息为：8位的灰度位图
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cerr << "ERROR::FREETYTPE: Failed to load Glyph\n";
            continue;
        }

        // 字形的位图数据
        auto bitmap = face->glyph->bitmap;

        // 生成纹理
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // 注意format设置为GL_RED
        // 通过字形生成的位图是一个8位灰度图，每一个颜色都由一个字节来表示
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bitmap.width, bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // 保存字符的字形信息供后面使用
        // clang-format off
        Character character = { 
            texture, 
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top), 
            face->glyph->advance.x
        };
        // clang-format on

        Characters.try_emplace(c, character);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // 清理FreeType资源
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        RenderText(program, "This is sample text", 10.f, 10.f, 1.f, glm::vec3(1.f, 0.f, 1.f));
        RenderText(program, "(C) LearnOpenGL.com", 400.f, 300.f, .5f, glm::vec3(1.f, 1.f, 0.f));
        RenderText(program, "Right Top", 580.f, 550.f, 1.f, glm::vec3(0.f, 1.f, 1.f));

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void RenderText(const ShaderProgram& program, const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    program.Use();
    program.SetUniform3fv("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    for (auto& c : text)
    {
        Character ch = Characters[c];

        // 四边形的原点坐标
        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
        // 四边形的大小
        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;

        // 对每个字符更新VBO，一个四边形6个顶点，每个顶点油4个GLfloat组成，位置xy纹理uv
        // clang-format off
        GLfloat vertices[6][4] = { 
            { xpos,     ypos + h,   0.f,    0.f },
            { xpos,     ypos,       0.f,    1.f },
            { xpos + w, ypos,       1.f,    1.f },
            { xpos,     ypos + h,   0.f,    0.f },
            { xpos + w, ypos,       1.f,    1.f },
            { xpos + w, ypos + h,   1.f,    0.f },
        };
        // clang-format on

        // 在四边形上绘制字形纹理
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        // 更新VBO内存的内容，注意前面将内存类型设置为：GL_DYNAMIC_DRAW
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // 绘制四边形
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 更新位置到下一个字形的原点，注意单位是1/64像素
        // 位偏移6个单位来获取单位为像素的值(2^6 = 64)
        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

#endif // TEST1