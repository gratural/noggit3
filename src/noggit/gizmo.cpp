// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/gizmo.hpp>
#include <noggit/Misc.h>
#include <noggit/moveable_object.hpp>
#include <noggit/World.h>

#include <opengl/context.hpp>
#include <opengl/shader.hpp>


namespace noggit
{
  gizmo::gizmo()
  {
    float p1 = 2.f;
    float center_cube_size = 0.2f;
    float s1 = 3.f, s2 = 0.0525f, s3 = 0.05f, s4 = 1.f; // axis boxes dimensions
    float hb_s1 = 3.25f, hb_s2 = 0.5f, hb_s3 = 0.06f, hb_s4 = 1.2f; // axis hitbox dimensions
    float circle_radius = 4.f;

    math::vector_3d cube_size(center_cube_size, center_cube_size, center_cube_size);

    create_cuboid(math::vector_3d(0.f, 0.f, 0.f), cube_size, cube_size * 1.5f, math::vector_3d(1.f, 1.f, 1.f), gizmo_move_type::xyz);

    math::vector_3d red(1.f, 0.f, 0.f);
    math::vector_3d green(0.f, 1.f, 0.f);
    math::vector_3d blue(0.f, 0.f, 1.f);

    for (int i = -1; i <= 1; i += 2)
    {
      create_cuboid(math::vector_3d(i * p1, 0.f, 0.f), math::vector_3d(s1, s2, s2), math::vector_3d(hb_s1, hb_s2, hb_s2), red, gizmo_move_type::x);
      create_cuboid(math::vector_3d(0.f, i * p1, 0.f), math::vector_3d(s2, s1, s2), math::vector_3d(hb_s2, hb_s1, hb_s2), green, gizmo_move_type::y);
      create_cuboid(math::vector_3d(0.f, 0.f, i * p1), math::vector_3d(s2, s2, s1), math::vector_3d(hb_s2, hb_s2, hb_s1), blue, gizmo_move_type::z);
    }

    for (int i = -1; i <= 1; i += 2)
    {
      for (int j = -1; j <= 1; j += 2)
      {
        create_cuboid(math::vector_3d(i * p1, j * p1, 0.f), math::vector_3d(s4, s4, s3), math::vector_3d(hb_s4, hb_s4, hb_s3), math::vector_3d(1.f, 0.75f, 0.f), gizmo_move_type::xy);
        create_cuboid(math::vector_3d(0.f, i * p1, j * p1), math::vector_3d(s3, s4, s4), math::vector_3d(hb_s3, hb_s4, hb_s4), math::vector_3d(0.f, 1.f, 0.5f), gizmo_move_type::yz);
        create_cuboid(math::vector_3d(i * p1, 0.f, j * p1), math::vector_3d(s4, s3, s4), math::vector_3d(hb_s4, hb_s3, hb_s4), math::vector_3d(0.5f, 0.f, 1.f), gizmo_move_type::zx);
      }
    }

    // the color is also the up vector
    create_quarter_circle(circle_radius, math::vector_3d(0.f, 1.f, 0.f),  red, red, gizmo_move_type::pitch);
    create_quarter_circle(circle_radius, math::vector_3d(0.f, 0.f, 1.f),  red, red, gizmo_move_type::pitch);
    create_quarter_circle(circle_radius, math::vector_3d(0.f, -1.f, 0.f), red, red, gizmo_move_type::pitch);
    create_quarter_circle(circle_radius, math::vector_3d(0.f, 0.f, -1.f), red, red, gizmo_move_type::pitch);

    create_quarter_circle(circle_radius, math::vector_3d(1.f, 0.f, 0.f),  green, green, gizmo_move_type::yaw);
    create_quarter_circle(circle_radius, math::vector_3d(0.f, 0.f, 1.f),  green, green, gizmo_move_type::yaw);
    create_quarter_circle(circle_radius, math::vector_3d(-1.f, 0.f, 0.f), green, green, gizmo_move_type::yaw);
    create_quarter_circle(circle_radius, math::vector_3d(0.f, 0.f, -1.f), green, green, gizmo_move_type::yaw);

    create_quarter_circle(circle_radius, math::vector_3d(1.f, 0.f, 0.f),  blue, blue, gizmo_move_type::roll);
    create_quarter_circle(circle_radius, math::vector_3d(0.f, 1.f, 0.f),  blue, blue, gizmo_move_type::roll);
    create_quarter_circle(circle_radius, math::vector_3d(-1.f, 0.f, 0.f), blue, blue, gizmo_move_type::roll);
    create_quarter_circle(circle_radius, math::vector_3d(0.f, -1.f, 0.f), blue, blue, gizmo_move_type::roll);
  }

  void gizmo::object_has_moved()
  {
    if (is_linked())
    {
      _need_uniform_update = true;
      _need_indices_update = true;

      math::vector_3d pos = position().value();
      _scale = std::max((_camera_position - pos).length(), 2.5f) / 20.f;
    }

  }
  void gizmo::move_camera(math::vector_3d const& camera_pos, math::degrees yaw, math::degrees pitch)
  {
    _camera_position = camera_pos;
    _camera_yaw = yaw;
    _camera_pitch = pitch;
    _need_indices_update = true;

    if (is_linked())
    {
      math::vector_3d pos = position().value();
      _scale = std::max((_camera_position - pos).length(), 2.5f) / 20.f;
    }
    else
    {
      _scale = 1.f;
    }
  }

  void gizmo::intersect(math::ray const& ray, std::vector<gizmo_intersect_data>& results)
  {
    if (!is_linked())
    {
      return;
    }

    // todo: add some bounding box check

    math::vector_3d pos = position().value();

    for (gizmo_indice_group_data const& data : _indices_group_data)
    {
      for (int i = data.start; i < data.start + data.count; i += 3)
      {
        if ( auto distance = ray.intersect_triangle ( _hitbox_vertices[_indices[i + 0]].position * _scale + pos
                                                    , _hitbox_vertices[_indices[i + 1]].position * _scale + pos
                                                    , _hitbox_vertices[_indices[i + 2]].position * _scale + pos
                                                    )
           )
        {
          results.emplace_back(*distance, ray.position(*distance), this, data);
        }
      }
    }
  }

  void gizmo::select_group(gizmo_indice_group_data const& target)
  {
    deselect_group();

    for (int i = 0; i < _indices_group_data.size(); ++i)
    {
      if (_indices_group_data[i].start == target.start)
      {
        _selected_group_index = i;
      }
    }
  }

  void gizmo::handle_mouse_move(float dt_x, float dt_y, World* world)
  {
    if (!is_linked())
    {
      return;
    }

    // used for rotation as scaling doesn't make sense there
    float rx = dt_x;
    float ry = dt_y;
    // todo: scale with screen dimensions
    dt_x *= _scale / 3.f;
    // flip the y change so it matches the mouse movements
    dt_y *= -_scale / 3.f;

    if (_selected_group_index)
    {
      auto& group = _indices_group_data[_selected_group_index.value()];

      math::vector_3d move;
      math::degrees::vec3 rot;
      bool rotate = false;

      switch (group.move_type)
      {
      case gizmo_move_type::x:
        move = math::vector_3d(dt_y, 0.f, dt_x);
        math::rotate(0.f, 0.f, &move.x, &move.z, -_camera_yaw);
        break;
      case gizmo_move_type::y:
        move = math::vector_3d(0.f, dt_y, 0.f);
        break;
      case gizmo_move_type::z:
        move = math::vector_3d(dt_y, 0.f, dt_x);
        math::rotate(0.f, 0.f, &move.x, &move.z, -_camera_yaw);
        break;
      case gizmo_move_type::xy:
        move = math::vector_3d(0.f, dt_y, dt_x);
        math::rotate(0.f, 0.f, &move.x, &move.z, -_camera_yaw);
        break;
      case gizmo_move_type::yz:
        move = math::vector_3d(0.f, dt_y, dt_x);
        math::rotate(0.f, 0.f, &move.x, &move.z, -_camera_yaw);
        break;
      case gizmo_move_type::zx:
        move = math::vector_3d(dt_y, 0.f, dt_x);
        math::rotate(0.f, 0.f, &move.x, &move.z, -_camera_yaw);
        break;
      case gizmo_move_type::xyz:
        move = math::vector_3d(0.f, dt_y, dt_x);
        math::rotate(0.f, 0.f, &move.x, &move.z, -_camera_yaw);
        break;
      case gizmo_move_type::yaw:
        rotate = true;
        rot.y = math::degrees(rx * 5.f);
        break;
      case gizmo_move_type::pitch:
        rot.x = math::degrees(ry * 5.f);
        rotate = true;
        break;
      case gizmo_move_type::roll:
        rot.z = math::degrees(-ry * 5.f);
        rotate = true;
        break;
      }

      if (rotate)
      {
        _linked_object.value()->rotate(rot, world, false);
      }
      else
      {
        _linked_object.value()->move(move, world);
      }
    }
  }

  void gizmo::link_to(noggit::moveable_object* obj)
  {
    if (_linked_object)
    {
      unlink();
    }

    if (obj)
    {
      _linked_object.emplace(obj);
      obj->link_to_gizmo(this);
    }
    else
    {
      _linked_object.value()->unlink_from_gizmo();
      _linked_object.reset();
    }

    _need_uniform_update = true;
  }

  void gizmo::unlink()
  {
    if (_linked_object)
    {
      _linked_object.value()->unlink_from_gizmo();
    }

    _linked_object.reset();
  }

  std::optional<math::vector_3d> gizmo::position() const
  {
    if (_linked_object)
    {
      return _linked_object.value()->position();
    }
    else
    {
      return std::nullopt;
    }
  }

  void gizmo::draw(opengl::scoped::use_program& shader)
  {
    if (!_linked_object)
    {
      return;
    }

    if (!_uploaded)
    {
      upload(shader);
    }

    if (_need_uniform_update)
    {
      shader.uniform("offset", position().value());
      shader.uniform("scale", _scale);
    }

    if (_need_indices_update)
    {
      update_indices();
    }

    opengl::scoped::vao_binder const _ (_vao);
    gl.multiDrawElements(GL_TRIANGLES, _indices_count.data(), GL_UNSIGNED_SHORT, _indices_offsets.data(), _indices_count.size());
  }

  void gizmo::upload(opengl::scoped::use_program& shader)
  {
    _vertex_array.upload();
    _vertex_buffers.upload();

    gl.bufferData<GL_ARRAY_BUFFER>(_vertices_vbo, sizeof(gizmo_vertex) * _vertices.size(), _vertices.data(), GL_STATIC_DRAW);
    gl.bufferData<GL_ELEMENT_ARRAY_BUFFER>(_indices_vbo, sizeof(std::uint16_t) * _indices.size(), _indices.data(), GL_STATIC_DRAW);

    opengl::scoped::index_buffer_manual_binder indices_binder(_indices_vbo);

    {
      opengl::scoped::vao_binder const _ (_vao);

      shader.attrib(_, "position", _vertices_vbo, 3, GL_FLOAT, GL_FALSE, sizeof(gizmo_vertex), static_cast<char*>(0) + offsetof(gizmo_vertex, position));
      shader.attrib(_, "color", _vertices_vbo, 3, GL_FLOAT, GL_FALSE, sizeof(gizmo_vertex), static_cast<char*>(0) + offsetof(gizmo_vertex, color));

      indices_binder.bind();
    }

    _uploaded = true;
  }

  void gizmo::update_indices()
  {
    if (!_linked_object)
    {
      return;
    }

    math::vector_3d pos = position().value();
    math::vector_3d diff = _camera_position - pos;

    int sx = misc::sign(diff.x);
    int sy = misc::sign(diff.y);
    int sz = misc::sign(diff.z);

    _indices_count.clear();
    _indices_offsets.clear();

    for (gizmo_indice_group_data& data : _indices_group_data)
    {
      if ( data.move_type == gizmo_move_type::xyz
        || (  (data.zone_x == sx || data.zone_x == 0)
           && (data.zone_y == sy || data.zone_y == 0)
           && (data.zone_z == sz || data.zone_z == 0)
           )
         )
      {
        _indices_count.push_back(data.count);
        _indices_offsets.push_back(data.offset);
        data.currently_visible = true;
      }
      else
      {
        data.currently_visible = false;
      }
    }

    _need_indices_update = false;
  }

  void gizmo::create_cuboid(math::vector_3d const& center, math::vector_3d const& dimensions, math::vector_3d const& hitbox_size, math::vector_3d color, gizmo_move_type mode)
  {
    int indices_ofs = _vertices.size();
    int indices_start = _indices.size();

    // rendered geometry
    {
      math::vector_3d half_size = dimensions * 0.5f;

      // top
      _vertices.emplace_back(center + math::vector_3d(-half_size.x, half_size.y, -half_size.z), color, mode);
      _vertices.emplace_back(center + math::vector_3d(-half_size.x, half_size.y,  half_size.z), color, mode);
      _vertices.emplace_back(center + math::vector_3d( half_size.x, half_size.y,  half_size.z), color, mode);
      _vertices.emplace_back(center + math::vector_3d( half_size.x, half_size.y, -half_size.z), color, mode);
      // bottom
      _vertices.emplace_back(center + math::vector_3d(-half_size.x, -half_size.y, -half_size.z), color, mode);
      _vertices.emplace_back(center + math::vector_3d(-half_size.x, -half_size.y,  half_size.z), color, mode);
      _vertices.emplace_back(center + math::vector_3d( half_size.x, -half_size.y,  half_size.z), color, mode);
      _vertices.emplace_back(center + math::vector_3d( half_size.x, -half_size.y, -half_size.z), color, mode);
    }
    // hitbox
    {
      math::vector_3d half_hitbox = hitbox_size * 0.5f;

      _hitbox_vertices.emplace_back(center + math::vector_3d(-half_hitbox.x,  half_hitbox.y, -half_hitbox.z), color, mode);
      _hitbox_vertices.emplace_back(center + math::vector_3d(-half_hitbox.x,  half_hitbox.y,  half_hitbox.z), color, mode);
      _hitbox_vertices.emplace_back(center + math::vector_3d( half_hitbox.x,  half_hitbox.y,  half_hitbox.z), color, mode);
      _hitbox_vertices.emplace_back(center + math::vector_3d( half_hitbox.x,  half_hitbox.y, -half_hitbox.z), color, mode);
      _hitbox_vertices.emplace_back(center + math::vector_3d(-half_hitbox.x, -half_hitbox.y, -half_hitbox.z), color, mode);
      _hitbox_vertices.emplace_back(center + math::vector_3d(-half_hitbox.x, -half_hitbox.y,  half_hitbox.z), color, mode);
      _hitbox_vertices.emplace_back(center + math::vector_3d( half_hitbox.x, -half_hitbox.y,  half_hitbox.z), color, mode);
      _hitbox_vertices.emplace_back(center + math::vector_3d( half_hitbox.x, -half_hitbox.y, -half_hitbox.z), color, mode);
    }

    // todo: move to shared function
    auto& add_indice([&](int i)
    {
      _indices.push_back(indices_ofs + i);
    });

    auto& add_triangle([&](int a, int b, int c)
    {
      add_indice(a);
      add_indice(b);
      add_indice(c);
    });

    auto& add_quad([&](int a, int b, int c, int d)
    {
      add_triangle(a, b, c);
      add_triangle(c, d, a);
    });

    add_quad(0, 1, 2, 3);
    add_quad(7, 6, 5, 4);
    add_quad(5, 6, 2, 1);
    add_quad(4, 5, 1, 0);
    add_quad(7, 4, 0, 3);
    add_quad(6, 7, 3, 2);

    gizmo_indice_group_data data;
    data.count = _indices.size() - indices_start;
    data.start = indices_start;
    data.offset = static_cast<char*>(0) + indices_start * sizeof(_indices[0]);
    data.move_type = mode;
    data.zone_x = misc::sign(center.x);
    data.zone_y = misc::sign(center.y);
    data.zone_z = misc::sign(center.z);

    _indices_group_data.push_back(data);
  }

  void gizmo::create_circle(float radius, math::vector_3d const& up, math::vector_3d color, gizmo_move_type mode)
  {
    int indices_ofs = _vertices.size();
    int indices_start = _indices.size();

    auto& add_indice([&](int i)
    {
      _indices.push_back(indices_ofs + i);
    });

    auto& add_triangle([&](int a, int b, int c)
    {
      add_indice(a);
      add_indice(b);
      add_indice(c);
    });

    auto& add_quad([&](int a, int b, int c, int d)
    {
      add_triangle(a, b, c);
      add_triangle(c, d, a);
    });

    math::vector_3d forward(up.y, up.z, up.x);
    forward.normalize();

    int segments = 180;
    float thickness = 0.05f;
    float angle = (2.f * math::constants::pi / segments);

    math::matrix_4x4 rotation(math::matrix_4x4::rotation_xyz, math::degrees::vec3(math::radians(angle * up.x), math::radians(angle * up.y), math::radians(angle * up.z)));

    math::vector_3d p = forward * radius;
    math::vector_3d p1 = p + forward * thickness;
    math::vector_3d p2 = p + up * thickness;
    math::vector_3d p3 = p - forward * thickness;
    math::vector_3d p4 = p - up * thickness;

    for (int i = 0; i <= segments; ++i)
    {
      p1 = rotation * p1;
      p2 = rotation * p2;
      p3 = rotation * p3;
      p4 = rotation * p4;

      _vertices.emplace_back(p1, color, mode);
      _vertices.emplace_back(p2, color, mode);
      _vertices.emplace_back(p3, color, mode);
      _vertices.emplace_back(p4, color, mode);
      _hitbox_vertices.emplace_back(p1 * 1.1f, color, mode);
      _hitbox_vertices.emplace_back(p2, color, mode);
      _hitbox_vertices.emplace_back(p3 * 0.9f, color, mode);
      _hitbox_vertices.emplace_back(p4, color, mode);

      if (i > 0)
      {
        add_quad(5, 6, 2, 1);
        add_quad(4, 5, 1, 0);
        add_quad(7, 4, 0, 3);
        add_quad(6, 7, 3, 2);

        indices_ofs += 4;
      }
    }

    gizmo_indice_group_data data;
    data.count = _indices.size() - indices_start;
    data.start = indices_start;
    data.offset = static_cast<char*>(0) + indices_start * sizeof(_indices[0]);
    data.move_type = mode;
    data.zone_x = 0;
    data.zone_y = 0;
    data.zone_z = 0;

    _indices_group_data.push_back(data);
  }

  void gizmo::create_quarter_circle(float radius, math::vector_3d const& forward, math::vector_3d const& up, math::vector_3d color, gizmo_move_type mode)
  {
    int indices_ofs = _vertices.size();
    int indices_start = _indices.size();

    auto& add_indice([&](int i)
    {
      _indices.push_back(indices_ofs + i);
    });

    auto& add_triangle([&](int a, int b, int c)
    {
      add_indice(a);
      add_indice(b);
      add_indice(c);
    });

    auto& add_quad([&](int a, int b, int c, int d)
    {
      add_triangle(a, b, c);
      add_triangle(c, d, a);
    });

    add_quad(0, 1, 2, 3);


    int segments = 45;
    float thickness = 0.05f;
    float thickness_hb = 0.5f;
    float angle = (0.5f * math::constants::pi / segments);

    math::matrix_4x4 rotation(math::matrix_4x4::rotation_xyz, math::degrees::vec3(math::radians(angle * up.x), math::radians(angle * up.y), math::radians(angle * up.z)));

    math::vector_3d p = forward * radius;
    math::vector_3d p1 = p + forward * thickness;
    math::vector_3d p2 = p + up * thickness;
    math::vector_3d p3 = p - forward * thickness;
    math::vector_3d p4 = p - up * thickness;
    math::vector_3d hb_p1 = p + forward * thickness_hb;
    math::vector_3d hb_p2 = p + up * thickness_hb;
    math::vector_3d hb_p3 = p - forward * thickness_hb;
    math::vector_3d hb_p4 = p - up * thickness_hb;

    math::vector_3d middle;

    for (int i = 0; i <= segments; ++i)
    {
      // used to check in which zone the arc is
      if (i == segments / 2)
      {
        middle = p1;
      }

      _vertices.emplace_back(p1, color, mode);
      _vertices.emplace_back(p2, color, mode);
      _vertices.emplace_back(p3, color, mode);
      _vertices.emplace_back(p4, color, mode);
      _hitbox_vertices.emplace_back(hb_p1, color, mode);
      _hitbox_vertices.emplace_back(hb_p2, color, mode);
      _hitbox_vertices.emplace_back(hb_p3, color, mode);
      _hitbox_vertices.emplace_back(hb_p4, color, mode);

      p1 = rotation * p1;
      p2 = rotation * p2;
      p3 = rotation * p3;
      p4 = rotation * p4;
      hb_p1 = rotation * hb_p1;
      hb_p2 = rotation * hb_p2;
      hb_p3 = rotation * hb_p3;
      hb_p4 = rotation * hb_p4;

      if (i > 0)
      {
        add_quad(5, 6, 2, 1);
        add_quad(4, 5, 1, 0);
        add_quad(7, 4, 0, 3);
        add_quad(6, 7, 3, 2);

        indices_ofs += 4;
      }
    }

    gizmo_indice_group_data data;
    data.count = _indices.size() - indices_start;
    data.start = indices_start;
    data.offset = static_cast<char*>(0) + indices_start * sizeof(_indices[0]);
    data.move_type = mode;
    data.zone_x = misc::sign(middle.x);
    data.zone_y = misc::sign(middle.y);
    data.zone_z = misc::sign(middle.z);

    _indices_group_data.push_back(data);




    add_quad(3, 2, 1, 0);
  }
}
