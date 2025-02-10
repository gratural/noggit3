// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/moveable_object_group.hpp>

#include <noggit/Misc.h>

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
    math::degrees::vec3 r = rotation;

    // for a single object pass the call to the object directly as it is its own pivot
    if (_objects.size() == 1)
    {
      r.y = -r.y;
      _objects.front()->rotate(r, world, local);
      set_rotation(_objects.front()->quaternion());
    }
    else
    {
      math::quaternion q;
      bool flip_model_rotation = false;

      if (!misc::float_equals(rotation.x._, 0.f))
      {
        q = math::quaternion(local ? local_forward() : math::vector_3d(1.f, 0.f, 0.f), rotation.x);
      }
      else if (!misc::float_equals(rotation.y._, 0.f))
      {
        q = math::quaternion(local ? local_up() : math::vector_3d(0.f, 1.f, 0.f), rotation.y);
        flip_model_rotation = true;
      }
      else if (!misc::float_equals(rotation.z._, 0.f))
      {
        q = math::quaternion(local ? local_right() : math::vector_3d(0.f, 0.f, 1.f), rotation.z);
      }

      if (local)
      {
        if (flip_model_rotation)
        {
          r.y = -r.y;
        }

        moveable_object::rotate(r, world, true);
      }

      // local rotations need to be improved
      math::matrix_4x4 rotation_mat = math::matrix_4x4(math::matrix_4x4::rotation, q);
      math::vector_3d const& pivot = position();

      // the models rotate on the opposite direction on the y axis to keep facing
      // the same direction relative to the pivot
      if (flip_model_rotation)
      {
        q = q.conjugate();
      }

      for (moveable_object* object : _objects)
      {
        math::vector_3d diff = object->position() - pivot;
        diff = rotation_mat * diff;

        object->update_position(pivot + diff, world);
        object->rotate(q, world, local);
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
    if (_objects.size() == 0)
    {
      reset();
      return;
    }

    math::vector_3d center(0.f, 0.f, 0.f);

    for (moveable_object* object : _objects)
    {
      center += object->position();
    }

    // to avoid 0 div when empty
    center /= std::max(std::size_t(1), _objects.size());

    set_position(center);

    if (_objects.size() == 1)
    {
      set_rotation(_objects.front()->rotation());
    }
    else
    {
      set_rotation(math::degrees::vec3());
    }
  }

  void moveable_object_group::reset()
  {
    unlink_from_gizmo();
    _objects.clear();
    set_rotation(math::quaternion());
  }

  math::vector_3d moveable_object_group::local_forward() const
  {
    static math::vector_3d const forward(1.0f, 0.0f, 0.0f);
    math::matrix_4x4 m(math::matrix_4x4::rotation_yzx, rotation());

    return (m * forward).normalized();
  }
  math::vector_3d moveable_object_group::local_up() const
  {
    static math::vector_3d const up(0.0f, 1.0f, 0.0f);
    math::matrix_4x4 m(math::matrix_4x4::rotation_yzx, rotation());

    return (m * up).normalized();
  }
  math::vector_3d moveable_object_group::local_right() const
  {
    return -(local_up() % local_forward()).normalized();
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
