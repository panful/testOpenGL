/**
 * 1. 将点精灵(GL_POINTS)显示为圆形
 * 2
 * 3.
 * 4. 一维
 * 5. 
 * 6. 照抄视频源码
 * 7. 加入IMGUI
 */

#define TEST7

#ifdef TEST1

#include <common.hpp>

int main()
{
    InitOpenGL initOpenGL("Circle", 800, 600);
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("shaders/02_08_08_TEST1.vert", "shaders/02_08_08_TEST1.frag");

    // clang-format off
    std::vector<float> vertices {
        -.6f,  .0f, 
        -.3f,  .0f,
         .0f,  .0f,
         .3f,  .0f,
         .6f,  .0f,
    };
    // clang-format on

    Renderer pointSprite(vertices, { 2 }, GL_POINTS);

    glEnable(GL_PROGRAM_POINT_SIZE);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        program.SetUniform1f("window_height", 600.f);

        pointSprite.Draw();

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
#include <random>

GLuint VAO { 0 }, VBO { 0 };

constexpr size_t ParticleCount = 100;
constexpr double h             = 0.1;    // Smoothing length
constexpr double mass          = 1.0;    // Particle mass
constexpr double restDensity   = 1000.0; // Rest density
constexpr double stiffness     = 1000.0; // Gas stiffness
constexpr double viscosity     = 0.1;    // Viscosity coefficient

std::vector<glm::vec2> Position;
std::vector<glm::vec2> Velocity;
std::vector<float> Density;
std::vector<float> Pressure;

void Init()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDist(-1.f, 1.f);
    std::uniform_real_distribution<float> velDist(-1.f, 1.f);

    Position.resize(ParticleCount);
    Velocity.resize(ParticleCount);
    Density.resize(ParticleCount);
    Pressure.resize(ParticleCount);

    for (size_t i = 0; i < ParticleCount; ++i)
    {
        Position[i].x = posDist(gen);
        Position[i].y = posDist(gen);
        Velocity[i].x = velDist(gen);
        Velocity[i].y = velDist(gen);
    }
}

// Poly6 kernel function
double poly6Kernel(double r)
{
    double q = 1 - (r * r) / (h * h);
    return 315.0 / (64.0 * std::numbers::pi_v<double> * std::pow(h, 9)) * q * q * q;
}

// Spiky kernel function
double spikyKernel(double r)
{
    double q = 1 - r / h;
    return 15.0 / (std::numbers::pi_v<double> * std::pow(h, 6)) * q * q * q;
}

// Calculate density and pressure for each particle
void calculateDensityPressure()
{
    for (size_t i = 0; i < ParticleCount; ++i)
    {
        Density[i] = 0.f;

        for (size_t j = 0; j < ParticleCount; ++j)
        {
            if (i != j)
            {
                auto r = std::hypot((Position[i].x - Position[j].x), (Position[i].y - Position[j].y));
                Density[i] += static_cast<float>(mass * poly6Kernel(r));
            }
        }
        Pressure[i] = static_cast<float>(stiffness * (Density[i] - restDensity));
    }
}

// Update velocities and positions of particles
void updateParticles()
{
    for (size_t i = 0; i < ParticleCount; ++i)
    {
        double ax = 0.0, ay = 0.0;

        for (size_t j = 0; j < ParticleCount; ++j)
        {
            if (i != j)
            {
                auto r = std::hypot((Position[i].x - Position[j].x), (Position[i].y - Position[j].y));

                auto term1 = mass / Density[j] * spikyKernel(r);
                auto term2 = (Pressure[j] + Pressure[i]) / (2 * Density[j]);

                ax += term1 * (Velocity[j].x - Velocity[i].x) + term2 * (Position[j].x - Position[i].x);
                ay += term1 * (Velocity[j].y - Velocity[i].y) + term2 * (Position[j].y - Position[i].y);
            }
        }

        Velocity[i].x += static_cast<float>(ax);
        Velocity[i].y += static_cast<float>(ay);
        Position[i].x += static_cast<float>(Velocity[i].x * 0.00001);
        Position[i].y += static_cast<float>(Velocity[i].y * 0.00001);
    }
}

void Update()
{
    calculateDensityPressure();
    updateParticles();
    for (size_t i = 0; i < ParticleCount; ++i)
    {
        Position[i].x += 0.01f;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ParticleCount * 2, Position.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int main()
{
    InitOpenGL initOpenGL("SPH", 800, 600);
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("shaders/02_08_08_TEST2.vert", "shaders/02_08_08_TEST2.frag");

    Init();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ParticleCount * 2, Position.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);

    glBindVertexArray(0);

    glPointSize(20.f);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Update();

        program.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, ParticleCount);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <common.hpp>
#include <numbers>
#include <random>

GLuint VAO { 0 }, VBO { 0 };

struct Particle
{
    glm::vec2 position;
    glm::vec2 velocity;
    float density;
    float pressure;
};

#define M_PI 3.1415926f

const int numParticles      = 200;
const float boxSize         = 1.0f;
const float smoothingLength = .5f; // Smoothing length for SPH kernel

std::vector<Particle> particles;
std::vector<glm::vec2> Position;

// Function to initialize particles with random positions and velocities
void initializeParticles()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDist(-1.0f, 1.0f);

    for (int i = 0; i < numParticles; ++i)
    {
        Particle particle;
        particle.position = glm::vec2(posDist(gen), posDist(gen));
        particle.velocity = glm::vec2(0.0f, 0.0f);
        particle.density  = 0.0f;
        particle.pressure = 0.1f;

        particles.push_back(particle);
    }
}

// SPH kernel function for density estimation (Poly6 kernel)
float poly6Kernel(float r, float h)
{
    if (r >= 0.0f && r <= h)
    {
        float term = (h * h - r * r);
        return 315.0f / (64.0f * M_PI * std::pow(h, 9.0f)) * std::pow(term, 3.0f);
    }
    else
    {
        return 0.0f;
    }
}

// Function to update particles using SPH
void updateParticles(float dt)
{
    const float stiffness   = 1000.0f; // Gas stiffness
    const float restDensity = 1000.0f; // Rest density
    const float mass        = 1.0f;    // Particle mass

    // Reset particle densities
    for (auto& particle : particles)
    {
        particle.density = 0.0f;
    }

    // Calculate densities using SPH
    for (size_t i = 0; i < numParticles; ++i)
    {
        for (size_t j = 0; j < numParticles; ++j)
        {
            glm::vec2 r    = particles[i].position - particles[j].position;
            float distance = glm::length(r);

            particles[i].density += mass * poly6Kernel(distance, smoothingLength);
        }
    }

    // Update positions using SPH forces
    for (auto& particle : particles)
    {
        glm::vec2 force(0.0f, 0.0f);

        for (const auto& neighbor : particles)
        {
            glm::vec2 r    = particle.position - neighbor.position;
            float distance = glm::length(r);
            if (distance <= 0.000001f && distance >= -0.0000001f)
                continue;

            float term1 = mass / neighbor.density;
            float term2 = (particle.pressure + neighbor.pressure) / (2 * neighbor.density);

            force += poly6Kernel(distance, smoothingLength) * (r / distance);
        }

        // float borderRange     = 1.f;
        // float borderStiffness = .01f;
        // force.x += borderStiffness * -particle.position.x;
        // force.y += borderStiffness * -particle.position.y;

        // Update velocity and position
        particle.velocity += force * dt;
        particle.position += particle.velocity * dt;

        // Handle boundary conditions (reflect particles if they go out of bounds)
        if (particle.position.x < -boxSize || particle.position.x > boxSize)
        {
            particle.velocity.x *= -.8f;
            particle.position.x = glm::clamp(particle.position.x, -boxSize, boxSize);
        }

        if (particle.position.y < -boxSize || particle.position.y > boxSize)
        {
            particle.velocity.y *= -.8f;
            particle.position.y = glm::clamp(particle.position.y, -boxSize, boxSize);
        }
    }
}

void Init()
{
    initializeParticles();

    Position.resize(numParticles);

    for (size_t i = 0; i < numParticles; ++i)
    {
        Position[i].x = particles[i].position.x;
        Position[i].y = particles[i].position.y;
    }
}

void Update()
{
    float dt = .001f;
    updateParticles(dt);

    for (size_t i = 0; i < numParticles; ++i)
    {
        Position[i].x = particles[i].position.x;
        Position[i].y = particles[i].position.y;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numParticles * 2, Position.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int main()
{
    InitOpenGL initOpenGL("SPH", 800, 600);
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("shaders/02_08_08_TEST2.vert", "shaders/02_08_08_TEST2.frag");

    Init();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numParticles * 2, Position.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);

    glBindVertexArray(0);

    glPointSize(10.f);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Update();

        program.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, numParticles);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <algorithm>
#include <common.hpp>
#include <numbers>
#include <random>

GLuint VAO { 0 }, VBO { 0 };

struct Particle
{
    float position;
    float velocity;
    float density;
    float pressure;
};

const int numParticles    = 20;
const float leftBoundary  = -1.f;
const float rightBoundary = 1.f;

std::vector<Particle> particles;
std::vector<glm::vec2> Position;

void initializeParticles()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDist(-1.f, 1.f);

    for (int i = 0; i < numParticles; ++i)
    {
        Particle particle;
        particle.position = posDist(gen);
        particle.velocity = 0.f;
        particle.density  = 0.f;
        particle.pressure = 0.f;

        particles.emplace_back(particle);
    }
}

float poly6Kernel(float r, float h)
{
    if (r >= 0.f && r <= h)
    {
        float term = (h * h - r * r);
        return 315.f / (64.f * std::numbers::pi_v<float> * std::pow(h, 9.f)) * std::pow(term, 3.f);
    }
    else
    {
        return 0.f;
    }
}

void updateParticles(float dt)
{
    std::default_random_engine engine {};
    std::uniform_int_distribution<int> distribution(-1, 1);

    float velocityFactory = 0.005f;
    for (size_t i = 0; i < numParticles; ++i)
    {
        particles[i].velocity = 0.f;
        for (size_t j = 0; j < numParticles; ++j)
        {
            // 同一个粒子不计算作用力
            if (i == j)
            {
                continue;
            }
            // 距离大于.5的粒子之间没有作用力
            if (std::abs(particles[i].position - particles[j].position) > .5f)
            {
                continue;
            }
            // 如果两个粒子之间的距离非常小（位置重叠）随机生成一个速度方向
            if (std::abs(particles[i].position - particles[j].position) < 0.00001f)
            {
                particles[i].velocity += static_cast<float>(distribution(engine));
                continue;
            }

            auto dir = std::signbit(particles[i].position - particles[j].position) ? -1.f : 1.f;
            particles[i].velocity += velocityFactory * dir * poly6Kernel(std::abs(particles[i].position - particles[j].position), .5f);
        }
    }

    for (auto& particle : particles)
    {
        particle.position += dt * particle.velocity;

        if (particle.position < leftBoundary)
        {
            particle.position -= leftBoundary;
            particle.velocity = -particle.velocity;
        }

        if (particle.position > rightBoundary)
        {
            particle.position -= rightBoundary;
            particle.velocity = -particle.velocity;
        }
    }
}

void Init()
{
    initializeParticles();

    Position.resize(numParticles);

    for (size_t i = 0; i < numParticles; ++i)
    {
        Position[i].x = particles[i].position;
        Position[i].y = 0.f;
    }
}

void Update()
{
    float dt = .01f;
    updateParticles(dt);

    for (size_t i = 0; i < numParticles; ++i)
    {
        Position[i].x = particles[i].position;
        Position[i].y = 0.f;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numParticles * 2, Position.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int main()
{
    InitOpenGL initOpenGL("SPH", 800, 600);
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("shaders/02_08_08_TEST2.vert", "shaders/02_08_08_TEST2.frag");

    Init();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numParticles * 2, Position.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);

    glBindVertexArray(0);

    glPointSize(10.f);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Update();

        program.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, numParticles);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST4

#ifdef TEST5

#include <algorithm>
#include <common.hpp>
#include <numbers>
#include <random>

GLuint VAO { 0 }, VBO { 0 };

struct Particle
{
    glm::vec2 position;
    float property;
    glm::vec2 velocity;
    float density;
};

constexpr int numParticles { 500 };
constexpr float smoothingRadius { 1.f };
constexpr float mass { 1.f };
constexpr float targetDensity { 5.75f };
constexpr float pressureMultiplier { 5.f };
constexpr float gravity { 0.f };
constexpr float collisionDamping { 1.f };

const float leftBoundary  = -1.f;
const float rightBoundary = 1.f;

std::vector<Particle> particles;
std::vector<glm::vec2> Position;

float SmoothingKernel(float dst, float radius)
{
    // float volume = std::numbers::pi_v<float> * std::pow(radius, 8.f) / 4.f;
    // float value  = std::max(0.f, radius * radius - dst * dst);
    // return value * value * value / volume;

    if (dst >= radius)
    {
        return 0.f;
    }

    float volume = (std::numbers::pi_v<float> * std::pow(radius, 4.f)) / 6.f;
    return (radius - dst) * (radius - dst) / volume;
}

float SmoothingKernelDerivative(float dst, float radius)
{
    if (dst >= radius)
    {
        return 0.f;
    }

    // float f     = radius * radius - dst * dst;
    // float scale = -24.f / (std::numbers::pi_v<float> * std::pow(radius, 8.f));

    // return scale * dst * f * f;

    float scale = 12.f / (std::pow(radius, 4.f) * std::numbers::pi_v<float>);
    return (dst - radius) * scale;
}

float ConvertDensityToPressure(float density)
{
    float densityError = density - targetDensity;
    float pressure     = densityError * pressureMultiplier;

    return pressure;
}

float ExampleFunction(const glm::vec2& pos)
{
    return std::cos(pos.y - 3.f + sin(pos.x));
}

float CalculateDensity(const glm::vec2& samplePoint)
{
    float density { 0.f };

    for (const auto& particle : particles)
    {
        float dst       = glm::length(particle.position - samplePoint);
        float influence = SmoothingKernel(dst, smoothingRadius);
        density += mass * influence;
    }

    return density;
}

float CalculateProperty(const glm::vec2& samplePoint)
{
    float property { 0.f };

    for (size_t i = 0; i < numParticles; ++i)
    {
        float dst       = glm::length(particles[i].position - samplePoint);
        float influence = SmoothingKernel(dst, smoothingRadius);
        float density   = CalculateDensity(particles[i].position);

        property += particles[i].property * mass / density * influence;
    }

    return property;
}

glm::vec2 CalculatePressureForce(size_t particleIndex)
{
    // const float stepSize { 0.001f };
    // float deltaX = CalculateProperty(samplePoint + glm::vec2(1.f, 0.f) * stepSize) - CalculateProperty(samplePoint);
    // float deltaY = CalculateProperty(samplePoint + glm::vec2(0.f, 1.f) * stepSize) - CalculateProperty(samplePoint);

    // return glm::vec2(deltaX, deltaY) / stepSize;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> randomDir(-1.0f, 1.0f);

    glm::vec2 pressureForce = glm::vec2(0.f);

    for (size_t i = 0; i < numParticles; ++i)
    {
        if (i == particleIndex)
        {
            continue;
        }

        glm::vec2 offset = particles[particleIndex].position - particles[i].position;

        float dst = glm::length(offset);

        glm::vec2 dir(0.f);
        if (dst == 0.f)
        {
            dir = glm::normalize(glm::vec2(randomDir(gen), randomDir(gen)));
        }
        else
        {
            dir = offset / dst;
        }

        float slope = SmoothingKernelDerivative(dst, smoothingRadius);
        // float density = CalculateDensity(particles[i].position);

        // pressureForce += -particles[i].property * dir * slope * mass / density;
        pressureForce += -ConvertDensityToPressure(particles[i].density) * dir * slope * mass / particles[i].density;
    }

    return pressureForce;
}

void ResolveCollision(size_t particleIndex)
{
    // particles[particleIndex].position.x = std::clamp(particles[particleIndex].position.x, leftBoundary, rightBoundary);
    // particles[particleIndex].position.y = std::clamp(particles[particleIndex].position.y, leftBoundary, rightBoundary);

    if (particles[particleIndex].position.x > rightBoundary)
    {
        particles[particleIndex].position.x = particles[particleIndex].position.x - rightBoundary;
        particles[particleIndex].velocity.x *= (-1.f * collisionDamping);
    }
    else if (particles[particleIndex].position.x < leftBoundary)
    {
        particles[particleIndex].position.x = particles[particleIndex].position.x - leftBoundary;
        particles[particleIndex].velocity.x *= (-1.f * collisionDamping);
    }
    if (particles[particleIndex].position.y > rightBoundary)
    {
        particles[particleIndex].position.y = particles[particleIndex].position.y - rightBoundary;
        particles[particleIndex].velocity.y *= (-1.f * collisionDamping);
    }
    else if (particles[particleIndex].position.y < leftBoundary)
    {
        particles[particleIndex].position.y = particles[particleIndex].position.y - leftBoundary;
        particles[particleIndex].velocity.y *= (-1.f * collisionDamping);
    }

    // if (std::abs(particles[particleIndex].position.x) >= rightBoundary)
    // {
    //     particles[particleIndex].position.x = std::clamp(particles[particleIndex].position.x, leftBoundary, rightBoundary);
    //     particles[particleIndex].velocity.x *= -1.f * collisionDamping;
    // }

    // if (std::abs(particles[particleIndex].position.y) >= rightBoundary)
    // {
    //     particles[particleIndex].position.y = std::clamp(particles[particleIndex].position.y, leftBoundary, rightBoundary);
    //     particles[particleIndex].velocity.y *= -1.f * collisionDamping;
    // }
}

void SimulationStep(float deltaTime)
{
    for (size_t i = 0; i < numParticles; ++i)
    {
        particles[i].velocity += glm::vec2(0.f, -1.f) * gravity * deltaTime;
        particles[i].density = CalculateDensity(particles[i].position);
    }

    for (size_t i = 0; i < numParticles; ++i)
    {
        glm::vec2 pressureForce        = CalculatePressureForce(i);
        glm::vec2 pressureAcceleration = pressureForce / particles[i].density;
        particles[i].velocity          = pressureAcceleration * deltaTime;
    }

    for (size_t i = 0; i < numParticles; ++i)
    {
        particles[i].position += particles[i].velocity * deltaTime;
        ResolveCollision(i);
    }
}

void UpdateParticles(float dt)
{
    SimulationStep(dt);
}

void InitializeParticles()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDist(-0.9f, .9f);

    for (int i = 0; i < numParticles; ++i)
    {
        Particle particle;
        particle.position = { posDist(gen), posDist(gen) };
        particle.property = ExampleFunction(particle.position);
        particle.velocity = glm::vec2(0.f);
        particle.density  = 0.f;

        particles.emplace_back(particle);
    }
}

void Init()
{
    InitializeParticles();

    Position.resize(numParticles);

    for (size_t i = 0; i < numParticles; ++i)
    {
        Position[i] = particles[i].position;
    }
}

void Update()
{
    float dt = .01f;
    UpdateParticles(dt);

    for (size_t i = 0; i < numParticles; ++i)
    {
        Position[i] = particles[i].position;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numParticles * 2, Position.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int main()
{
    InitOpenGL initOpenGL("SPH", 800, 600);
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("shaders/02_08_08_TEST2.vert", "shaders/02_08_08_TEST2.frag");

    Init();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numParticles * 2, Position.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);

    glBindVertexArray(0);

    glPointSize(10.f);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Update();

        program.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, numParticles);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST5

#ifdef TEST6

#include <algorithm>
#include <common.hpp>
#include <numbers>
#include <random>

GLuint VAO { 0 }, VBO { 0 };

std::vector<glm::vec2> Positions;
std::vector<glm::vec2> Velocities;
std::vector<float> Densities;
std::vector<glm::vec2> PredictedPositions;

constexpr size_t RowParticles { 20 };
constexpr size_t ColParticles { 20 };
constexpr size_t NumParticles { RowParticles * ColParticles };

constexpr float BoundsMin { -1.f };
constexpr float BoundsMax { 1.f };
constexpr float Gravity { 0.f };
constexpr float CollisionDamping { .95f }; // 弹力系数
constexpr float SmoothingRadius { .1f };
constexpr float Mass { 1.f };
constexpr float TargetDensity { .5f };       // 目标密度
constexpr float PressureMultiplier { .05f }; // 压力倍增器

float SmoothingKernel(float dst, float radius)
{
    if (dst >= radius)
    {
        return 0.f;
    }

    float volume = (std::numbers::pi_v<float> * std::pow(radius, 4.f)) / 6.f;
    return (radius - dst) * (radius - dst) / volume;
}

float SmoothingKernelDerivative(float dst, float radius)
{
    if (dst >= radius)
    {
        return 0.f;
    }

    float scale = 12.f / (std::pow(radius, 4.f) * std::numbers::pi_v<float>);
    return (dst - radius) * scale;
}

float ConvertDensityToPressure(float density)
{
    float densityError = density - TargetDensity;
    float pressure     = densityError * PressureMultiplier;

    return pressure;
}

float CalculateDensity(const glm::vec2& samplePoint)
{
    float density { 0.f };

    for (const auto& position : Positions)
    {
        float dst       = glm::length(position - samplePoint);
        float influence = SmoothingKernel(dst, SmoothingRadius);
        density += Mass * influence;
    }

    return density;
}

float CalculateSharedPressure(float densityA, float densityB)
{
    float pressureA = ConvertDensityToPressure(densityA);
    float pressureB = ConvertDensityToPressure(densityB);

    return (pressureA + pressureB) / 2.f;
}

glm::vec2 CalculatePressureForce(size_t index)
{
    static std::random_device rd;
    static std::mt19937 engine(rd());
    static std::uniform_real_distribution<float> randomDir(-1.0f, 1.0f);

    glm::vec2 pressureForce = glm::vec2(0.f);

    for (size_t i = 0; i < NumParticles; ++i)
    {
        if (i == index)
        {
            continue;
        }

        auto offset         = Positions[i] - Positions[index];
        auto dst            = glm::length(offset);
        auto dir            = dst == 0.f ? glm::normalize(glm::vec2(randomDir(engine), randomDir(engine))) : offset / dst;
        auto slope          = SmoothingKernelDerivative(dst, SmoothingRadius);
        auto density        = Densities[i];
        auto sharedPressure = CalculateSharedPressure(density, Densities[index]);
        pressureForce += sharedPressure * dir * slope * Mass / density;
    }

    return pressureForce;
}

void ResolveCollisions(size_t index)
{
    if (std::abs(Positions[index].x) > BoundsMax)
    {
        Positions[index].x = std::clamp(Positions[index].x, BoundsMin, BoundsMax);
        Velocities[index].x *= -1.f * CollisionDamping;
    }

    if (std::abs(Positions[index].y) > BoundsMax)
    {
        Positions[index].y = std::clamp(Positions[index].y, BoundsMin, BoundsMax);
        Velocities[index].y *= -1.f * CollisionDamping;
    }
}

void UpdateParticles(float deltaTime)
{
    for (size_t i = 0; i < NumParticles; ++i)
    {
        Velocities[i] += glm::vec2(0.f, -1.f) * Gravity * deltaTime;
        PredictedPositions[i] = Positions[i] + Velocities[i] * deltaTime;
    }

    for (size_t i = 0; i < NumParticles; ++i)
    {
        Densities[i] = CalculateDensity(PredictedPositions[i]);
    }

    for (size_t i = 0; i < NumParticles; ++i)
    {
        auto pressureForce        = CalculatePressureForce(i);
        auto pressureAcceleration = pressureForce / Densities[i];
        Velocities[i] += pressureAcceleration * deltaTime;
    }

    for (size_t i = 0; i < NumParticles; ++i)
    {
        Positions[i] += Velocities[i] * deltaTime;
        ResolveCollisions(i);
    }
}

void InitializeParticles()
{
    static std::random_device rd;
    static std::mt19937 engine(rd());
    static std::uniform_real_distribution<float> randomPosition(-.5f, .5f);

    Positions.resize(NumParticles);
    Velocities.resize(NumParticles);
    Densities.resize(NumParticles);
    PredictedPositions.resize(NumParticles);
    for (size_t i = 0; i < RowParticles; ++i)
    {
        for (size_t j = 0; j < ColParticles; ++j)
        {
            Positions[i * RowParticles + j] = { -.8f + 1.6f / static_cast<float>(RowParticles) * static_cast<float>(i),
                -.8f + 1.6f / static_cast<float>(ColParticles) * static_cast<float>(j) };
            // Positions[i * RowParticles + j]  = { randomPosition(engine),randomPosition(engine) };
            Velocities[i * RowParticles + j] = { 0.f, 0.f };
            Densities[i * RowParticles + j]  = 0.f;
        }
    }
}

void Update()
{
    float dt = .01f;
    UpdateParticles(dt);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * NumParticles * 2, Positions.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int main()
{
    InitOpenGL initOpenGL("SPH", 800, 600);
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("shaders/02_08_08_TEST2.vert", "shaders/02_08_08_TEST2.frag");

    InitializeParticles();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * NumParticles * 2, Positions.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);

    glBindVertexArray(0);

    glPointSize(10.f);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Update();

        program.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, NumParticles);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST6

#ifdef TEST7

#include <algorithm>
#include <common.hpp>
#include <execution>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <numbers>
#include <random>

GLuint VAO { 0 }, VBO { 0 };

std::vector<glm::vec2> Positions;
std::vector<glm::vec2> Velocities;
std::vector<float> Densities;
std::vector<glm::vec2> PredictedPositions;
std::vector<size_t> Indices;

constexpr size_t RowParticles { 30 };
constexpr size_t ColParticles { 30 };
constexpr size_t NumParticles { RowParticles * ColParticles };

glm::vec2 BoundsSize { 16.f, 9.f };
constexpr float Gravity { 0.f };
constexpr float CollisionDamping { .9f }; // 弹力系数
constexpr float Mass { 1.f };

float SmoothingRadius { 1.5f };
float TargetDensity { 2.5f };     // 目标密度
float PressureMultiplier { .5f }; // 压力倍增器

float SmoothingKernel(float dst, float radius)
{
    if (dst >= radius)
    {
        return 0.f;
    }

    float volume = (std::numbers::pi_v<float> * std::pow(radius, 4.f)) / 6.f;
    return (radius - dst) * (radius - dst) / volume;
}

float SmoothingKernelDerivative(float dst, float radius)
{
    if (dst >= radius)
    {
        return 0.f;
    }

    float scale = 12.f / (std::pow(radius, 4.f) * std::numbers::pi_v<float>);
    return (dst - radius) * scale;
}

float ConvertDensityToPressure(float density)
{
    float densityError = density - TargetDensity;
    float pressure     = densityError * PressureMultiplier;

    return pressure;
}

float CalculateDensity(const glm::vec2& samplePoint)
{
    float density { 0.f };

    for (const auto& position : Positions)
    {
        float dst       = glm::length(position - samplePoint);
        float influence = SmoothingKernel(dst, SmoothingRadius);
        density += Mass * influence;
    }

    return density;
}

float CalculateSharedPressure(float densityA, float densityB)
{
    float pressureA = ConvertDensityToPressure(densityA);
    float pressureB = ConvertDensityToPressure(densityB);

    return (pressureA + pressureB) / 2.f;
}

glm::vec2 CalculatePressureForce(size_t index)
{
    static std::random_device rd;
    static std::mt19937 engine(rd());
    static std::uniform_real_distribution<float> randomDir(-1.0f, 1.0f);

    glm::vec2 pressureForce = glm::vec2(0.f);

    for (size_t i = 0; i < NumParticles; ++i)
    {
        if (i == index)
        {
            continue;
        }

        auto offset         = Positions[i] - Positions[index];
        auto dst            = glm::length(offset);
        auto dir            = dst == 0.f ? glm::normalize(glm::vec2(randomDir(engine), randomDir(engine))) : offset / dst;
        auto slope          = SmoothingKernelDerivative(dst, SmoothingRadius);
        auto sharedPressure = CalculateSharedPressure(Densities[i], Densities[index]);
        pressureForce += sharedPressure * dir * slope * Mass / Densities[i];
    }

    return pressureForce;
}

void ResolveCollisions(size_t index)
{
    glm::vec2 halfBoundsSize = BoundsSize / 2.f;
    if (std::abs(Positions[index].x) > halfBoundsSize.x)
    {
        Positions[index].x = std::clamp(Positions[index].x, -halfBoundsSize.x, halfBoundsSize.x);
        Velocities[index].x *= -1.f * CollisionDamping;
    }

    if (std::abs(Positions[index].y) > halfBoundsSize.y)
    {
        Positions[index].y = std::clamp(Positions[index].y, -halfBoundsSize.y, halfBoundsSize.y);
        Velocities[index].y *= -1.f * CollisionDamping;
    }
}

void UpdateParticles(float deltaTime)
{
#if 1
    std::for_each(std::execution::par, Indices.cbegin(), Indices.cend(),
        [deltaTime](auto index) { Velocities[index] += glm::vec2(0.f, -1.f) * Gravity * deltaTime; });

    std::for_each(std::execution::par, Indices.cbegin(), Indices.cend(),
        [deltaTime](auto index) { PredictedPositions[index] = Positions[index] + Velocities[index] * deltaTime; });

    std::for_each(std::execution::par, Indices.cbegin(), Indices.cend(),
        [deltaTime](auto index) { Densities[index] = CalculateDensity(PredictedPositions[index]); });

    std::for_each(std::execution::par, Indices.cbegin(), Indices.cend(),
        [deltaTime](auto index)
        {
            auto pressureForce        = CalculatePressureForce(index);
            auto pressureAcceleration = pressureForce / Densities[index];
            Velocities[index] += pressureAcceleration * deltaTime;
        });

    std::for_each(std::execution::par, Indices.cbegin(), Indices.cend(),
        [deltaTime](auto index)
        {
            Positions[index] += Velocities[index] * deltaTime;
            ResolveCollisions(index);
        });

    // auto tmpDensities = Densities;
    // std::sort(std::execution::par, tmpDensities.begin(), tmpDensities.end());

#else
    for (size_t i = 0; i < NumParticles; ++i)
    {
        Velocities[i] += glm::vec2(0.f, -1.f) * Gravity * deltaTime;
        PredictedPositions[i] = Positions[i] + Velocities[i] * deltaTime;
    }

    for (size_t i = 0; i < NumParticles; ++i)
    {
        Densities[i] = CalculateDensity(PredictedPositions[i]);
    }

    for (size_t i = 0; i < NumParticles; ++i)
    {
        auto pressureForce        = CalculatePressureForce(i);
        auto pressureAcceleration = pressureForce / Densities[i];
        Velocities[i] += pressureAcceleration * deltaTime;
    }

    for (size_t i = 0; i < NumParticles; ++i)
    {
        Positions[i] += Velocities[i] * deltaTime;
        ResolveCollisions(i);
    }
#endif
}

void InitializeParticles()
{
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<float> randomPosition(-.5f, .5f);

    glm::vec2 halfBoundsSize = BoundsSize / 4.f;
    glm::vec2 startPosition  = -halfBoundsSize;
    glm::vec2 endPosition    = halfBoundsSize;

    Positions.resize(NumParticles);
    Velocities.resize(NumParticles);
    Densities.resize(NumParticles);
    PredictedPositions.resize(NumParticles);
    for (size_t i = 0; i < RowParticles; ++i)
    {
        for (size_t j = 0; j < ColParticles; ++j)
        {
            Positions[i * RowParticles + j]
                = { startPosition.x + (endPosition.x - startPosition.x) / static_cast<float>(RowParticles) * static_cast<float>(i),
                      startPosition.y + (endPosition.y - startPosition.y) / static_cast<float>(ColParticles) * static_cast<float>(j) };
            // Positions[i * RowParticles + j]  = { randomPosition(engine),randomPosition(engine) };
            Velocities[i * RowParticles + j] = { 0.f, 0.f };
            Densities[i * RowParticles + j]  = 0.f;
        }
    }

    Indices.resize(NumParticles);
    std::iota(Indices.begin(), Indices.end(), 0);
}

void Update()
{
    float dt = .01f;
    UpdateParticles(dt);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * NumParticles * 2, Positions.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int main()
{
    InitOpenGL initOpenGL("SPH", 800, 450);
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("shaders/02_08_08_TEST2.vert", "shaders/02_08_08_TEST2.frag");

    //---------------------------------------------------------------------------------
    // 初始化Dear ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui::StyleColorsDark();

    InitializeParticles();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * NumParticles * 2, Positions.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);

    glBindVertexArray(0);

    glPointSize(10.f);
    double lastTime { 0. };
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Update();

        auto currentTime = glfwGetTime();
        // std::cout << "FPS: " << static_cast<uint32_t>(1. / (currentTime - lastTime)) << '\n';
        lastTime = currentTime;

        program.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, NumParticles);
        glBindVertexArray(0);

        //---------------------------------------------------------------------------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("SPH parameter");
            ImGui::SliderFloat("Pressure multiplier", &PressureMultiplier, 0.00001f, 10.f);
            ImGui::SliderFloat("Target density", &TargetDensity, 0.00001f, 10.f);
            ImGui::SliderFloat("Smoothing radius", &SmoothingRadius, 0.00001f, 10.f);
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST7
