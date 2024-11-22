// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <math/vector_3d.hpp>
#include <noggit/Brush.h>
#include <noggit/float_property.hpp>
#include <noggit/TextureManager.h>
#include <noggit/ui/CurrentTexture.h>

#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>

#include <boost/optional.hpp>

class World;

namespace noggit
{
  namespace ui
  {
    class texture_swapper : public QWidget
    {
    public:
      texture_swapper ( QWidget* parent
                      , const math::vector_3d* camera_pos
                      , World*
                      );

      boost::optional<scoped_blp_texture_reference> const& texture_to_swap() const
      {
        return _texture_to_swap;
      }

      float radius() const { return _radius.get(); }
      float hardness() const { return _hardness.get(); }
      float pressure() const { return _pressure.get(); }

      void change_radius(float change) { _radius.change(change); }
      void change_hardness(float change) { _hardness.change(change); }
      void change_pressure(float change) { _pressure.change(change); }

      bool brush_mode() const
      {
        return _brush_mode_group->isChecked();
      }

      Brush brush() const { return Brush(radius(), hardness()); }

      void toggle_brush_mode()
      {
        _brush_mode_group->setChecked(!_brush_mode_group->isChecked());
      }

      void set_texture(std::string const& filename);

      current_texture* const texture_display() { return _texture_to_swap_display; }

    private:
      boost::optional<scoped_blp_texture_reference> _texture_to_swap;
      float_property _radius;
      float_property _hardness;
      float_property _pressure;

    private:
      current_texture* _texture_to_swap_display;

      QGroupBox* _brush_mode_group;
    };
  }
}
