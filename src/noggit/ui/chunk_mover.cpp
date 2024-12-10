// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/ui/chunk_mover.hpp>
#include <noggit/ui/checkbox.hpp>
#include <noggit/ui/double_spinbox.hpp>
#include <noggit/ui/slider_spinbox.hpp>

#include <noggit/map_chunk_headers.hpp>
#include <noggit/World.h>

#include <QButtonGroup>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QRadioButton>

namespace noggit::ui
{
  chunk_mover_ui::chunk_mover_ui(noggit::chunk_mover* chunk_mover, QWidget* parent)
    : noggit_tool(parent)
    , _chunk_mover(chunk_mover)
    , _override_height(true)
    , _override_textures(true)
    , _override_alphamaps(true)
    , _override_vertex_colors(true)
    , _override_liquids(true)
    , _override_shadows(false)
    , _override_area_id(true)
    , _override_holes(true)
    , _override_models(true)
    , _radius(15.f)
    , _square_brush(true)
    , _fix_gaps(true)
    , _clear_shadows(true)
    , _clear_models(true)
    , _preview_enabled(true)
  {
    auto layout(new QFormLayout(this));

    QGroupBox* overrides_group = new QGroupBox("Override", this);
    auto overrides_layout(new QFormLayout(overrides_group));

    overrides_layout->addRow(new checkbox("Height", &_override_height, overrides_group));
    overrides_layout->addRow(new checkbox("Textures", &_override_textures, overrides_group));
    overrides_layout->addRow(new checkbox("Alphamaps", &_override_alphamaps, overrides_group));
    overrides_layout->addRow(new checkbox("Vertex Colors", &_override_vertex_colors, overrides_group));
    overrides_layout->addRow(new checkbox("Liquids", &_override_liquids, overrides_group));
    overrides_layout->addRow(new checkbox("Shadows", &_override_shadows, overrides_group));
    overrides_layout->addRow(new checkbox("Area ID", &_override_area_id, overrides_group));
    overrides_layout->addRow(new checkbox("Holes", &_override_holes, overrides_group));
    overrides_layout->addRow(new checkbox("Models", &_override_models, overrides_group));

    layout->addRow(overrides_group);

    QGroupBox* ground_param_group = new QGroupBox("Height Mode", this);
    auto ground_param_layout(new QGridLayout(ground_param_group));

    QButtonGroup* height_mode_group = new QButtonGroup(ground_param_group);
    QRadioButton* hm_normal = new QRadioButton("Normal");
    QRadioButton* hm_min = new QRadioButton("Min");
    QRadioButton* hm_max = new QRadioButton("Max");
    QRadioButton* hm_add = new QRadioButton("Add");
    QRadioButton* hm_sub = new QRadioButton("Subtract");

    height_mode_group->addButton(hm_normal, static_cast<int>(chunk_override_params::height_mode::normal));
    height_mode_group->addButton(hm_min, static_cast<int>(chunk_override_params::height_mode::min));
    height_mode_group->addButton(hm_max, static_cast<int>(chunk_override_params::height_mode::max));
    height_mode_group->addButton(hm_add, static_cast<int>(chunk_override_params::height_mode::add));
    height_mode_group->addButton(hm_sub, static_cast<int>(chunk_override_params::height_mode::subtract));
    hm_normal->toggle();

    ground_param_layout->addWidget(hm_normal, 0, 0);
    ground_param_layout->addWidget(hm_min, 1, 0);
    ground_param_layout->addWidget(hm_max, 1, 1);
    ground_param_layout->addWidget(hm_add, 2, 0);
    ground_param_layout->addWidget(hm_sub, 2, 1);

    layout->addRow(ground_param_group);

    QGroupBox* param_group = new QGroupBox("Parameters", this);
    auto param_layout(new QFormLayout(param_group));

    param_layout->addRow(new slider_spinbox("Radius", &_radius, 1.f, 500.f, 0, this));
    param_layout->addRow(new checkbox("Square Brush", &_square_brush, param_group));

    auto spinbox(new double_spinbox(&chunk_mover->height_offset_property(), this));
    spinbox->setDecimals(3);
    spinbox->setRange(-20000.f, 20000.f);
    param_layout->addRow("Height Offset:", spinbox);

    param_layout->addRow(new checkbox("Fix Gaps", &_fix_gaps, param_group));
    param_layout->addRow(new checkbox("Clear Shadows", &_clear_shadows, param_group));
    param_layout->addRow(new checkbox("Clear Models", &_clear_models, param_group));
    param_layout->addRow(new checkbox("Preview Changes", &_preview_enabled, param_group));

    layout->addRow(param_group);


    connect ( height_mode_group, qOverload<int> (&QButtonGroup::buttonClicked)
            , [&] (int id)
              {
                _height_mode = static_cast<chunk_override_params::height_mode>(id);
              }
            );
  }

  void chunk_mover_ui::tick(float dt, math::vector_3d const& cursor_pos, bool cursor_under_map, World* world)
  {
    if (_left_mouse_button)
    {
      bool square_brush = use_square_brush();

      if (_mod_shift_down)
      {
        world->select_chunks_in_range(cursor_pos, radius(), square_brush, false, *_chunk_mover);
      }
      if (_mod_ctrl_down)
      {
        world->select_chunks_in_range(cursor_pos, radius(), square_brush, true, *_chunk_mover);
      }
    }

    // disable preview when selecting/deselecting chunks
    if (_mod_shift_down || _mod_ctrl_down)
    {
      _chunk_mover->disable_preview();
    }
    else
    {
      _chunk_mover->enable_preview();
    }
  }

  void chunk_mover_ui::mouse_move_event(QLineF const& relative_movement)
  {
    if (_left_mouse_button && _mod_alt_down)
    {
      change_radius(relative_movement.dx() / mouse_sensibility);
    }
  }

  void chunk_mover_ui::wheel_event(QWheelEvent* event)
  {
    change_height_offset(scroll_wheel_delta_for_range(event, 10.f));
  }

  void chunk_mover_ui::change_height_offset(float change)
  {
    _chunk_mover->height_offset_property().change(change);
  }

  void chunk_mover_ui::paste_selection()
  {
    _chunk_mover->apply(false);
  }

  chunk_override_params chunk_mover_ui::override_params() const
  {
    chunk_override_params params;
    params.height = _override_height.get();
    params.textures = _override_textures.get();
    params.alphamaps = _override_alphamaps.get();
    params.vertex_colors = _override_vertex_colors.get();
    params.liquids = _override_liquids.get();
    params.shadows = _override_shadows.get();
    params.area_id = _override_area_id.get();
    params.holes = _override_holes.get();
    params.models = _override_models.get();

    params.height_override_mode = _height_mode;

    params.fix_gaps = _fix_gaps.get();
    params.clear_shadows = _clear_shadows.get();
    params.clear_models = _clear_models.get();

    params.preview_terrain_changes = _preview_enabled.get();

    return params;
  }

  void chunk_mover_ui::toggle_preview()
  {
    _preview_enabled.toggle();

    _chunk_mover->apply(true);
  }
}
