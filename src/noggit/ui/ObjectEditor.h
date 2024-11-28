// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <math/trig.hpp>
#include <math/vector_3d.hpp>
#include <noggit/Selection.h>
#include <noggit/bool_toggle_property.hpp>
#include <noggit/ui/noggit_tool.hpp>

#include <QLabel>
#include <QWidget>

#include <boost/optional.hpp>

class MapView;
class QButtonGroup;
class World;

namespace noggit
{
  namespace ui
  {
    class asset_browser;
    class model_import;
    class rotation_editor;
    class helper_models;
  }
}

enum ModelPasteMode
{
  PASTE_ON_TERRAIN,
  PASTE_ON_SELECTION,
  PASTE_ON_CAMERA,
  PASTE_MODE_COUNT
};

namespace noggit
{
  struct object_paste_params
  {
    float minRotation = -180.f;
    float maxRotation = 180.f;
    float minTilt = -5.f;
    float maxTilt = 5.f;
    float minScale = 0.9f;
    float maxScale = 1.1f;
  };

  namespace ui
  {
    class object_editor : public noggit_tool
    {
    public:
      object_editor ( MapView*
                    , World*
                    , QWidget* parent = nullptr
                    );

      ~object_editor();

      void import_last_model_from_wmv(int type);
      void copy(std::string const& filename);
      void copy_current_selection(World* world);
      void pasteObject ( math::vector_3d cursor_pos
                       , math::vector_3d camera_pos
                       , World*
                       );
      void togglePasteMode();

      model_import *modelImport;
      rotation_editor* rotationEditor;
      helper_models* helper_models_widget;
      asset_browser* asset_browser_widget;
      QSize sizeHint() const override;

      virtual void tick(float dt, math::vector_3d const& cursor_pos, bool cursor_under_map, World* world) override;
      virtual void reset_extra_states() override;

      virtual void mouse_move_event(QLineF const& relative_movement) override;
      virtual void key_press_event(QKeyEvent* event) override;
      virtual void key_release_event(QKeyEvent* event) override;

      void set_cam_yaw(math::degrees yaw) { _camera_yaw = yaw; }
    private:
      // movement related
      float _mouse_mov_x;
      float _mouse_mov_y;

      float _mouse_rotation_dt = 0.f;

      float _move_speed_factor = 0.001f;

      int _scale_key = 0;
      int _rot_y_key = 0;
      int _move_x_key = 0;
      int _move_y_key = 0;
      int _move_z_key = 0;

      math::degrees _camera_yaw = math::degrees(0.f);

    private:
      noggit::bool_toggle_property _move_model_to_cursor_position = { true };
      noggit::bool_toggle_property _snap_multi_selection_to_ground = { false };
      noggit::bool_toggle_property _rotate_along_ground = { true };
      noggit::bool_toggle_property _rotate_along_ground_smooth = { true };
      noggit::bool_toggle_property _rotate_along_ground_random = { false };
      noggit::bool_toggle_property _use_median_pivot_point = { true };

      object_paste_params _paste_params;

      QButtonGroup* pasteModeGroup;
      QLabel* _filename;

      bool _copy_model_stats;

      std::vector<selection_type> selected;
      std::vector<selection_type> _model_instance_created;

      void replace_selection(std::vector<selection_type> new_selection);

      void showImportModels();
      void SaveObjecttoTXT (World*);
      int pasteMode;
    };
  }
}
