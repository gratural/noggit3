// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/ui/terrain_tool.hpp>

#include <noggit/tool_enums.hpp>
#include <noggit/World.h>
#include <noggit/ui/checkbox.hpp>
#include <noggit/ui/slider_spinbox.hpp>
#include <util/qt/overload.hpp>

#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>

namespace noggit
{
  namespace ui
  {
    terrain_tool::terrain_tool(bool_toggle_property* auto_update_water_opacity, QWidget* parent)
      : noggit_tool(parent)
      , _edit_type (eTerrainType_Linear)
      , _radius(15.0f)
      , _speed(2.0f)
      , _inner_radius(0.0f)
      , _vertex_angle (0.0f)
      , _vertex_orientation (0.0f)
      , _auto_update_water_opacity(auto_update_water_opacity)
      , _vertex_mode(eVertexMode_Center)
    {

      auto layout (new QFormLayout (this));

      _type_button_group = new QButtonGroup (this);
      QRadioButton* radio_flat = new QRadioButton ("Flat");
      QRadioButton* radio_linear = new QRadioButton ("Linear");
      QRadioButton* radio_smooth = new QRadioButton ("Smooth");
      QRadioButton* radio_polynomial = new QRadioButton ("Polynomial");
      QRadioButton* radio_trigo = new QRadioButton ("Trigonom");
      QRadioButton* radio_quadra = new QRadioButton ("Quadratic");
      QRadioButton* radio_gauss = new QRadioButton ("Gaussian");
      QRadioButton* radio_vertex = new QRadioButton ("Vertex");

      _type_button_group->addButton (radio_flat, (int)eTerrainType_Flat);
      _type_button_group->addButton (radio_linear, (int)eTerrainType_Linear);
      _type_button_group->addButton (radio_smooth, (int)eTerrainType_Smooth);
      _type_button_group->addButton (radio_polynomial, (int)eTerrainType_Polynom);
      _type_button_group->addButton (radio_trigo, (int)eTerrainType_Trigo);
      _type_button_group->addButton (radio_quadra, (int)eTerrainType_Quadra);
      _type_button_group->addButton (radio_gauss, (int)eTerrainType_Gaussian);
      _type_button_group->addButton (radio_vertex, (int)eTerrainType_Vertex);

      radio_linear->toggle();

      QGroupBox* terrain_type_group (new QGroupBox ("Type"));
      QGridLayout* terrain_type_layout (new QGridLayout (terrain_type_group));
      terrain_type_layout->addWidget (radio_flat, 0, 0);
      terrain_type_layout->addWidget (radio_linear, 0, 1);
      terrain_type_layout->addWidget (radio_smooth, 1, 0);
      terrain_type_layout->addWidget (radio_polynomial, 1, 1);
      terrain_type_layout->addWidget (radio_trigo, 2, 0);
      terrain_type_layout->addWidget (radio_quadra, 2, 1);
      terrain_type_layout->addWidget (radio_gauss, 3, 0);
      terrain_type_layout->addWidget (radio_vertex, 3, 1);

      layout->addWidget (terrain_type_group);

      QGroupBox* settings_group = new QGroupBox("Edit Mode", this);
      QFormLayout* settings_layout(new QFormLayout(settings_group));

      auto only_affect_ground_below_cursor_cb = new checkbox("Below cursor", &_only_affect_ground_below_cursor, this);
      only_affect_ground_below_cursor_cb->setToolTip("Only affect the ground under the cursor height (or over when lowering the ground)");
      settings_layout->addWidget(only_affect_ground_below_cursor_cb);

      auto only_affect_ground_above_cursor_cb = new checkbox("Above cursor", &_only_affect_ground_above_cursor, this);
      only_affect_ground_above_cursor_cb->setToolTip("Only affect the ground above the cursor height (or under when lowering the ground)");
      settings_layout->addWidget(only_affect_ground_above_cursor_cb);

      layout->addWidget(settings_group);


      QGroupBox* radius_group (new QGroupBox ("Radius"));
      QFormLayout* radius_layout (new QFormLayout (radius_group));

      radius_layout->addRow(new slider_spinbox("Outer", &_radius, 0.f, 1000.f, 2, radius_group));
      radius_layout->addRow(new slider_spinbox("Inner", &_inner_radius, 0.f, 1.f, 2, radius_group));

      layout->addWidget (radius_group);

      _speed_box = new QGroupBox (this);
      QFormLayout* speed_layout (new QFormLayout (_speed_box));
      speed_layout->addRow (new slider_spinbox("Speed", &_speed, 0.f, 10.f, 2, _speed_box));


      layout->addWidget (_speed_box);

      _vertex_type_group = new QGroupBox ("Vertex edit");
      QVBoxLayout* vertex_layout (new QVBoxLayout (_vertex_type_group));

      _vertex_button_group = new QButtonGroup (this);
      QRadioButton* radio_mouse = new QRadioButton ("Cursor", _vertex_type_group);
      QRadioButton* radio_center = new QRadioButton ("Selection center", _vertex_type_group);

      radio_mouse->setToolTip ("Orient vertices using the cursor pos as reference");
      radio_center->setToolTip ("Orient vertices using the selection center as reference");

      _vertex_button_group->addButton (radio_mouse, (int)eVertexMode_Mouse);
      _vertex_button_group->addButton (radio_center, (int)eVertexMode_Center);

      radio_center->toggle();

      QHBoxLayout* vertex_type_layout (new QHBoxLayout);
      vertex_type_layout->addWidget (radio_mouse);
      vertex_type_layout->addWidget (radio_center);
      vertex_layout->addItem (vertex_type_layout);

      QHBoxLayout* vertex_angle_layout (new QHBoxLayout);
      vertex_angle_layout->addWidget (_orientation_dial = new QDial (_vertex_type_group));
      _orientation_dial->setRange(0, 360);
      _orientation_dial->setWrapping(true);
      _orientation_dial->setSliderPosition(_vertex_orientation._ - 90); // to get ingame orientation
      _orientation_dial->setToolTip("Orientation");
      _orientation_dial->setSingleStep(10);

      vertex_angle_layout->addWidget (_angle_slider = new QSlider (_vertex_type_group));
      _angle_slider->setRange(-89, 89);
      _angle_slider->setSliderPosition(_vertex_angle._);
      _angle_slider->setToolTip("Angle");

      vertex_layout->addItem (vertex_angle_layout);

      layout->addWidget (_vertex_type_group);
      _vertex_type_group->hide();

      QGroupBox* options_group = new QGroupBox("Options", this);
      QFormLayout* options_layout(new QFormLayout(options_group));


      options_layout->addRow(new checkbox("Models follow ground", &_models_follow_ground, this));
      options_layout->addRow(new checkbox("Models follow ground normals", &_models_follow_ground_normals, this));
      options_layout->addRow(new checkbox("Auto update water opacity", auto_update_water_opacity, this));

      layout->addWidget(options_group);

      connect ( _type_button_group, qOverload<int> (&QButtonGroup::buttonClicked)
              , [&] (int id)
                {
                  _edit_type = static_cast<eTerrainType> (id);
                  updateVertexGroup();
                }
              );

      connect ( _vertex_button_group, qOverload<int> (&QButtonGroup::buttonClicked)
              , [&] (int id)
                {
                  _vertex_mode = id;
                }
              );

      connect ( _angle_slider, &QSlider::valueChanged
              , [this] (int v)
                  {
                    setAngle (v);
                  }
                );

      connect ( _orientation_dial, &QDial::valueChanged
              , [this] (int v)
                  {
                    setOrientation (v + 90.0f);
                  }
                );

      setMinimumWidth(sizeHint().width());
    }

    void terrain_tool::tick(float dt, math::vector_3d const& cursor_pos, bool cursor_under_map, World* world)
    {
      if (_vertices_height_dt != 0.f)
      {
        moveVertices(world, _vertices_height_dt);
        _vertices_height_dt = 0.f;
      }

      if (cursor_under_map)
      {
        return;
      }

      if (_left_mouse_button)
      {
        if (_mod_shift_down)
        {
          changeTerrain(world, cursor_pos, 7.5f * dt);
        }
        else if (_mod_ctrl_down)
        {
          changeTerrain(world, cursor_pos, -7.5f * dt);
        }
      }

      if (_right_mouse_button && _mod_space_down && _edit_type == eTerrainType_Vertex)
      {
        setOrientRelativeTo(world, cursor_pos);
      }
    }

    void terrain_tool::mouse_move_event(QLineF const& relative_movement)
    {
      if (_left_mouse_button)
      {
        if (_mod_alt_down)
        {
          change_radius(relative_movement.dx() / mouse_sensibility);
        }
        if (_mod_space_down)
        {
          change_speed(relative_movement.dx() / (mouse_sensibility * 2.f));
        }
      }
      if (_right_mouse_button)
      {
        if (_mod_shift_down && _edit_type == eTerrainType_Vertex)
        {
          _vertices_height_dt = -relative_movement.dy() / mouse_sensibility;
        }
        if (_mod_alt_down)
        {
          change_inner_radius(relative_movement.dx() / (mouse_sensibility * 15.f));
        }
      }
    }

    void terrain_tool::wheel_event(QWheelEvent* event)
    {
      if (_mod_alt_down)
      {
        changeOrientation(scroll_wheel_delta_for_range(event, 360.f));
      }
      else if (_mod_shift_down)
      {
        changeAngle(scroll_wheel_delta_for_range(event, 178.f));
      }
    }

    void terrain_tool::changeTerrain(World* world, math::vector_3d const& pos, float dt)
    {
      if(_edit_type != eTerrainType_Vertex)
      {
        terrain_edit_mode edit_mode = terrain_edit_mode::normal;

        if (_only_affect_ground_below_cursor.get())
        {
          edit_mode = terrain_edit_mode::only_below_cursor;
        }
        else if(_only_affect_ground_above_cursor.get())
        {
          edit_mode = terrain_edit_mode::only_above_cursor;
        }

        world->changeTerrain(pos, dt * _speed.get(), _radius.get(), _edit_type, _inner_radius.get(), edit_mode);

        if (_models_follow_ground.get())
        {
          world->raise_models_terrain_brush(pos, dt * _speed.get(), _radius.get(), _edit_type, _inner_radius.get(), _models_follow_ground_normals.get());
        }

        if (_auto_update_water_opacity->get())
        {
          world->update_water_opacity(pos, _radius.get());
        }
      }
      else
      {
        // < 0 ==> control is pressed
        if (dt >= 0.0f)
        {
          world->selectVertices(pos, _radius.get());
        }
        else
        {
          if (world->deselectVertices(pos, _radius.get()))
          {
            _vertex_angle = math::degrees (0.0f);
            _vertex_orientation = math::degrees (0.0f);
            world->clearVertexSelection();
          }
        }
      }
    }

    void terrain_tool::moveVertices (World* world, float dt)
    {
      world->moveVertices(dt * _speed.get());
    }

    void terrain_tool::flattenVertices (World* world)
    {
      if (_edit_type == eTerrainType_Vertex)
      {
        world->flattenVertices (world->vertexCenter().y);
      }
    }

    void terrain_tool::nextType()
    {
      _edit_type = static_cast<eTerrainType> ((static_cast<int> (_edit_type) + 1) % eTerrainType_Count);
      _type_button_group->button (_edit_type)->toggle();
      updateVertexGroup();
    }

    void terrain_tool::changeOrientation (float change)
    {
      setOrientation (_vertex_orientation._ + change);
    }

    void terrain_tool::setOrientation (float orientation)
    {
      if (_edit_type == eTerrainType_Vertex)
      {
        QSignalBlocker const blocker (_orientation_dial);

        while (orientation >= 360.0f)
        {
          orientation -= 360.0f;
        }
        while (orientation < 0.0f)
        {
          orientation += 360.0f;
        }

        _vertex_orientation = math::degrees (orientation);
        _orientation_dial->setSliderPosition (_vertex_orientation._ - 90.0f);

        emit updateVertices(_vertex_mode, _vertex_angle, _vertex_orientation);
      }
    }

    void terrain_tool::setOrientRelativeTo (World* world, math::vector_3d const& pos)
    {
      if (_edit_type == eTerrainType_Vertex)
      {
        math::vector_3d const& center = world->vertexCenter();
        _vertex_orientation = math::radians (std::atan2(center.z - pos.z, center.x - pos.x));
        emit updateVertices(_vertex_mode, _vertex_angle, _vertex_orientation);
      }
    }

    void terrain_tool::changeAngle (float change)
    {
      setAngle (_vertex_angle._ + change);
    }

    void terrain_tool::setAngle (float angle)
    {
      if (_edit_type == eTerrainType_Vertex)
      {
        QSignalBlocker const blocker (_angle_slider);
        _vertex_angle = math::degrees (std::max(-89.0f, std::min(89.0f, angle)));
        _angle_slider->setSliderPosition (_vertex_angle._);
        emit updateVertices(_vertex_mode, _vertex_angle, _vertex_orientation);
      }
    }

    void terrain_tool::updateVertexGroup()
    {
      if (_edit_type != eTerrainType_Vertex)
      {
        _vertex_type_group->hide();
        _speed_box->show();
      }
      else
      {
        _vertex_type_group->show();
        _speed_box->hide();
      }
    }

    QSize terrain_tool::sizeHint() const
    {
      return QSize(215, height());
    }
  }
}
