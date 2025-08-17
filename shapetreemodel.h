#ifndef SHAPETREEMODEL_H
#define SHAPETREEMODEL_H

#include <QAbstractItemModel>

#include "ishape.h"

class ShapeTreeModel : public QAbstractItemModel {
public:
  ShapeTreeModel(QObject *parent = nullptr);

private:
  IShape *root;
  const IShape *findParent(const IShape *current, const IShape *target) const;
};

#endif // SHAPETREEMODEL_H
