// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/moveable_object.hpp>

#include <noggit/gizmo.hpp>
#include <noggit/MapHeaders.h>
#include <noggit/World.h>

namespace noggit
{
  moveable_object::moveable_object(math::vector_3d const& pos, math::degrees::vec3 const& rotation)
    : _position(pos)
    , _rotation(rotation)
    , _scale(1.f)
    , _can_scale(false)
  {

  }
  moveable_object::moveable_object(math::vector_3d const& pos, math::degrees::vec3 const& rotation, float scale)
    : _position(pos)
    , _rotation(rotation)
    , _scale(scale)
    , _can_scale(true)
  {

  }

  moveable_object::moveable_object(ENTRY_MDDF const* m2_entry)
  {
    _position = math::vector_3d(m2_entry->pos[0], m2_entry->pos[1], m2_entry->pos[2]);
    _rotation = math::degrees::vec3( math::degrees(m2_entry->rot[0])
                                   , math::degrees(m2_entry->rot[1])
                                   , math::degrees(m2_entry->rot[2])
                                   );
    _scale = m2_entry->scale / 1024.0f;
    _can_scale = true;
  }
  moveable_object::moveable_object(ENTRY_MODF const* wmo_entry)
  {
    _position = math::vector_3d(wmo_entry->pos[0], wmo_entry->pos[1], wmo_entry->pos[2]);
    _rotation = math::degrees::vec3( math::degrees(wmo_entry->rot[0])
                                   , math::degrees(wmo_entry->rot[1])
                                   , math::degrees(wmo_entry->rot[2])
                                   );
    _scale = 1.f;
    _can_scale = false;
  }

  moveable_object::moveable_object(moveable_object&& other)
    : _position(other._position)
    , _rotation(other._rotation)
    , _scale(other._scale)
    , _can_scale(other._can_scale)
  {

  }

  moveable_object& moveable_object::operator= (moveable_object&& other)
  {
    std::swap(_position, other._position);
    std::swap(_rotation, other._rotation);
    std::swap(_scale, other._scale);
    std::swap(_can_scale, other._can_scale);

    return *this;
  }

  void moveable_object::set_position(math::vector_3d const& pos)
  {
    _position = pos;
    update_gizmo_if_linked();
  }
  void moveable_object::set_rotation(math::degrees::vec3 const& rotation)
  {
    _rotation = rotation;
  }
  void moveable_object::set_scale(float scale)
  {
    if (_can_scale)
    {
      _scale = scale;
    }
  }

  void moveable_object::move(float dx, float dy, float dz, World* world)
  {
    move(math::vector_3d(dx, dy, dz), world);
  }

  void moveable_object::move(math::vector_3d const& pos_dt, World* world)
  {
    update_position(_position + pos_dt, world);
  }

  void moveable_object::rotate(math::degrees::vec3 const& rotation, World* world)
  {
    before_move(world);
    set_rotation(_rotation + rotation);
    after_move(world);
  }

  void moveable_object::update_position(math::vector_3d const& pos, World* world)
  {
    before_move(world);
    set_position(pos);
    after_move(world);
  }

  void moveable_object::update_rotation(math::degrees::vec3 const& rotation, World* world)
  {
    before_move(world);
    set_rotation(rotation);
    after_move(world);
  }

  void moveable_object::update_scale(float scale, World* world)
  {
    before_move(world);
    set_scale(scale);
    after_move(world);
  }

  void moveable_object::link_to_gizmo(noggit::gizmo* gizmo)
  {
    _linked_gizmo = gizmo;
  }
  void moveable_object::update_gizmo_if_linked()
  {
    if (_linked_gizmo)
    {
      _linked_gizmo.value()->object_has_moved();
    }
  }
  void moveable_object::unlink_from_gizmo()
  {
    _linked_gizmo.reset();
  }
}
