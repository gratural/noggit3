// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/ui/noggit_tool.hpp>

#include <QKeyEvent>

namespace noggit::ui
{
  void noggit_tool::mouse_press_event(QMouseEvent* event)
  {
    switch (event->button())
    {
    case Qt::LeftButton:   _left_mouse_button   = true; break;
    case Qt::MiddleButton: _middle_mouse_button = true; break;
    case Qt::RightButton:  _right_mouse_button  = true; break;
    }
  }
  void noggit_tool::mouse_release_event(QMouseEvent* event)
  {
    switch (event->button())
    {
    case Qt::LeftButton:   _left_mouse_button   = false; break;
    case Qt::MiddleButton: _middle_mouse_button = false; break;
    case Qt::RightButton:  _right_mouse_button  = false; break;
    }
  }

  void noggit_tool::key_release_event(QKeyEvent* event)
  {
    auto key = event->key();

    switch (key)
    {
    case Qt::Key_Alt:     _mod_alt_down   = false; break;
    case Qt::Key_Control: _mod_ctrl_down  = false; break;
    case Qt::Key_Shift:   _mod_shift_down = false; break;
    case Qt::Key_Space:   _mod_space_down = false; break;
    }
  }
  void noggit_tool::key_press_event(QKeyEvent* event)
  {
    auto key = event->key();

    switch (key)
    {
    case Qt::Key_Alt:     _mod_alt_down   = true; break;
    case Qt::Key_Control: _mod_ctrl_down  = true; break;
    case Qt::Key_Shift:   _mod_shift_down = true; break;
    case Qt::Key_Space:   _mod_space_down = true; break;
    }
  }

  void noggit_tool::reset_input_states()
  {
    _mod_alt_down   = false;
    _mod_ctrl_down  = false;
    _mod_shift_down = false;
    _mod_space_down = false;

    _left_mouse_button   = false;
    _middle_mouse_button = false;
    _right_mouse_button  = false;

    reset_extra_states();
  }

  float noggit_tool::scroll_wheel_delta_for_range(QWheelEvent* event, float range) const
  {
    return ( _mod_ctrl_down ? 0.01f : 0.1f)
           * range
           // alt = horizontal delta
           * (_mod_alt_down ? event->angleDelta().x() : event->angleDelta().y())
           / 320.f
           ;
  }
}
