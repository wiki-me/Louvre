#ifndef TOPLEVELBUTTON_H
#define TOPLEVELBUTTON_H

#include <LTextureView.h>

class ToplevelView;

using namespace Louvre;

class ToplevelButton : public LTextureView
{
public:
    enum ButtonType
    {
        Close,
        Minimize,
        Maximize
    };

    ToplevelButton(LView *parent, ToplevelView *toplevelView, ButtonType type);

    void update();

    ToplevelView *toplevelView;
    ButtonType buttonType;
    bool pressed = false;

    void pointerButtonEvent(const LPointerButtonEvent &event) override;
    void pointerLeaveEvent(const LPointerMoveEvent &) override;
    void pointerMoveEvent(const LPointerMoveEvent &) override;
};

#endif // TOPLEVELBUTTON_H
