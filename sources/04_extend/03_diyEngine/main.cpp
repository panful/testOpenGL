#include "actor.h"
#include "cells.h"
#include "dataArray.h"
#include "geometry.h"
#include "interactor.h"
#include "interactorStyle.h"
#include "log.h"
#include "mapper.h"
#include "new.h"
#include "points.h"
#include "property.h"
#include "renderer.h"
#include "window.h"

int main()
{
    New<Points> points;
    points->Allocate(6);
    points->InsertNextPoint(-.5f, -.5f, 0.f);
    points->InsertNextPoint(.5f, -.5f, 0.f);
    points->InsertNextPoint(.5f, .5f, 0.f);
    points->InsertNextPoint(-.5f, .5f, 0.f);
    points->InsertNextPoint(-.8f, -.5f, 0.f);
    points->InsertNextPoint(-.8f, .5f, 0.f);

    New<Cells> cells;
    cells->InsertNextCell({ 0, 1, 2 });
    cells->InsertNextCell({ 0, 2, 3 });
    cells->InsertNextCell({ 4, 5 });

    New<DataArray> point_colors;
    point_colors->SetName("color");
    point_colors->SetNumberOfComponents(3);
    point_colors->InsertNextTuple3(1, 0, 0);
    point_colors->InsertNextTuple3(0, 1, 0);
    point_colors->InsertNextTuple3(0, 0, 1);
    point_colors->InsertNextTuple3(1, 1, 1);
    point_colors->InsertNextTuple3(0, 1, 0);
    point_colors->InsertNextTuple3(0, 1, 0);

    // 使用单元着色，必须按点、线、三角形的顺序插入颜色
    New<DataArray> cell_colors;
    cell_colors->SetName("color");
    cell_colors->SetNumberOfComponents(3);
    cell_colors->InsertNextTuple3(1, 1, 0);
    cell_colors->InsertNextTuple3(0, 1, 1);
    cell_colors->InsertNextTuple3(0, 0, 1);

    New<Geometry> geom;
    geom->SetPoints(points);
    geom->SetCells(cells);
    geom->AddPointArray(point_colors);
    geom->AddCellArray(cell_colors);

    New<Mapper> mapper;
    mapper->SetGeometry(geom);
    mapper->SetColorVisibility(true);
    // mapper->SetColorVisibility(false);
    mapper->SetColorModeToCellData();
    // mapper->SetColorModeToPointData();
    mapper->SetColorArray("color");

    New<Actor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor({ 1., 1., 0. });

    New<Renderer> renderer;
    renderer->AddActor(actor);
    // renderer->SetViewport({ 0.1, 0.1, 0.45, 0.45 });
    renderer->SetBackground({ .1, .2, .3 });

    // New<Renderer> renderer2;
    // renderer2->AddActor(actor);
    // renderer2->SetViewport({ 0.55, 0.55, 0.9, 0.9 });
    // renderer2->SetBackground({ .3, .2, .1 });

    New<Window> window;
    window->AddRenderer(renderer);
    // window->AddRenderer(renderer2);

    New<InteractorStyle> style;
    New<Interactor> interactor;
    interactor->SetWindow(window);
    interactor->SetInteractorStyle(style);

    window->Render();
    interactor->Start();
}
