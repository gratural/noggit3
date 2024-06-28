// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <math/vector_3d.hpp>

#include <QWidget>

class World;

namespace noggit
{
  namespace ui
  {
    class noggit_tool : public QWidget
    {
    public:
      noggit_tool(QWidget* parent = nullptr) : QWidget(parent) {}

      virtual void tick(float dt, math::vector_3d const& cursor_pos, bool cursor_under_map, World* world) {}

      virtual void mouse_move_event(QLineF const& relative_movement) {}
      virtual void mouse_press_event(QMouseEvent* event);
      virtual void mouse_release_event(QMouseEvent* event);
      virtual void wheel_event(QWheelEvent* event) {}
      virtual void key_release_event(QKeyEvent* event);
      virtual void key_press_event(QKeyEvent* event);

      void reset_input_states();

      static constexpr float mouse_sensibility = 15.f;

    protected:
      //! \note 8.f for degrees, 40.f for smoothness
      float scroll_wheel_delta_for_range(QWheelEvent* event, float range) const;

      bool _mod_alt_down = false;
      bool _mod_ctrl_down = false;
      bool _mod_shift_down = false;
      bool _mod_space_down = false;

      bool _left_mouse_button = false;
      bool _middle_mouse_button = false;
      bool _right_mouse_button = false;
    };
  }
}
