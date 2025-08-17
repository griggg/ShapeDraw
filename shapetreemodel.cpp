#include "shapetreemodel.h"

ShapeTreeModel::ShapeTreeModel(MyStorage *storage, QObject *parent)
    : QAbstractItemModel(parent), m_storage(storage)
{
}

ShapeTreeModel::~ShapeTreeModel()
{
}

void ShapeTreeModel::setStorage(MyStorage *storage)
{
  beginResetModel();
  m_storage = storage;
  endResetModel();
}

void ShapeTreeModel::refresh()
{
  beginResetModel();
  endResetModel();
}

QVariant ShapeTreeModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role != Qt::DisplayRole && role != Qt::ToolTipRole)
    return QVariant();

  const IShape *item = getItem(index);
  if (!item)
    return QVariant();

  if (index.column() == 0) {
    // Первая колонка - имя фигуры
    return QString::fromStdString(ItemTypeToStr(item->getType()));
  } else if (index.column() == 1) {
    // Вторая колонка - дополнительная информация
    const CustomShape* shape = dynamic_cast<const CustomShape*>(item);
    if (shape == nullptr) return QString("");
    else return QString("X: %1, Y: %2").arg(shape->getX()).arg(shape->getY());
  }

  return QVariant();
}

Qt::ItemFlags ShapeTreeModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return Qt::NoItemFlags;

  return QAbstractItemModel::flags(index);
}

QVariant ShapeTreeModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    if (section == 0) return "Shape";
    else if (section == 1) return "Position";
  }
  return QVariant();
}

QModelIndex ShapeTreeModel::index(int row, int column, const QModelIndex &parent) const
{
  if (!m_storage || !hasIndex(row, column, parent))
    return QModelIndex();

  const IShape *parentItem;

  if (!parent.isValid()) {
    // Корневой элемент - обращаемся к хранилищу
    if (row >= m_storage->count())
      return QModelIndex();
    parentItem = m_storage->getItem(row);
  } else {
    // Дочерний элемент
    parentItem = getItem(parent);
    if (!parentItem)
      return QModelIndex();

           // Проверяем, является ли родитель группой
    const GroupComposite *group = dynamic_cast<const GroupComposite*>(parentItem);
    if (!group || row >= group->childrens())
      return QModelIndex();

    parentItem = group->getShapesToUngroup()[row];
  }

  return createIndex(row, column, const_cast<IShape*>(parentItem));
}

QModelIndex ShapeTreeModel::parent(const QModelIndex &index) const
{
  if (!index.isValid())
    return QModelIndex();

  const IShape *childItem = getItem(index);
  if (!childItem)
    return QModelIndex();

         // Ищем родителя в хранилище
  for (int i = 0; i < m_storage->count(); ++i) {
    const IShape *item = m_storage->getItem(i);
    if (!item) continue;

           // Если это группа и она содержит наш элемент
    const GroupComposite *group = dynamic_cast<const GroupComposite*>(item);
    if (group) {
      const QVector<IShape*> children = group->getShapesToUngroup();
      for (int j = 0; j < children.size(); ++j) {
        if (children[j] == childItem) {
          return createIndex(i, 0, const_cast<IShape*>(item));
        }
      }
    }
  }

  return QModelIndex();
}

int ShapeTreeModel::rowCount(const QModelIndex &parent) const
{
  if (!m_storage)
    return 0;

  if (!parent.isValid()) {
    // Корневой уровень - количество элементов в хранилище
    return m_storage->count();
  } else {
    // Для групп - количество дочерних элементов
    const IShape *parentItem = getItem(parent);
    if (!parentItem)
      return 0;

    const GroupComposite *group = dynamic_cast<const GroupComposite*>(parentItem);
    if (group)
      return group->childrens();
    else
      return 0; // Обычные фигуры не имеют детей
  }
}

int ShapeTreeModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 2; // Две колонки: имя и позиция
}

const IShape *ShapeTreeModel::getItem(const QModelIndex &index) const
{
  if (!index.isValid())
    return nullptr;

  return static_cast<const IShape*>(index.internalPointer());
}
