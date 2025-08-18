#include "mytreeview.h"
#include <QDebug>



MyTreeView::MyTreeView(QWidget *parent) : QTreeView(parent)
{
  setMouseTracking(true);
  hoveredIndex = QModelIndex();
  setFont(QFont("Arial", 12)); // Шрифт Arial, размер 12
}

void MyTreeView::keyPressEvent(QKeyEvent* event) {
  // Игнорируем все клавиши вообще
  qDebug() << "Key press ignored:" << event->text();
  event->accept(); // ничего не делаем
}



void MyTreeView::mousePressEvent(QMouseEvent* event) {
  QModelIndex index = indexAt(event->pos());

  if (index.isValid() && model()->hasChildren(index)) {
    // Разворачиваем/сворачиваем только при клике по группе
    if (isExpanded(index))
      collapse(index);
    else
      expand(index);

    event->accept();
  } else {
    // Клик по листу — просто передаём базовому классу
    QTreeView::mousePressEvent(event);
  }
}

void MyTreeView::mouseMoveEvent(QMouseEvent *event)
{
  QModelIndex index = indexAt(event->pos());
  if (index != hoveredIndex) {
    hoveredIndex = index;
    update();
  }
  QTreeView::mouseMoveEvent(event);
}

void MyTreeView::leaveEvent(QEvent *event) {
  if (hoveredIndex.isValid()) {
    emit shapeSelected(hoveredIndex, false);

    hoveredIndex = QModelIndex();
    viewport()->update();
  }
  QTreeView::leaveEvent(event);
}

void MyTreeView::paintEvent(QPaintEvent *event)  {
  QTreeView::paintEvent(event);
  if (hoveredIndex.isValid()) {
    emit shapeSelected(hoveredIndex, true);
  } else {
    emit shapeSelected(QModelIndex(), false);
  }
}
