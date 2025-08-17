#include "mytreeview.h"
#include <QDebug>

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
