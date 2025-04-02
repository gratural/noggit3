// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/ui/asset_browser.hpp>
#include <noggit/ui/ObjectEditor.h>
#include <noggit/MPQ.h>

#include <QtCore/QRegularExpression>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

#include <map>
#include <string>

namespace noggit::ui
{
  asset_browser::asset_browser(QWidget* parent, noggit::ui::object_editor* object_editor) : QWidget(parent)
  {
    setWindowTitle("Asset Browser");
    setWindowIcon(QIcon(":/icon"));
    setWindowFlags(Qt::Tool);
    resize(300, 600);

    QVBoxLayout* layout = new QVBoxLayout(this);
    QGridLayout* search_layout = new QGridLayout();

    _search_bar = new QLineEdit(this);

    auto search_btn = new QPushButton("search", this);
    auto toggle_btn = new QPushButton(">", this);

    // force the size otherwise they take too much space
    search_btn->setFixedWidth(50);
    toggle_btn->setFixedWidth(20);

    search_layout->addWidget(_search_bar, 0, 0);
    search_layout->addWidget(search_btn, 0, 1);
    search_layout->addWidget(toggle_btn, 0, 2);

    layout->addLayout(search_layout);

    _asset_tree = new QTreeWidget();

    layout->addWidget(_asset_tree);

    _asset_tree->setColumnCount(1);
    _asset_tree->setHeaderHidden(true);

    create_tree();

    connect (_asset_tree, &QTreeWidget::itemSelectionChanged
            , [=]
              {
                auto const& selected_items = _asset_tree->selectedItems();
                if (object_editor && selected_items.size())
                {
                  auto tree_item = selected_items.back();
                  std::string file = tree_item->text(0).toStdString();
                  tree_item = tree_item->parent();

                  // recreate the filename from the tree nodes
                  while (tree_item)
                  {
                    file = tree_item->text(0).toStdString() + '/' + file;
                    tree_item = tree_item->parent();
                  }

                  object_editor->copy(file);
                }
              }
            );

    connect( search_btn, &QPushButton::clicked
           , [=]()
             {
               create_tree(_search_bar->text());
             }
           );
    connect( toggle_btn, &QPushButton::clicked
           , [=]()
             {
               if(_expanded)
               {
                 _asset_tree->collapseAll();
                 toggle_btn->setText(">");
               }
               else
               {
                 _asset_tree->expandAll();
                 toggle_btn->setText("<");
               }

               _expanded = !_expanded;
             }
           );
  }

  struct asset_browser::asset_tree_node
  {
    asset_tree_node(QString name) : name(name) {}

    asset_tree_node& add_child(QString const& child_name)
    {
      return children.emplace(child_name, child_name).first->second;
    }

    QString name;
    std::map<QString, asset_tree_node> children;
  };

  void asset_browser::create_tree(QString filter)
  {
    static const QRegularExpression models_and_wmo("([^\\.]+\\.(m2|wmo))");
    static const QRegularExpression wmo_group("(.*_[0-9]{3}\\.wmo)");

    _asset_tree->clear();
    asset_tree_node root({});

    bool use_filter = !filter.isNull();

    filter = QString::fromStdString(noggit::mpq::normalized_filename(filter.toStdString()));

    for (auto const& fileStd : gListfile)
    {
      auto const file = QString::fromStdString(fileStd);

      if (!models_and_wmo.match(file).hasMatch() || wmo_group.match(file).hasMatch())
      {
        continue;
      }

      if (use_filter && !file.contains(filter, Qt::CaseSensitive))
      {
        continue;
      }

      asset_tree_node* node = &root;

      for (QString&& part : file.split('/'))
      {
        node = &node->add_child(std::move(part));
      }
    }

    auto tree_root = _asset_tree->invisibleRootItem();

    for (auto const& child : root.children)
    {
      add_children(child.second, tree_root);
    }

    _asset_tree->resizeColumnToContents(0);

    if (use_filter)
    {
      _asset_tree->expandAll();
    }
  }

  void asset_browser::add_children(asset_tree_node const& data_node, QTreeWidgetItem* parent)
  {
    QTreeWidgetItem* node_item = new QTreeWidgetItem(parent);

    node_item->setText(0, data_node.name);

    parent->addChild(node_item);

    for (auto const& child : data_node.children)
    {
      add_children(child.second, node_item);
    }
  }
}
