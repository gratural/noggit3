// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <noggit/ui/noggit_tool.hpp>

#include <QtWidgets/QWidget>
#include <QtWidgets/QTreeWidget>

#include <functional>
#include <optional>
#include <string>

namespace noggit
{
  namespace ui
  {
    class zone_id_browser : public noggit_tool
    {
    public:
      zone_id_browser(QWidget* parent = nullptr);
      void set_map_id(int id);
      void set_area_id(int id);
      std::optional<int> area_id() const { return _area_id; }

      virtual void tick(float dt, math::vector_3d const& cursor_pos, bool cursor_under_map, World* world) override;

    private:
      QTreeWidget* _area_tree;
      std::map<int, QTreeWidgetItem*> _items;
      int mapID;

      std::optional<int> _area_id;

      void build_area_list();
      QTreeWidgetItem* create_or_get_tree_widget_item(int area_id);
      QTreeWidgetItem* add_area(int area_id);
    };
  }
}
