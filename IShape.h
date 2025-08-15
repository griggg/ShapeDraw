#ifndef ISHAPE_H
#define ISHAPE_H

#include <QAbstractGraphicsShapeItem>

#include <QApplication>

#include <QColorDialog>

#include <QDebug>

#include <QFileDialog>

#include <QFormLayout>

#include <QGraphicsEllipseItem>

#include <QGraphicsItem>

#include <QGraphicsScene>

#include <QGraphicsView>

#include <QLabel>

#include <QLayout>

#include <QMainWindow>

#include <QMouseEvent>

#include <QPainterPath>

#include <QPushButton>

#include <QRandomGenerator>

#include <QSizeGrip>

#include <QSplitter>

#include <QSet>

#include <fstream>

#include <stack>

#include <vector>

class IShape {
public: virtual~IShape() {}
    virtual void changeSize(QRectF) = 0;
    virtual bool isContains(QPointF p) = 0;
    virtual void changeColor(QColor color) = 0;

    virtual void setNewPos(double w, double h) = 0;

    virtual void showSelected() = 0;

    virtual void showUnSelected(QColor color) = 0;
    virtual void drawOnScene(QGraphicsScene * scene) = 0;
    virtual bool ismovable() = 0;

    virtual void setMovable(bool) = 0;

    virtual void removeFromScene() = 0;
    virtual void move(double difX, double difY, QGraphicsView * view) = 0;
    virtual void scale(double mlt, QGraphicsView * view) = 0;
    virtual bool canScale(double mlt, QGraphicsView * view) = 0;

    virtual bool isShapeRectInView(QGraphicsView * view) = 0;
    virtual void moveInViewBack(QGraphicsView * view) = 0;
    virtual bool isFocused(QPointF scenePos) = 0;

    virtual void saveToFile(std::ofstream & ) = 0;
    virtual IShape * loadFromFile(std::ifstream & ) = 0;

    virtual bool canMove(double difX, double difY, QGraphicsView * view) = 0;

    QColor color;
};

#endif // ISHAPE_H
