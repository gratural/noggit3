// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <noggit/Selection.h>
#include <math/matrix_4x4.hpp>
#include <math/vector_3d.hpp>
#include <math/ray.hpp>
#include <math/trig.hpp>
#include <opengl/scoped.hpp>
#include <opengl/shader.fwd.hpp>

#include <optional>
#include <vector>

class World;

namespace noggit
{
  class moveable_object;

  enum class gizmo_move_type : int
  {
    x, y, z,
    xy, yz, zx,
    xyz,
    pitch, yaw, roll
  };

  struct gizmo_vertex
  {
    gizmo_vertex(math::vector_3d const& position, math::vector_3d const& color, gizmo_move_type type)
      : position(position), color(color), move_type(type) { }

    math::vector_3d position;
    math::vector_3d color;
    gizmo_move_type move_type;
  };

  struct gizmo_indice_group_data
  {
    int start;
    int count;
    void* offset;
    gizmo_move_type move_type;
    // -1 = negative, 0 = ignore, +1 = positive
    int zone_x;
    int zone_y;
    int zone_z;
    bool currently_visible;
  };

  class gizmo;

  struct gizmo_intersect_data
  {
    gizmo_intersect_data(float dist, math::vector_3d const& position, gizmo* gizmo, gizmo_indice_group_data group)
      : distance(dist), position(position), gizmo(gizmo), group(group) {}

    float distance;
    math::vector_3d position;
    gizmo* gizmo;
    gizmo_indice_group_data group;
  };

  class gizmo
  {
  public:
    gizmo();

    void object_has_moved();
    void move_camera(math::vector_3d const& camera_pos, math::degrees yaw, math::degrees pitch);

    void intersect(math::ray const& ray, std::vector<gizmo_intersect_data>& results);
    void select_group(gizmo_indice_group_data const& target);
    void deselect_group() { _selected_group_index.reset(); }
    bool is_selected() const { return _selected_group_index.has_value(); }

    void begin_move() { _is_moving = true; }
    void end_move() { _is_moving = false; deselect_group(); }
    bool is_moving() const { return _is_moving; }
    void handle_mouse_move(float dt_x, float dt_y, World* world);

    bool is_linked() const { return _linked_object.has_value(); }
    void link_to(noggit::moveable_object* obj);
    void unlink();

    std::optional<math::vector_3d> position() const;
    bool local_space() const { return _local_space; }
    void toggle_local_space();

    void draw(opengl::scoped::use_program& shader);

  private:
    void upload(opengl::scoped::use_program& shader);

    bool _uploaded = false;
    bool _need_uniform_update = true;
    bool _need_indices_update = true;

    opengl::scoped::deferred_upload_vertex_arrays<1> _vertex_array;
    GLuint const& _vao = _vertex_array[0];
    opengl::scoped::deferred_upload_buffers<2> _vertex_buffers;
    GLuint const& _vertices_vbo = _vertex_buffers[0];
    GLuint const& _indices_vbo = _vertex_buffers[1];

  private:
    std::optional<moveable_object*> _linked_object;

    void update_indices();
    void create_cuboid(math::vector_3d const& center, math::vector_3d const& dimensions, math::vector_3d const& hitbox_size, math::vector_3d color, gizmo_move_type mode);
    void create_circle(float radius, math::vector_3d const& up, math::vector_3d color, gizmo_move_type mode);
    void create_quarter_circle(float radius, math::vector_3d const& forward, math::vector_3d const& up, math::vector_3d color, gizmo_move_type mode);

    std::vector<gizmo_vertex> _vertices;
    std::vector<gizmo_vertex> _hitbox_vertices;
    std::vector<gizmo_vertex> _rotated_hitbox_vertices;
    std::vector<std::uint16_t> _indices;

    std::optional<int> _selected_group_index;
    std::vector<gizmo_indice_group_data> _indices_group_data;
    std::vector<void*> _indices_offsets;
    std::vector<int> _indices_count;

    void update_model_matrix();
    math::vector_3d up() const;
    math::vector_3d forward() const;
    math::vector_3d right() const;

    math::matrix_4x4 _rotation_matrix;
    math::matrix_4x4 _model_matrix;
    math::matrix_4x4 _model_matrix_inv;
    math::vector_3d _camera_position;
    math::degrees _camera_yaw = math::degrees(0.f);
    math::degrees _camera_pitch = math::degrees(0.f);
    float _scale = 1.f;

    bool _is_moving = false;
    bool _local_space = false;
  };
}
