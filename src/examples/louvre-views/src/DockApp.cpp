#include <LPointerButtonEvent.h>
#include "DockApp.h"
#include "App.h"
#include "Dock.h"
#include "Global.h"
#include "Tooltip.h"

DockApp::DockApp(App *app, Dock *dock) : LTextureView()
{
    this->app = app;
    this->dock = dock;
    setParent(dock->appsContainer);
    setTexture(app->texture);
    setBufferScale(2);
    enableInput(true);
    enableBlockPointer(false);

    dot = new LTextureView(G::dockTextures().dot, this);
    dot->setBufferScale(2);
    dot->setPos((size().w() - dot->size().w()) / 2, size().h() - 2);
    dot->setVisible(false);
    app->dockApps.push_back(this);
    appLink = std::prev(app->dockApps.end());
    dock->update();
}

DockApp::~DockApp()
{
    if (app->launchAnimation)
        app->launchAnimation->stop();

    delete dot;
    dot = nullptr;
    setParent(nullptr);
    app->dockApps.erase(appLink);
    dock->update();
}

void DockApp::pointerEnterEvent(const LPointerMoveEvent &)
{
    G::tooltip()->setText(app->name);
    G::tooltip()->targetView = this;
    dock->update();
}

void DockApp::pointerButtonEvent(const LPointerButtonEvent &event)
{
    if (event.button() == LPointer::Left && event.state() == LPointer::Released)
        app->clicked();
}
