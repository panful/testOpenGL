#include "actor.h"
#include "cells.h"
#include "geometry.h"
#include "interactor.h"
#include "interactorStyle.h"
#include "mapper.h"
#include "new.h"
#include "points.h"
#include "renderer.h"
#include "window.h"

int main()
{
    New<Points> points;
    points->Allocate(3);
    points->InsertNextPoint(-.5f, -.5f, 0.f);
    points->InsertNextPoint(.5f, -.5f, 0.f);
    points->InsertNextPoint(0.f, .5f, 0.f);
    points->InsertNextPoint(-.5f, .5f, 0.f);
    points->InsertNextPoint(.5f, .5f, 0.f);

    New<Cells> cells;
    cells->InsertNextCell({ 0, 1, 2 });
    cells->InsertNextCell({ 3,4 });

    New<Geometry> geom;
    geom->SetPoints(points);
    geom->SetCells(cells);

    New<Mapper> mapper;
    mapper->SetGeometry(geom);

    New<Actor> actor;
    actor->SetMapper(mapper);

    New<Renderer> renderer;
    renderer->AddActor(actor);

    New<Window> window;
    window->AddRenderer(renderer);

    New<InteractorStyle> style;
    New<Interactor> interactor;
    interactor->SetWindow(window);
    interactor->SetInteractorStyle(style);

    window->Render();
    interactor->Start();
}
