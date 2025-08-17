#ifndef MYTREEVIEW_H
#define MYTREEVIEW_H

#include <QTreeView>
#include <QKeyEvent>
#include <QMouseEvent>

class MyTreeView : public QTreeView {
  Q_OBJECT
public:
  using QTreeView::QTreeView;

protected:
  void keyPressEvent(QKeyEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
};

#endif // MYTREEVIEW_H
