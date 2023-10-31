#pragma once

#include "object.h"
#include <array>
#include <list>

class Actor;
class Window;
class Camera;

class Renderer : public Object
{
public:
    static Renderer* New();

    void AddActor(Actor*);
    void RemoveActor(Actor*);

    void SetWindow(Window*);
    void SetViewport(const std::array<double, 4>&);
    void SetBackground(const std::array<double, 3>&);
    void SetBackground(const std::array<double, 3>&, const std::array<double, 3>&);

    virtual void Render();
    virtual void RenderOpaque();
    virtual void RenderTranslucent();
    virtual void Clear();

    void ResetCamera() const;
    void SetCamera(Camera*);
    Camera* GetCamera() const noexcept;

private:
    bool HasActor(Actor*);

protected:
    Renderer();
    ~Renderer() override;

protected:
    std::list<Actor*> m_actors {};
    Camera* m_camera { nullptr };
    Window* m_window { nullptr };

    std::array<double, 4> m_viewport { 0., 0., 1., 1. };

    bool m_useGradientBack { false };
    std::array<double, 3> m_background {};
    std::array<double, 3> m_background2 {};

    bool m_useShadows { false };
    bool m_useDepthPeeling { false };
    bool m_usePickProps { false };
    bool m_useFXAA { false };
};