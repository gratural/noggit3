// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <math/vector_3d.hpp>
#include <math/trig.hpp>

class World;
struct ENTRY_MDDF;
struct ENTRY_MODF;

namespace noggit
{
  class moveable_object
  {
  public:
    moveable_object(math::vector_3d const& pos, math::degrees::vec3 const& rotation);
    moveable_object(math::vector_3d const& pos, math::degrees::vec3 const& rotation, float scale);
    moveable_object(ENTRY_MDDF const* m2_entry);
    moveable_object(ENTRY_MODF const* wmo_entry);

    moveable_object(moveable_object const& other) = default;
    moveable_object& operator= (moveable_object const& other) = default;

    moveable_object(moveable_object&& other);
    moveable_object& operator= (moveable_object&& other);

    virtual void before_move(World* world) {}
    virtual void after_move(World* world) {}

    void set_position(math::vector_3d const& pos);
    void set_rotation(math::degrees::vec3 const& rotation);
    void set_scale(float scale);

    void move(float dx, float dy, float dz, World* world);
    void move(math::vector_3d const& pos_dt, World* world);
    void update_position(math::vector_3d const& pos, World* world);
    void update_rotation(math::degrees::vec3 const& rotation, World* world);
    void update_scale(float scale, World* world);

    math::vector_3d const& position() const { return _position; }
    math::degrees::vec3 const& rotation() const { return _rotation; }
    float scale() const { return _scale; }

    bool can_scale() const { return _can_scale; }

  private:
    math::vector_3d _position;
    math::degrees::vec3 _rotation;
    float _scale;
    bool _can_scale;
  };
}
