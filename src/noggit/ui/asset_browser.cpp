// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/ui/asset_browser.hpp>
#include <noggit/ui/ObjectEditor.h>
#include <noggit/MPQ.h>


#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

#include <regex>

namespace noggit::ui
{
  asset_browser::asset_browser(QWidget* parent, noggit::ui::object_editor* object_editor) : QWidget(parent)
  {
    setWindowTitle("Asset Browser");
    setWindowIcon(QIcon(":/icon"));
    setWindowFlags(Qt::Tool);
    resize(300, 600);

    QVBoxLayout* layout = new QVBoxLayout(this);
    QGridLayout* search_layout = new QGridLayout(this);

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
               create_tree(_search_bar->text().toStdString());
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

  void asset_browser::create_tree(std::string filter)
  {
    static const QRegularExpression models_and_wmo("([^\\.]+\\.(m2|wmo))");
    static const QRegularExpression wmo_group("(.*_[0-9]{3}\\.wmo)");

    _asset_tree->clear();
    asset_tree_node root("root");

    bool use_filter = filter != "";

    QStringList sorted_listfile;
    for (const auto& file : gListfile) {
      sorted_listfile.append(QString::fromStdString(file));
    }
    
    sorted_listfile.sort();

    for (const QString& file : sorted_listfile)
    {
      if (!models_and_wmo.match(file).hasMatch() || wmo_group.match(file).hasMatch())

      {
        continue;
      }

      if (use_filter && !file.contains(QString::fromStdString(filter), Qt::CaseInsensitive))

      {
        continue;
      }

      QStringList parts = file.split('/');
      asset_tree_node* node = &root;
  
      for (const QString& part : parts)
      {
          node = &node->add_child(part.toStdString());
      }
    }

    auto tree_root = _asset_tree->invisibleRootItem();

    for (asset_tree_node const& child : root.children)
    {
      add_children(child, tree_root);
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

    node_item->setText(0, QString::fromStdString(data_node.name));

    parent->addChild(node_item);

    for (asset_tree_node const& child : data_node.children)
    {
      add_children(child, node_item);
    }
  }
}
