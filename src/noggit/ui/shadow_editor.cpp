// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/ui/shadow_editor.hpp>
#include <noggit/ui/slider_spinbox.hpp>
#include <noggit/World.h>


#include <QtWidgets/QFormLayout>
#include <QKeyEvent>


namespace noggit::ui
{
  shadow_editor::shadow_editor(QWidget* parent)
    : noggit_tool(parent)
    , _radius_property(10.f)
  {
    auto layout(new QFormLayout(this));

    layout->addWidget(new slider_spinbox("Radius", &_radius_property, 0.f, 100.f, 2, this));
  }


  void shadow_editor::tick(float /* dt */, math::vector_3d const& cursor_pos, bool cursor_under_map, World* world)
  {
    if (!cursor_under_map && _left_mouse_button)
    {
      if (_mod_shift_down)
      {
        world->set_shadow(cursor_pos, _radius_property.get(), true);
      }
      if (_mod_ctrl_down)
      {
        world->set_shadow(cursor_pos, _radius_property.get(), false);
      }
    }
  }

  void shadow_editor::mouse_move_event(QLineF const& relative_movement)
  {
    if (_left_mouse_button && _mod_alt_down)
    {
      change_radius(relative_movement.dx() / mouse_sensibility);
    }
  }
}
