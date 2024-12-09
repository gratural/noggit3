// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <noggit/bool_toggle_property.hpp>
#include <noggit/Selection.h>

#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QWidget>
#include <QDockWidget>

class World;

namespace noggit
{
  namespace ui
  {
    class rotation_editor : public QWidget
    {
    public:
      rotation_editor(QWidget* parent, World* world, noggit::bool_toggle_property* use_median_pivot_point);

      void updateValues(World* world);
    private:
      noggit::bool_toggle_property* _use_median_pivot_point;

      // for single selection
      void set_model_rotation(World* world);
      // for multi selection
      void change_models_rotation(World* world);

      QDoubleSpinBox* _rotation_x;
      QDoubleSpinBox* _rotation_z;
      QDoubleSpinBox* _rotation_y;
      QDoubleSpinBox* _position_x;
      QDoubleSpinBox* _position_z;
      QDoubleSpinBox* _position_y;
      QDoubleSpinBox* _scale;
    };
  }
}
