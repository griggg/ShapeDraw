#include "shapetreemodel.h"

#include "ishape.h"

ShapeTreeModel::ShapeTreeModel(QObject *parent) : QAbstractItemModel(parent) {}

// Кол-во строк (детей) у parent
int rowCount(const QModelIndex &parent = QModelIndex()) const override {
  if (!parent.isValid())
    return root.children.size();

  const Item *parentItem = static_cast<const Item *>(parent.internalPointer());
  return parentItem->children.size();
}

// Кол-во колонок (всегда 2)
int columnCount(const QModelIndex &parent = QModelIndex()) const override {
  Q_UNUSED(parent);
  return 2;
}

// Данные для отображения
QVariant data(const QModelIndex &index, int role) const override {
  if (!index.isValid())
    return QVariant();

  const Item *item = static_cast<const Item *>(index.internalPointer());
  if (!item)
    return QVariant();

  if (role == Qt::DisplayRole) {
    if (index.column() == 0)
      return item->name;
    else if (index.column() == 1)
      return item->description;
    else if (index.column() == 2)
      return "";
  } else if (role == Qt::ToolTipRole) {
    // Возвращаем текст подсказки для ячейки
    if (index.column() == 0)
      return QString("Это имя: %1").arg(item->name);
    else if (index.column() == 1)
      return QString("Описание: %1").arg(item->description);
    else if (index.column() == 2)
      return QString("Дополнительная информация");
  }

  return QVariant();
}

// Создаём индекс (строка, колонка, родитель)
QModelIndex index(int row, int column,
                  const QModelIndex &parent = QModelIndex()) const override {
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  const Item *parentItem = nullptr;
  if (!parent.isValid())
    parentItem = &root;
  else
    parentItem = static_cast<const Item *>(parent.internalPointer());

  if (row < 0 || row >= parentItem->children.size())
    return QModelIndex();

  const Item *childItem = &parentItem->children[row];
  // Важно: в createIndex передаем void* с const_cast, чтобы хранить указатель
  // на элемент
  return createIndex(row, column, const_cast<Item *>(childItem));
}

// Получаем родителя индекса
QModelIndex parent(const QModelIndex &child) const override {
  const Item *childItem = static_cast<const Item *>(child.internalPointer());

  const Item *parentItem = findParent(&root, childItem);
  if (!parentItem || parentItem == &root)
    return QModelIndex();

  // И так далее...

  const Item *grandParent = findParent(&root, parentItem);

  // Далее, чтобы создать индекс, нужно убрать const:

  int row = 0;
  const QList<Item> *siblings = nullptr;

  if (grandParent)
    siblings = &grandParent->children;
  else
    siblings = &root.children;

  for (int i = 0; i < siblings->size(); ++i) {
    if (&(*siblings)[i] == parentItem) {
      row = i;
      break;
    }
  }

  // const_cast для передачи указателя в createIndex:
  return createIndex(row, 0, const_cast<Item *>(parentItem));
}

// Заголовки колонок
QVariant headerData(int section, Qt::Orientation orientation,
}

const ShapeTreeModel::IShape *findParent(const IShape *current,
                                         const IShape *target) const {
  for (const Item &child : current->children) {
    if (&child == target)
      return current;
    const Item *res = findParent(&child, target);
    if (res)
      return res;
  }
  return nullptr;
}
}
;
