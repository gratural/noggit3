// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <noggit/bool_toggle_property.hpp>
#include <noggit/float_property.hpp>
#include <noggit/ui/noggit_tool.hpp>
#include <math/vector_3d.hpp>

namespace noggit
{
  namespace ui
  {
    class clearing_tool : public noggit_tool
    {
    public:
      clearing_tool(QWidget* parent = nullptr);

      virtual void tick(float dt, math::vector_3d const& cursor_pos, bool cursor_under_map, World* world) override;
      virtual void mouse_move_event(QLineF const& relative_movement) override;

      void change_radius(float change) { _radius.change(change); }
      float radius() const { return _radius.get(); }

      QSize sizeHint() const override;

    private:
      int _mode = 0;

      float_property _radius;
      float_property _texture_threshold;

      bool_toggle_property _clear_height;
      bool_toggle_property _clear_textures;
      bool_toggle_property _clear_duplicate_textures;
      bool_toggle_property _clear_textures_under_threshold;
      bool_toggle_property _clear_texture_flags;
      bool_toggle_property _clear_liquids;
      bool_toggle_property _clear_m2s;
      bool_toggle_property _clear_wmos;
      bool_toggle_property _clear_shadows;
      bool_toggle_property _clear_mccv;
      bool_toggle_property _clear_impassible_flag;
      bool_toggle_property _clear_holes;
    };
  }
}
