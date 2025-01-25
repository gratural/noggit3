// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/moveable_object_group.hpp>

#include <math/matrix_4x4.hpp>
#include <math/quaternion.hpp>

namespace noggit
{

  void moveable_object_group::move(math::vector_3d const& pos_dt, World* world)
  {
    for (moveable_object* object : _objects)
    {
      object->move(pos_dt, world);
    }

    set_position(position() + pos_dt);
  }

  void moveable_object_group::rotate(math::degrees::vec3 const& rotation, World* world, bool local)
  {
    // for a single object pass the call to the object directly as it is its own pivot
    if (_objects.size() == 1)
    {
      _objects.front()->rotate(rotation, world, local);
    }
    else
    {
      if (local)
      {
        // not implemented yet
      }
      else
      {
        math::vector_3d const& pivot = position();
        math::degrees::vec3 r = rotation;
        r.y = -r.y;

        math::matrix_4x4 rotation_mat(math::matrix_4x4::rotation_xyz, -r);

        for (moveable_object* object : _objects)
        {
          math::vector_3d diff = object->position() - pivot;
          diff = rotation_mat * diff;

          object->update_position(pivot + diff, world);
          object->rotate(rotation, world, local);
        }
      }
    }
  }

  bool moveable_object_group::add_object(moveable_object* object)
  {
    if (object_is_grouped(object))
    {
      return false;
    }

    _objects.push_back(object);

    update_center();
    return true;
  }

  bool moveable_object_group::remove_object(moveable_object* object)
  {
    for (auto it = _objects.begin(); it != _objects.end(); ++it)
    {
      if (*it == object)
      {
        _objects.erase(it);

        update_center();
        return true;
      }
    }

    return false;
  }

  bool moveable_object_group::object_is_grouped(moveable_object* object)
  {
    for (auto it = _objects.begin(); it != _objects.end(); ++it)
    {
      if (*it == object)
      {
        return true;
      }
    }

    return false;
  }

  void moveable_object_group::update_center()
  {
    math::vector_3d center(0.f, 0.f, 0.f);

    for (moveable_object* object : _objects)
    {
      center += object->position();
    }

    // to avoid 0 div when empty
    center /= std::max(std::size_t(1), _objects.size());

    set_position(center);
  }

  void moveable_object_group::reset()
  {
    unlink_from_gizmo();
    _objects.clear();
    update_center();
    set_rotation(math::quaternion());
  }

  std::optional<math::vector_3d> moveable_object_group::pivot() const
  {
    if (object_count() > 0)
    {
      return position();
    }
    else
    {
      return std::nullopt;
    }
  }
}
