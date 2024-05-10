#pragma once

#include "Visitor.h"
#include <list>

class Camera;
class View;

class Trackball : public Visitor
{
public:
    void Apply(Event& event) override;

    void Apply(KeyEvent& keyEvent) override;
    void Apply(KeyPressEvent& keyPressEvent) override;
    void Apply(KeyReleaseEvent& keyReleaseEvent) override;

    void Apply(MouseEvent& mouseEvent) override;
    void Apply(MousePressEvent& mousePressEvent) override;
    void Apply(MouseReleaseEvent& mouseReleaseEvent) override;
    void Apply(MouseMoveEvent& mouseMoveEvent) override;

    void AddView(View* view);

private:
    std::list<View*> m_views {};
    View* m_pokedView {};
};
