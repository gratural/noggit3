// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <math/vector_4d.hpp>
#include <noggit/float_property.hpp>
#include <noggit/ui/noggit_tool.hpp>


namespace noggit::ui
{
  class shadow_editor : public noggit_tool
  {
  public:
    shadow_editor(QWidget* parent = nullptr);

    void change_radius(float change) { _radius_property.change(change); }
    float radius() const { return _radius_property.get(); }

    virtual void tick(float dt, math::vector_3d const& cursor_pos, bool cursor_under_map, World* world) override;
    virtual void mouse_move_event(QLineF const& relative_movement) override;

  private:
    float_property _radius_property;
    float_property _pitch_property;
    float_property _threshold_property;
  };
}
