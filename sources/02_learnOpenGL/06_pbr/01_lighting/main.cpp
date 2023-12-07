/*
 * 1. PBR(Physically Based Rendering)基于物理的渲染
 * 2. 带贴图的PBR
 */

#define TEST2

#ifdef TEST1

#include <common.hpp>
#include <numbers>

// @brief 创建一个球心在(0,0,0)的球体
// @param [in] longitude 经线上的顶点个数
// @param [in] latitude 纬线上的顶点个数
// @param [in] radius 半径
Renderer CreateSphere(uint32_t longitude = 32, uint32_t latitude = 32, float radius = 1.f)
{
    longitude = longitude > 3 ? longitude : 3;
    latitude  = latitude > 3 ? latitude : 3;

    std::vector<float> vertices;
    vertices.reserve((latitude * (longitude - 2) + 2) * 3);
    std::vector<uint32_t> indices;
    indices.reserve((longitude - 2) * latitude * 2);

    auto deltaLatitude  = (2 * std::numbers::pi_v<float> / latitude);
    auto deltaLongitude = (std::numbers::pi_v<float> / (longitude - 1));

    // 最上面单独的一个点
    vertices.emplace_back(0.f);
    vertices.emplace_back(radius);
    vertices.emplace_back(0.f);
    // 法线
    vertices.emplace_back(0.f);
    vertices.emplace_back(radius);
    vertices.emplace_back(0.f);

    // 每一层，即纬线所在的圈
    for (size_t i = 1; i < longitude - 1; ++i)
    {
        auto r = radius * std::sin(i * deltaLongitude);
        auto y = radius * std::cos(i * deltaLongitude);

        // 每一层上的每一个点（纬线上的每一个点）
        for (size_t j = 0; j < latitude; ++j)
        {
            auto x = r * std::sin(j * deltaLatitude);
            auto z = r * std::cos(j * deltaLatitude);

            vertices.emplace_back(x);
            vertices.emplace_back(y);
            vertices.emplace_back(z);
            // 法线
            vertices.emplace_back(x);
            vertices.emplace_back(y);
            vertices.emplace_back(z);
        }
    }

    // 最下面单独的一个点
    vertices.emplace_back(0.f);
    vertices.emplace_back(-radius);
    vertices.emplace_back(0.f);
    // 法线
    vertices.emplace_back(0.f);
    vertices.emplace_back(-radius);
    vertices.emplace_back(0.f);

    //---------------------------------------------------
    // 北极圈
    for (unsigned int j = 1; j < latitude; ++j)
    {
        indices.emplace_back(0);
        indices.emplace_back(j);
        indices.emplace_back(j + 1);
    }
    indices.emplace_back(0);
    indices.emplace_back(latitude);
    indices.emplace_back(1);

    // 中间
    for (unsigned int i = 1; i + 2 < longitude; ++i)
    {
        auto start = (1 + (i - 1) * latitude);

        for (unsigned int j = 0; j + 1 < latitude; ++j)
        {
            indices.emplace_back(start + j);
            indices.emplace_back(start + j + latitude);
            indices.emplace_back(start + j + latitude + 1);

            indices.emplace_back(start + j);
            indices.emplace_back(start + j + latitude + 1);
            indices.emplace_back(start + j + 1);
        }

        indices.emplace_back(start + latitude - 1);
        indices.emplace_back(start + latitude - 1 + latitude);
        indices.emplace_back(start + latitude);

        indices.emplace_back(start + latitude - 1);
        indices.emplace_back(start + latitude);
        indices.emplace_back(start);
    }

    // 南极圈
    auto south = (longitude - 2) * latitude + 1;
    assert(south > latitude);
    for (unsigned int i = 1; i < latitude; ++i)
    {
        indices.emplace_back(south);
        indices.emplace_back(south - i);
        indices.emplace_back(south - i - 1);
    }
    indices.emplace_back(south);
    indices.emplace_back(south - latitude);
    indices.emplace_back(south - 1);

    return Renderer(vertices, indices, { 3, 3 });
}

// 金属材质没有漫反射，只有镜面反射，绝缘体（电介质）才有漫反射，即折射
// 折射和反射的和为1，不能大于1

// 辐射率(Radiance)：量化单一方向上光线的大小或强度
// 辐射通量(Radiant Flux)：单位时间光源的能量，单位为瓦特(W)或流明(lumen,lm)
// 立体角(Solid Angle)：
// 辐射强度：单位时间内光源在单位立体角上的能量

int main()
{
    InitOpenGL init(Camera({ 0.f, 0.f, 30.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }));
    auto window = init.GetWindow();

    auto sphere = CreateSphere();

    ShaderProgram program("shaders/02_06_01_TEST1.vs", "shaders/02_06_01_TEST1.fs");
    program.Use();
    program.SetUniform3f("albedo", 0.5f, 0.0f, 0.0f);
    program.SetUniform1f("ao", 1.0f);

    // 光源的位置和颜色
    // clang-format off
    glm::vec3 lightPositions[] = {
        glm::vec3(-5.0f,  5.0f, 10.0f),
        glm::vec3( 5.0f,  5.0f, 10.0f),
        glm::vec3(-5.0f, -5.0f, 10.0f),
        glm::vec3( 5.0f, -5.0f, 10.0f),
    };

    glm::vec3 lightColors[] = { 
        glm::vec3(300.0f, 300.0f, 300.0f), 
        glm::vec3(300.0f, 300.0f, 300.0f), 
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f) 
    };
    // clang-format on

    // 用来控制生成7*7=49个球
    int nrRows      = 7;
    int nrColumns   = 7;
    float spacing   = 3.0f;
    glm::mat4 model = glm::mat4(1.f);

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();
        program.SetUniform3fv("camPos", init.GetViewPosition());
        program.SetUniformMat4("view", init.GetViewMatrix());
        program.SetUniformMat4("projection", init.GetProjectionMatrix());

        // 渲染共 7*7=49个球
        for (int row = 0; row < nrRows; ++row)
        {
            program.SetUniform1f("metallic", (float)row / (float)nrRows);
            for (int col = 0; col < nrColumns; ++col)
            {
                program.SetUniform1f("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

                auto x = (col - (nrColumns / 2)) * spacing;
                auto y = (row - (nrRows / 2)) * spacing;

                model = glm::mat4(1.f);
                model = glm::translate(model, glm::vec3(x, y, 0.0f));
                program.SetUniformMat4("model", model);
                program.SetUniformMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

                sphere.Draw(GL_TRIANGLES);
            }
        }

        // 光源
        for (size_t i = 0; i < 4; ++i)
        {
            program.SetUniform3fv("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
            program.SetUniform3fv("lightColors[" + std::to_string(i) + "]", lightColors[i]);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <common.hpp>
#include <numbers>

Renderer CreateSphere(uint32_t longitude = 10, uint32_t latitude = 10, float radius = 1.0f)
{
    auto THE_PI = std::numbers::pi_v<float>;

    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    // Generate vertices
    for (uint32_t lat = 0; lat <= latitude; ++lat)
    {
        float theta    = lat * THE_PI / latitude;
        float sinTheta = std::sin(theta);
        float cosTheta = std::cos(theta);

        for (uint32_t lon = 0; lon <= longitude; ++lon)
        {
            float phi    = lon * 2 * THE_PI / longitude;
            float sinPhi = std::sin(phi);
            float cosPhi = std::cos(phi);

            // Vertex position
            float x = radius * cosPhi * sinTheta;
            float y = radius * cosTheta;
            float z = radius * sinPhi * sinTheta;

            // Vertex normal (same as position)
            float nx = x;
            float ny = y;
            float nz = z;

            // Vertex UV coordinates (spherical mapping)
            float u = 1.0f - static_cast<float>(lon) / longitude;
            float v = 1.0f - static_cast<float>(lat) / latitude;

            // Add vertex attributes to the vector
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    // Generate indices
    for (uint32_t lat = 0; lat < latitude; ++lat)
    {
        for (uint32_t lon = 0; lon < longitude; ++lon)
        {
            uint32_t curr = lat * (longitude + 1) + lon;
            uint32_t next = curr + longitude + 1;

            // Add indices for the two triangles forming each quad
            indices.push_back(curr);
            indices.push_back(next);
            indices.push_back(curr + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
            indices.push_back(curr + 1);
        }
    }

    std::cout << "points: " << vertices.size() / 8 << "\tindices: " << indices.size() / 3 << '\n';
    return Renderer(vertices, indices, { 3, 3, 2 });
}

int main()
{
    InitOpenGL init(Camera({ 0.f, 0.f, 30.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }));
    auto window = init.GetWindow();

    auto sphere = CreateSphere(64, 64);

    Texture texAlbedo("textures/rusted_iron/albedo.png");
    Texture texNormal("textures/rusted_iron/normal.png");
    Texture texMetallic("textures/rusted_iron/metallic.png");
    Texture texRoughness("textures/rusted_iron/roughness.png");
    Texture texAo("textures/rusted_iron/ao.png");

    ShaderProgram program("shaders/02_06_01_TEST1.vs", "shaders/02_06_01_TEST2.fs");
    program.Use();
    program.SetUniform1i("albedoMap", 0);
    program.SetUniform1i("normalMap", 1);
    program.SetUniform1i("metallicMap", 2);
    program.SetUniform1i("roughnessMap", 3);
    program.SetUniform1i("aoMap", 4);

    // 光源的位置和颜色
    // clang-format off
    glm::vec3 lightPositions[] = {
        glm::vec3(-5.0f,  5.0f, 10.0f),
        glm::vec3( 5.0f,  5.0f, 10.0f),
        glm::vec3(-5.0f, -5.0f, 10.0f),
        glm::vec3( 5.0f, -5.0f, 10.0f),
    };

    glm::vec3 lightColors[] = { 
        glm::vec3(300.0f, 300.0f, 300.0f), 
        glm::vec3(300.0f, 300.0f, 300.0f), 
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f) 
    };
    // clang-format on

    // 用来控制生成7*7=49个球
    int nrRows      = 7;
    int nrColumns   = 7;
    float spacing   = 3.0f;
    glm::mat4 model = glm::mat4(1.f);

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();
        program.SetUniform3fv("camPos", init.GetViewPosition());
        program.SetUniformMat4("view", init.GetViewMatrix());
        program.SetUniformMat4("projection", init.GetProjectionMatrix());
        texAlbedo.Use(0);
        texNormal.Use(1);
        texMetallic.Use(2);
        texRoughness.Use(3);
        texAo.Use(4);

        // 渲染共 7*7=49个球
        for (int row = 0; row < nrRows; ++row)
        {
            for (int col = 0; col < nrColumns; ++col)
            {
                auto x = (col - (nrColumns / 2)) * spacing;
                auto y = (row - (nrRows / 2)) * spacing;

                model = glm::mat4(1.f);
                model = glm::translate(model, glm::vec3(x, y, 0.0f));
                program.SetUniformMat4("model", model);
                program.SetUniformMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

                sphere.Draw(GL_TRIANGLES);
            }
        }

        // 光源
        for (size_t i = 0; i < 4; ++i)
        {
            program.SetUniform3fv("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
            program.SetUniform3fv("lightColors[" + std::to_string(i) + "]", lightColors[i]);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST2