#pragma once

#include "Group.h"

class Camera;

class View : public Group
{
public:
    void Accept(Visitor& visitor) override;

    void Clear() const noexcept;

    void SetCamera(Camera* camera);
    const Camera* GetCamera() const noexcept;

private:
    Camera* m_camera {};
};
