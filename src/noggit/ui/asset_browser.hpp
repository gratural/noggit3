// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <QtCore/QString>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

namespace noggit::ui
{
  class object_editor;

  class asset_browser : public QWidget
  {
    Q_OBJECT
  public:
    asset_browser(QWidget* parent = nullptr, noggit::ui::object_editor* object_editor = nullptr);

  private:
    void create_tree(QString filter = QString{});

    struct asset_tree_node;
    void add_children(asset_tree_node const& data_node, QTreeWidgetItem* parent);

    bool _expanded = false;

    QLineEdit* _search_bar;
    QTreeWidget* _asset_tree;
  };
}
