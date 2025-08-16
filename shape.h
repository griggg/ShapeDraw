#ifndef SHAPE_H
#define SHAPE_H

#include "IShape.h"
#include "IShapeCreator.h"

using namespace std;

void debug(std::string info) {
    qDebug() << info.c_str() << Qt::endl;
}
void debug2(QString info) {
    qDebug() << info << Qt::endl;
}


enum ItemType {
    TRNGL = 0, CRCL = 1, LINE = 2, TRPZD = 3, SQUARE = 4
};

class GroupComposite: public IShape {
protected: int size;
    int x;
    int y;
    QVector < IShape * > shapes;
    IShapeCreator * shapeCreator;

public: QGraphicsScene * scene;
    QColor color;
    QGraphicsView * view = nullptr;

    ~GroupComposite() {
        for (int i = 0; i < shapes.count(); i++) {
            delete shapes[i];
        }
        shapes.clear();
        debug("GroupComposite destructor");
    }

    void changeColor(QColor color) override {
        this -> color = color;
        for (auto i: shapes) {
            i -> changeColor(color);
        }
    }

    QVector < IShape * > getShapesToUngroup() {
        return shapes;
    }

    void setShapeCreator(IShapeCreator * shapeCreator) {
        this -> shapeCreator = shapeCreator;
    }

    virtual IShape * loadFromFile(ifstream & inFile) override {
        debug("Group loadFromFile");
        if (shapeCreator == nullptr)
            throw runtime_error("Error: GroupComposite::loadFromFile() shapeCreator = nullptr");
        // shapeCreator = new ShapeCreator(inFile, scene);
        int n;
        inFile >> n;
        qDebug() << n << " " << ": loadFromFile debug1" << Qt::endl;
        for (int i = 0; i < n; i++) {
            string s;
            inFile >> s;
            IShape * newItem;
            qDebug() << s << " " << ": loadFromFile 3" << Qt::endl;
            if (s == "GROUP") {
                qDebug() << s << " " << ": loadFromFile 3 GROUP" << Qt::endl;
                QVector < IShape * > empty;
                GroupComposite * group = new GroupComposite(empty, scene);
                group -> setShapeCreator(shapeCreator);
                newItem = group -> loadFromFile(inFile);
            } else {
                shapeCreator -> setType(s);
                shapeCreator -> setInFile(inFile);
                newItem = shapeCreator -> createShape();

            }
            if (newItem == nullptr)
                throw runtime_error("Error: GroupComposite::loadFromFile() newItem = nullptr");
            shapes.append(newItem);;
        }

        return this;
    }

    GroupComposite(QVector < IShape * > shapes, QGraphicsScene * scene) {

        for (IShape * s: shapes) {
            this -> shapes.append(s);
        }
        this -> view = * (scene -> views().begin());
        this -> scene = scene;
        this -> setMovable(false);
        this -> showUnSelected(this -> color);
        this -> color = QColor(Qt::blue);
    }

    void saveToFile(ofstream & outFile) override {
        outFile << "GROUP" << endl << (int) shapes.count() << endl;
        for (IShape * s: shapes)
            s -> saveToFile(outFile);
    }

    void removeFromScene() override {
        for (IShape * s: shapes)
            s -> removeFromScene();
    }

    virtual void changeSize(QRectF rect) override {
        for (IShape * s: shapes)
            s -> changeSize(rect);
    }

    bool isContains(QPointF p) override {
        for (IShape * s: shapes)
            if (s -> isContains(p))
                return true;
        return false;
    }

    void setNewPos(double x, double y) override {
        for (IShape * s: shapes)
            s -> setNewPos(x, y);
    }

    virtual void showSelected() override {
        debug("qqqq");
        for (IShape * s: shapes)
            s -> showSelected();
    }

    virtual void showUnSelected(QColor color) override {
        debug("showUnSelected");
        for (IShape * s: shapes) {
            if (s == nullptr)
                throw runtime_error("GroupComposite::showUnSelected() s = nullptr");
            s -> showUnSelected(color);
        }
        debug("showUnSelected2");
    }

    bool isFocused(QPointF scenePos) override {
        bool is = false;
        for (IShape * s: shapes)
            if (s -> isFocused(scenePos))
                return true;
        return false;
    }

    virtual bool canScale(double mlt) override {
        bool can = true;
        for (IShape * s: shapes) {
            can = can && s -> canScale(mlt);
        }
        return can;
    }

    void scale(double mlt) override {
        if (canScale(mlt) == false)
            return;
        for (IShape * s: shapes) {
            s -> scale(mlt);
        }
    }

    bool isShapeRectInView() override {

        for (IShape * s: shapes)
            if (s -> isShapeRectInView() == false)
                return false;
        return true;
    }

    bool canMove(double difX, double difY) override {
        for (IShape * shape: shapes) {
            if (shape -> canMove(difX, difY) == false)
                return false;
        }
        return true;
    }

    void move(double difX, double difY) override {
        if (canMove(difX, difY) == false)
            return;
        for (IShape * shape: shapes) {
            shape -> move(difX, difY);
        }
    }

    void setMovable(bool val) override{
        debug("Group setMovable start");
        this -> movable = val;
        for (IShape * s: shapes)
            s -> setMovable(val);
        debug("Group setMovable success");
    }

    void draw() override {
        for (IShape * s: shapes) {
            s -> draw();
        }
    }

    bool ismovable() override {
        return this -> movable;
    }

    void moveInViewBack() override {
        for (IShape * s: shapes)
            s -> moveInViewBack();
    }

private: bool movable;
};

// Single responsibility нарушает, не только создает но и загружает
// Везде с интерфейсами а не с конкретными работать
// разбить на файлы


class CustomShape: protected QAbstractGraphicsShapeItem, public IShape {
protected: int size;
    int x;
    int y;

public:
    QGraphicsScene * scene = nullptr;
    QGraphicsView *view = nullptr;
    QColor color;

    CustomShape(int posX, int posY, int size, QGraphicsView* view, QGraphicsItem * parent = nullptr): QAbstractGraphicsShapeItem(parent) {
        setAcceptHoverEvents(true);
        this -> size = size;
        this -> movable = false;
        this->view = view;
        this->scene = view->scene();
        x = posX;
        y = posY;
        // this->color = QColor(Qt::blue);
    }

    virtual CustomShape * clone() {
        return nullptr;
    }

    void changeColor(QColor color) override {
        this -> color = color;
        setBrush(QBrush(color));
        setPen(QPen(Qt::white, 2));
    }

    void saveToFile(ofstream & outFile) override {
        outFile << this -> getType() << "\n";
        outFile << this -> color.toRgb().red() << " " <<
            this -> color.toRgb().green() << " " <<
            this -> color.toRgb().blue() << "\n";
        outFile << this -> x << " " << this -> y << " " << this -> size << "\n";
    }

    virtual string getType() {
        return "";
    }

    void removeFromScene() override{
        this -> scene -> removeItem(dynamic_cast < QGraphicsItem * > (this));
    }

    QRectF getRect() {
        return boundingRect();
    }

    virtual void changeSize(QRectF rect) override {
        size = rect.height();
    }

    QPointF getMapFromScene(QPointF pos) {
        return this -> mapFromScene(pos);
    }
    bool isContains(QPointF p) override {
        // Возвращает bool содержиться ли курсор в области фигуры
        return this -> contains(p);
    }

    void setNewPos(double x, double y) override {
        prepareGeometryChange();
        this -> setPos(x, y);
    }

    virtual void showSelected() override {
        this -> setPen(QPen(Qt::red));
    }

    virtual void showUnSelected(QColor color) override {
        this -> setPen(QPen(color));
    }

    bool isFocused(QPointF scenePos) override {
        QPointF localPos = this -> getMapFromScene(scenePos);
        if (this -> isContains(localPos))
            debug("BBBBBBBBBBBBBBBBBBBBBBBBB");
        return this -> isContains(localPos);
    }

    virtual bool canScale(double mlt ) override {
        QAbstractGraphicsShapeItem * item2 =
            dynamic_cast < QAbstractGraphicsShapeItem * > (this);
        if (!item2)
            return false;

        QRectF curRect = item2 -> boundingRect().translated(item2 -> pos());
        QRectF newRect = this -> getRect();
        QRectF tmp = curRect;
        newRect.setHeight(curRect.height() * mlt);
        newRect.setWidth(curRect.width() * mlt);

        curRect.setHeight(curRect.height() * mlt);
        curRect.setWidth(curRect.width() * mlt);

        QRectF viewRect =
            view -> mapToScene(view -> viewport() -> rect()).boundingRect();
        viewRect.setTop(viewRect.top() + 20);
        viewRect.setLeft(viewRect.left() + 20);
        viewRect.setRight(viewRect.right());
        viewRect.setBottom(viewRect.bottom());
        if (viewRect.contains(curRect)) {
            return true;
        }
        return false;
    }

    void scale(double mlt) override {
        QAbstractGraphicsShapeItem * item2 =
            dynamic_cast < QAbstractGraphicsShapeItem * > (this);
        if (!item2)
            return;
        if (canScale(mlt) == false)
            return;

        QRectF newRect = this -> getRect();
        newRect.setHeight(newRect.height() * mlt);
        newRect.setWidth(newRect.width() * mlt);

        this -> changeSize(newRect);

        prepareGeometryChange();
        this -> update();
    }

    bool isShapeRectInView() override {
        // после создания Shape вызывается перед тем как изменить положение
        // фигуры

        QRectF viewRect =
            view -> mapToScene(view -> viewport() -> rect()).boundingRect();

        return viewRect.contains(this -> sceneBoundingRect());
    }

    bool canMove(double difX, double difY) override {
        QRectF itemRect = this -> mapRectToScene(this -> boundingRect());
        QRectF newItemRect = itemRect.translated(difX, difY);

        QRectF viewRect =
            view -> mapToScene(view -> viewport() -> rect()).boundingRect();

        return viewRect.contains(newItemRect);
    }

    void move(double difX, double difY) override {
        QRectF itemRect = this -> mapRectToScene(this -> boundingRect());
        QRectF newItemRect = itemRect.translated(difX, difY);

        QRectF viewRect =
            view -> mapToScene(view -> viewport() -> rect()).boundingRect();

        if (viewRect.contains(newItemRect)) {
            this -> setPos(this -> pos() + QPointF(difX, difY));
            this -> x += difX;
            this -> y += difY;
        }
    }

    void setMovable(bool val) override {
        debug("aaaa");
        this -> movable = val;
        debug("aaaa 2");
    }

    void draw() override {
        debug("BBB");
        scene -> addItem(this);

        changeColor(this -> color);

        QRectF rect = this -> getRect();
        double w = rect.width();
        double h = rect.height();
        this -> setPos(x, y);
    }

    bool ismovable() override {
        return this -> movable;
    }

    void moveInViewBack() override {
        QRectF itemRect = this -> sceneBoundingRect();
        QRectF viewRect =
            view -> mapToScene(view -> viewport() -> rect()).boundingRect();
        this -> move(0.0f, -abs(viewRect.bottom() - itemRect.bottom()));
    }

private: bool movable;
};

class MyCircleItem: public CustomShape {
public:
    string getType() override {
        return "Circle";
    }
    MyCircleItem(int posX, int posY, int size, QGraphicsView* view,
                 QGraphicsItem * parent = nullptr): CustomShape(posX, posY, size, view, parent) {
        setAcceptHoverEvents(true);

    }

    QRectF boundingRect() const override {
        return QRectF(-size / 2, -size / 2, size, size);
    }

    void paint(QPainter * painter,
               const QStyleOptionGraphicsItem * option,
               QWidget * widget) override {
        painter -> setBrush(this -> brush());
        painter -> setPen(this -> pen());
        painter -> drawEllipse(boundingRect());
    }

    IShape * loadFromFile(ifstream & inFile) override {
        debug("111");
        int r, g, b;
        inFile >> r >> g >> b;
        int x, y, size;
        inFile >> x >> y >> size;
        this -> x = x;
        this -> y = y;
        this -> size = size;
        changeColor(QColor::fromRgb(r, g, b));
        return this;
    }
};

class MySquareItem: public CustomShape {
public:
    string getType() override {
        return "Square";
    }
    MySquareItem(int posX, int posY, int size,
                 QGraphicsView* view, QGraphicsItem * parent = nullptr): CustomShape(posX, posY, size, view, parent) {
        setAcceptHoverEvents(true);
    }

    QRectF boundingRect() const override {
        return QRectF(-size / 2, -size / 2, size, size);
    }

    void paint(QPainter * painter,
               const QStyleOptionGraphicsItem * option,
               QWidget * widget) override {
        painter -> setBrush(this -> brush());
        painter -> setPen(this -> pen());
        painter -> drawRect(boundingRect());
    }

    IShape * loadFromFile(ifstream & inFile) override {
        int r, g, b;
        inFile >> r >> g >> b;
        int x, y, size;
        inFile >> x >> y >> size;
        this -> x = x;
        this -> y = y;
        this -> size = size;
        changeColor(QColor::fromRgb(r, g, b));
        return this;
    }
};

class MyTriangleItem: public CustomShape {
public:
    string getType() override {
        return "Triangle";
    }
    MyTriangleItem(int posX, int posY, int size, QGraphicsView* view,
                   QGraphicsItem * parent = nullptr): CustomShape(posX, posY, size, view, parent) {
        setAcceptHoverEvents(true);
    }

    QRectF boundingRect() const override {
        return QRectF(-size / 2, -size / 2, size, size);
    }

    void paint(QPainter * painter,
               const QStyleOptionGraphicsItem * option,
               QWidget * widget) override {
        QPolygonF polygon;
        polygon << QPointF(-size / 2, size / 2) << QPointF(size / 2, size / 2) <<
            QPointF(0, -size / 2) << QPointF(-size / 2, size / 2);
        painter -> setBrush(this -> brush());
        painter -> setPen(this -> pen());
        painter -> drawPolygon(polygon);
    }
    IShape * loadFromFile(ifstream & inFile) override {
        int r, g, b;
        inFile >> r >> g >> b;
        int x, y, size;
        inFile >> x >> y >> size;
        this -> x = x;
        this -> y = y;
        this -> size = size;
        changeColor(QColor::fromRgb(r, g, b));
        return this;
    }
};

// Класс для линий
class MyLineItem: public CustomShape {
public: string getType() override {
        return "Line";
    }

    MyLineItem(int posX, int posY, int size, QGraphicsView* view,
               QGraphicsItem * parent = nullptr): CustomShape(posX, posY, size, view, parent) {
        setAcceptHoverEvents(true);
    }

    QRectF boundingRect() const override {
        qreal currentPenW =
            pen().widthF();
        qreal padding = currentPenW / 2.0 + 2.0;
        return QRectF(-padding, -size / 2.0 - 1, 2 * padding, size + 2);
    }

    virtual void changeSize(QRectF rect) override {
        this -> size = rect.height();
    }

    void paint(QPainter * painter,
               const QStyleOptionGraphicsItem * option,
               QWidget * widget) override {
        painter -> setBrush(this -> brush());
        painter -> setPen(this -> pen());;

        painter -> drawLine(0, -size / 2, 0, size / 2);
    }

    void showSelected() override {
        // this->setBrush(QBrush(Qt::red));
        this -> setPen(QPen(Qt::red, 5));
    }

    void showUnSelected(QColor color) override {
        this -> setPen(QPen(color, 5));
    }
    IShape * loadFromFile(ifstream & inFile) override {
        int r, g, b;
        inFile >> r >> g >> b;
        int x, y, size;
        inFile >> x >> y >> size;
        this -> x = x;
        this -> y = y;
        this -> size = size;
        changeColor(QColor::fromRgb(r, g, b));
        return this;
    }
};

class MyTrapezoidItem: public CustomShape {
public:
    string getType() override {
        return "Trapezoid";
    }

    MyTrapezoidItem(int posX, int posY, int size, QGraphicsView* view,
                    QGraphicsItem * parent = nullptr): CustomShape(posX, posY, size, view, parent) {
        this -> height = size;
        setAcceptHoverEvents(true);
        this -> height = height;
        this -> size = size;
    }

    void changeSize(QRectF rect) override {
        size = rect.width();
        height = rect.height();
    }

    QRectF boundingRect() const override {
        return QRectF(-size / 2, -height / 2, size, height);
    }

    void paint(QPainter * painter,
               const QStyleOptionGraphicsItem * option,
               QWidget * widget) override {
        QPolygonF polygon;
        polygon << QPointF(-size / 2, height / 2) // Левая нижняя
                <<
            QPointF(size / 2, height / 2) // Правая нижняя
                <<
            QPointF(size / 4, -height / 2) // Верхняя левая
                <<
            QPointF(-size / 4, -height / 2); // Верхняя правая

        painter -> setBrush(this -> brush());
        painter -> setPen(this -> pen());
        painter -> drawPolygon(polygon);
    }
    IShape * loadFromFile(ifstream & inFile) override  {
        int r, g, b;
        inFile >> r >> g >> b;
        int x, y, size;
        inFile >> x >> y >> size;
        this -> x = x;
        this -> y = y;
        this -> size = size;
        this -> height = size;
        changeColor(QColor::fromRgb(r, g, b));
        return this;
    }

private: int height;
};


class ShapeCreator: public IShapeCreator {
private: string type = "None";
    ifstream * inFile = nullptr;
    QGraphicsScene * scene = nullptr;
    QGraphicsView *view = nullptr;

public: ShapeCreator(ifstream & inFile, QGraphicsView * view) {
        this -> inFile = & inFile;
        this -> scene = view->scene();
        this->view = view;
    }
    void setType(string type) {
        this -> type = type;
    }
    void setInFile(ifstream & inFile) {
        this -> inFile = & inFile;
    }
    IShape * createShape() {
        IShape * shape = nullptr;
        if (scene == nullptr)
            throw runtime_error("ShapeCreator::createShape() scene = nullptr");
        debug(type + "type");
        qDebug() << type << " " << ": loadFromFile debug2" << Qt::endl;
        if (type == "Circle") {
            shape = new MyCircleItem(0, 0, 0, view);
        } else if (type == "Square") {
            shape = new MySquareItem(0, 0, 0, view);
        } else if (type == "Line") {
            shape = new MyLineItem(0, 0, 0, view);
        } else if (type == "Trapezoid") {
            shape = new MyTrapezoidItem(0, 0, 0, view);
        } else if (type == "Triangle") {
            shape = new MyTriangleItem(0, 0, 0, view);
        } else if (type == "GROUP") {
            GroupComposite * group = new GroupComposite({}, scene);
            group -> setShapeCreator(this);
            shape = group;
        } else
            throw runtime_error("does not exit this type of shape " + type);
        debug("QQQ");
        shape -> loadFromFile( * inFile);

        shape -> draw();

        return shape;
    }
};

#endif // SHAPE_H
