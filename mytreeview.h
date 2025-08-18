#ifndef MYTREEVIEW_H
#define MYTREEVIEW_H

#include <QTreeView>
#include <QKeyEvent>
#include <QMouseEvent>

class MyTreeView : public QTreeView {
  Q_OBJECT
public:
  MyTreeView(QWidget *parent);
  QModelIndex hoveredIndex;
protected:
  void keyPressEvent(QKeyEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent *event) override;

  void leaveEvent(QEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

signals:
  void shapeSelected(QModelIndex, bool);
};

#endif // MYTREEVIEW_H
