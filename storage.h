#ifndef STORAGE_H
#define STORAGE_H

#include "shape.h"

#include <QGraphicsScene>
#include <QVector>


class MyStorage {
private: QVector < IShape * > items;

public:
    string filename; //"C:\\Users\\pvars\\Desktop\\data2.txt";
    QGraphicsScene * scene;
    QGraphicsView *view;
    IShapeCreator * shapeCreator = nullptr;
    // Конструктор

    void setShapeCreator(IShapeCreator * shapeCreator) {
        this -> shapeCreator = shapeCreator;
    }

    MyStorage() {
    }

    // Деструктор
    ~MyStorage() {
        clearAll();
    }
    void unGroupSelected() {
        QVector < IShape * > temp;
        for (auto it = items.begin(); it != items.end();) {
            if (( * it) -> ismovable() &&
                dynamic_cast < GroupComposite * > ( * it) != nullptr) {
                QVector < IShape * > groupShapes =
                    dynamic_cast < GroupComposite * > ( * it) -> getShapesToUngroup();
                // не удаляем it, тк не хотим удалить фигуры
                for (auto i: groupShapes) {
                    temp.append(i);
                }
                * it = nullptr;
                it = items.erase(it);
            } else {
                ++it;
            }
        }
        for (IShape * i: temp) {
            this -> addItem(i);
        }
    }
    void createGroupFromSelected() {
        QVector < IShape * > temp;
        for (auto it = items.begin(); it != items.end();) {
            if (( * it) -> ismovable()) {
                temp.append( * it);
                it = items.erase(it);
            } else {
                ++it;
            }
        }
        IShape * newGroup = new GroupComposite(temp, scene);
        this -> addItem(newGroup);
    }

    void setView(QGraphicsView * view) {
        this -> scene = view->scene();
        this->view = view;
    }
    virtual IShape * createShape(string type, ifstream & inFile) {
        return nullptr;
    }

    void setCreatorShape(IShapeCreator * shapeCreator) {
        this -> shapeCreator = shapeCreator;
    }

    void load() {
        this -> clearAll();
        // if (shapeCreator == nullptr) throw runtime_error("error: shapecreator = nullptr");

        std::ifstream inFile(filename);

        string s;

        while (inFile >> s) {
            IShape * newItem;
            IShapeCreator * shapeCreator = new ShapeCreator(inFile, view);

            shapeCreator -> setType(s);
            newItem = shapeCreator -> createShape();

            if (newItem != nullptr) {
                items.append(newItem);
            } else {
                throw runtime_error("MyStorage::load() newItem = nullptr");
            }
        }
        debug("ASD");
        inFile.close();
    }

    void save() {
        ofstream outFile(filename);
        for (IShape * shape: items) {

            shape -> saveToFile(outFile);
        }
        outFile.close();
    }

    // Удаление всех элементов
    void clearAll() {
        for (IShape * item: items) {
            if (item) {
                delete item; // освобождаем память
            }
        }
        items.clear();
    }

    // Удаление только выбранных
    void clearSelected() {
        for (auto it = items.begin(); it != items.end();) {
            if ( * it && ( * it) -> ismovable()) {
                if (scene) {
                    ( * it) -> removeFromScene(); // Удаляем со сцены
                }
                delete * it; // Освобождаем память
                it = items.erase(it); // Удаляем элемент из вектора и получаем
                // итератор на следующий
            } else {
                ++it; // Переходим к следующему элементу
            }
        }
    }

    // Добавить элемент в общий список элементов
    void addItem(IShape * item) {
        items.append(item);
        // save();
    }

    // Снять выделение со всех элементов
    void unselectAll() {
        debug("unselectAll");
        for (IShape * item: items) {
            if (item == nullptr) {
                debug("item = nullptr");
                continue;
            }
            if (dynamic_cast < GroupComposite * > (item)) {
                debug("it`s group");
            } else {
                debug("it`s shape");
            }
            debug("unselectAll item->setmovable 2");
            item -> setMovable(false);
            debug("unselectAll 22222");
            item -> showUnSelected(item -> color);
            debug("unselectAll showUnSelected");
        }
        debug("unselectAll3");
    }

    // Получить элемент по индексу
    IShape * getItem(int index) {
        if (index >= 0 && index < items.size()) {
            return items[index];
        }
        return nullptr;
    }

    // Получить количество элементов
    int count() {
        return items.size();
    }

    // Получить выбранные элементы
    QVector < IShape * > getSelectedItems() {
        QVector < IShape * > selectedItems;
        for (auto i: items) {
            if (i && i -> ismovable()) {
                selectedItems.append(i);
            }
        }
        return selectedItems;
    }
};

#endif // STORAGE_H
