#ifndef SHAPETREEMODEL_H
#define SHAPETREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

#include "storage.h"

#include "shape.h"

class ShapeTreeModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  explicit ShapeTreeModel(MyStorage *storage, QObject *parent = nullptr);
  ~ShapeTreeModel();

         // QAbstractItemModel interface
  QVariant data(const QModelIndex &index, int role) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  void setStorage(MyStorage *storage);
  void refresh();

private:
  MyStorage *m_storage;
  const IShape *getItem(const QModelIndex &index) const;
};

#endif // SHAPETREEMODEL_H
