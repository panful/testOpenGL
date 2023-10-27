#pragma once

#include "object.h"
#include "smartpointer.h"
#include <list>

class Actor;
class Camera;

class Renderer : public Object
{
public:
    static Renderer* New();

    void AddActor(Actor*);
    void RemoveActor(Actor*);

    virtual void Render();
    void ResetCamera() const;
    void SetCamera(Camera*);
    Camera* GetCamera() const;

private:
    bool HasActor(Actor*);

protected:
    Renderer();
    ~Renderer() override;

protected:
    std::list<SmartPointer<Actor>> m_actors;
    Camera* m_camera { nullptr };

    bool m_useShadows { false };
    bool m_useDepthPeeling { false };
    bool m_usePickProps { false };
    bool m_useFXAA { false };
};