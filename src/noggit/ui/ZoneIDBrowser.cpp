// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/ui/ZoneIDBrowser.h>

#include <noggit/DBC.h>
#include <noggit/Log.h>
#include <noggit/Misc.h>
#include <noggit/World.h>

#include <QtWidgets/QVBoxLayout>

#include <iostream>
#include <sstream>
#include <string>

namespace noggit
{
  namespace ui
  {
    zone_id_browser::zone_id_browser(QWidget* parent)
      : noggit_tool(parent)
      , _area_tree(new QTreeWidget())
      , mapID(-1)
    {
      new QVBoxLayout(this);
      this->layout()->addWidget(_area_tree);


      connect ( _area_tree, &QTreeWidget::itemSelectionChanged
              , [this]
                {
                  auto const& selected_items = _area_tree->selectedItems();
                  if (selected_items.size())
                  {
                    _area_id.emplace(selected_items.back()->data(0, 1).toInt());
                  }
                }
              );
    }

    void zone_id_browser::set_map_id(int id)
    {
      mapID = id;

      for (DBCFile::Iterator i = gMapDB.begin(); i != gMapDB.end(); ++i)
      {
        if (i->getInt(MapDB::MapID) == id)
        {
          std::stringstream ss;
          ss << id << "-" << i->getString(MapDB::InternalName);
          _area_tree->setHeaderLabel(ss.str().c_str());
        }
      }

      build_area_list();
    }

    void zone_id_browser::set_area_id(int id)
    {
      QSignalBlocker const block_area_tree(_area_tree);

      _area_id = id;

      if (_items.find(id) != _items.end())
      {
        _area_tree->selectionModel()->clear();
        auto* item = _items.at(id);

        item->setSelected(true);

        while ((item = item->parent()))
        {
          item->setExpanded(true);
        }
      }
    }

    void zone_id_browser::tick(float dt, math::vector_3d const& cursor_pos, bool cursor_under_map, World* world)
    {
      if (!cursor_under_map && _left_mouse_button)
      {
        if (_mod_shift_down && _area_id)
        {
          // draw the selected AreaId on current selected chunk
          world->setAreaID(cursor_pos, _area_id.value(), false);
        }
        else if (_mod_ctrl_down)
        {
          // pick areaID from chunk
          MapChunk* chunk = world->get_chunk_at(cursor_pos);
          set_area_id(chunk->getAreaID());
        }
      }
    }

    void zone_id_browser::build_area_list()
    {
      QSignalBlocker const block_area_tree(_area_tree);
      _area_tree->clear();
      _area_tree->setColumnCount(1);
      _items.clear();

      //  Read out Area List.
      for (DBCFile::Iterator i = gAreaDB.begin(); i != gAreaDB.end(); ++i)
      {
        if (i->getInt(AreaDB::Continent) == mapID)
        {
          add_area(i->getInt(AreaDB::AreaID));
        }
      }
    }

    QTreeWidgetItem* zone_id_browser::create_or_get_tree_widget_item(int area_id)
    {
      auto it = _items.find(area_id);

      if (it != _items.end())
      {
        return _items.at(area_id);
      }
      else
      {
        QTreeWidgetItem* item = new QTreeWidgetItem();

        std::stringstream ss;
        ss << area_id << "-" << gAreaDB.getAreaName(area_id);
        item->setData(0, 1, QVariant(area_id));
        item->setText(0, QString(ss.str().c_str()));
        _items.emplace(area_id, item);

        return item;
      }
    }

    QTreeWidgetItem* zone_id_browser::add_area(int area_id)
    {
      QTreeWidgetItem* item = create_or_get_tree_widget_item(area_id);

      std::uint32_t parent_area_id = gAreaDB.get_area_parent(area_id);

      if (parent_area_id && parent_area_id != area_id)
      {
        QTreeWidgetItem* parent_item = add_area(parent_area_id);
        parent_item->addChild(item);
      }
      else
      {
        _area_tree->addTopLevelItem(item);
      }

      return item;
    }
  }
}
