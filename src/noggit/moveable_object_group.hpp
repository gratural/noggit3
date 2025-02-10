// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include<noggit/moveable_object.hpp>

#include <math/vector_3d.hpp>
#include <math/trig.hpp>

#include <optional>
#include <vector>

namespace noggit
{
  class moveable_object_group : public moveable_object
  {
  public:
    moveable_object_group() : moveable_object(math::vector_3d(), math::degrees::vec3()) {}

    using moveable_object::move;

    virtual void move(math::vector_3d const& pos_dt, World* world) override;
    virtual void rotate(math::degrees::vec3 const& rotation, World* world, bool local) override;

    bool add_object(moveable_object* object);
    bool remove_object(moveable_object* object);
    bool object_is_grouped(moveable_object* object);

    void update_center();
    void reset();

    math::vector_3d local_forward() const;
    math::vector_3d local_up() const;
    math::vector_3d local_right() const;

    int object_count() const { return _objects.size(); }
    std::optional<math::vector_3d> pivot() const;

  private:
    std::vector<moveable_object*> _objects;
  };
}
