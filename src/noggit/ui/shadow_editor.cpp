// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/ui/shadow_editor.hpp>
#include <noggit/ui/slider_spinbox.hpp>
#include <noggit/World.h>


#include <QFormLayout>
#include <QGroupBox>
#include <QKeyEvent>


namespace noggit::ui
{
  shadow_editor::shadow_editor(QWidget* parent)
    : noggit_tool(parent)
    , _radius_property(10.f)
    , _pitch_property(47.f)
    , _threshold_property(128.f)
  {
    auto layout(new QFormLayout(this));

    QGroupBox* brush_group = new QGroupBox("Brush", this);
    auto brush_layout(new QFormLayout(brush_group));
    brush_layout->addWidget(new slider_spinbox("Radius", &_radius_property, 0.f, 100.f, 2, this));

    QGroupBox* generation_group = new QGroupBox("Auto Generation", this);
    auto generation_layout(new QFormLayout(generation_group));
    generation_layout->addWidget(new slider_spinbox("Sun Pitch", &_pitch_property, 1.f, 90.f, 0, this));
    generation_layout->addWidget(new slider_spinbox("Shadow Threshold", &_threshold_property, 0.f, 256.f, 0, this));

    layout->addRow(brush_group);
    layout->addRow(generation_group);
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

    if (!cursor_under_map && _middle_mouse_button)
    {
      world->update_legacy_shadow_for_tile_at(cursor_pos, _pitch_property.get(), static_cast<int>(std::round(_threshold_property.get())));
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
