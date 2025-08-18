#ifndef STORAGE_H
#define STORAGE_H

#include <QGraphicsScene>
#include <QVector>

#include "shape.h"

class MyStorage : public QObject {
  Q_OBJECT
private:
  QVector<IShape *> items;

public:
  string filename; //"C:\\Users\\pvars\\Desktop\\data2.txt";
  QGraphicsScene *scene;
  QGraphicsView *view;
  IShapeCreator *shapeCreator = nullptr;
  // Конструктор

  void setShapeCreator(IShapeCreator *shapeCreator);

  MyStorage();

  // Деструктор
  ~MyStorage();
  void unGroupSelected();
  void createGroupFromSelected();

  void setView(QGraphicsView *view);
  virtual IShape *createShape(string type, ifstream &inFile);

  void setCreatorShape(IShapeCreator *shapeCreator);

  void load();

  void save();

  // Удаление всех элементов
  void clearAll();

  // Удаление только выбранных
  void clearSelected();

  // Добавить элемент в общий список элементов
  void addItem(IShape *item);

  // Снять выделение со всех элементов
  void unselectAll();

  // Получить элемент по индексу
  IShape *getItem(int index);

  // Получить количество элементов
  int count();

  // Получить выбранные элементы
  QVector<IShape *> getSelectedItems();

  const QVector<IShape *> getItems();

signals:
  void shapeAdded(IShape*);

};

inline void MyStorage::setShapeCreator(IShapeCreator *shapeCreator) {
  this->shapeCreator = shapeCreator;
}

inline MyStorage::MyStorage() {}

inline MyStorage::~MyStorage() { clearAll(); }

inline void MyStorage::unGroupSelected() {
  QVector<IShape *> temp;
  for (auto it = items.begin(); it != items.end();) {
    if ((*it)->ismovable() &&
        dynamic_cast<GroupComposite *>(*it) != nullptr) {
      QVector<IShape *> groupShapes =
          dynamic_cast<GroupComposite *>(*it)->getShapesToUngroup();
      // не удаляем it, тк не хотим удалить фигуры
      for (auto i : groupShapes) {
        temp.append(i);
      }
      *it = nullptr;
      it = items.erase(it);
    } else {
      ++it;
    }
  }
  for (IShape *i : temp) {
    this->addItem(i);
  }
}

inline void MyStorage::createGroupFromSelected() {
  QVector<IShape *> temp;
  for (auto it = items.begin(); it != items.end();) {
    if ((*it)->ismovable()) {
      temp.append(*it);
      it = items.erase(it);
    } else {
      ++it;
    }
  }
  IShape *newGroup = new GroupComposite(temp, scene);
  this->addItem(newGroup);
}

inline void MyStorage::setView(QGraphicsView *view) {
  this->scene = view->scene();
  this->view = view;
}

inline IShape *MyStorage::createShape(string type, ifstream &inFile) { return nullptr; }

inline void MyStorage::setCreatorShape(IShapeCreator *shapeCreator) {
  this->shapeCreator = shapeCreator;
}

inline void MyStorage::load() {
  this->clearAll();
  // if (shapeCreator == nullptr) throw runtime_error("error: shapecreator =
  // nullptr");

  std::ifstream inFile(filename);

  string s;

  while (inFile >> s) {
    IShape *newItem;
    IShapeCreator *shapeCreator = new ShapeCreator(inFile, view);

    shapeCreator->setType(s);
    newItem = shapeCreator->createShape();

    if (newItem != nullptr) {
      items.append(newItem);
    } else {
      throw runtime_error("MyStorage::load() newItem = nullptr");
    }
  }
  inFile.close();
}

inline void MyStorage::save() {
  ofstream outFile(filename);
  for (IShape *shape : items) {
    shape->saveToFile(outFile);
  }
  outFile.close();
}

inline void MyStorage::clearAll() {
  for (IShape *item : items) {
    if (item) {
      delete item; // освобождаем память
    }
  }
  items.clear();
}

inline void MyStorage::clearSelected() {
  for (auto it = items.begin(); it != items.end();) {
    if (*it && (*it)->ismovable()) {
      if (scene) {
        (*it)->removeFromScene(); // Удаляем со сцены
      }
      delete *it;           // Освобождаем память
      it = items.erase(it); // Удаляем элемент из вектора и получаем
                            // итератор на следующий
    } else {
      ++it; // Переходим к следующему элементу
    }
  }
}

inline void MyStorage::addItem(IShape *item) {
  items.append(item);
  if (dynamic_cast<CustomShape*>(item))
    emit dynamic_cast<CustomShape*>(item)->shapeChanged(item);
  // save();
  emit this->shapeAdded(item);
}

inline void MyStorage::unselectAll() {
  for (IShape *item : items) {
    if (item == nullptr) {
      // debug("item = nullptr");
      continue;
    }
    if (dynamic_cast<GroupComposite *>(item)) {
    } else {
    }
    item->setMovable(false);
    item->showUnSelected(item->color);
  }
}

inline IShape *MyStorage::getItem(int index) {
  if (index >= 0 && index < items.size()) {
    return items[index];
  }
  return nullptr;
}

inline int MyStorage::count() { return items.size(); }

inline QVector<IShape *> MyStorage::getSelectedItems() {
  QVector<IShape *> selectedItems;
  for (auto i : items) {
    if (i && i->ismovable()) {
      selectedItems.append(i);
    }
  }
  return selectedItems;
}

inline const QVector<IShape *> MyStorage::getItems()
{
  return this->items;
}

#endif // STORAGE_H
