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

      virtual void tick([[maybe_unused]] float dt,
                        [[maybe_unused]] math::vector_3d const& cursor_pos,
                        [[maybe_unused]] bool cursor_under_map,
                        World*) {}

      virtual void mouse_move_event([[maybe_unused]] QLineF const& relative_movement) {}
      virtual void mouse_press_event(QMouseEvent*);
      virtual void mouse_release_event(QMouseEvent*);
      virtual void wheel_event(QWheelEvent*) {}
      virtual void key_release_event(QKeyEvent*);
      virtual void key_press_event(QKeyEvent*);

      void reset_input_states();
      // for tool specific states
      // called by reset_input_states
      virtual void reset_extra_states() {}

      static constexpr float mouse_sensibility = 15.f;

      void set_window_size(int width, int height) { _window_width = width; _window_height = height; }
      float aspect_ratio() const { return static_cast<float>(_window_width) / static_cast<float>(_window_height); }

    protected:
      int _window_width = 1;
      int _window_height = 1;

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
