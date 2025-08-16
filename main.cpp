#include "./ui_mainwindow.h"

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

#include "shape.h"

#include "command.h"

#include "storage.h"


class MyView: public QGraphicsView {
public:
    QColor color = QColor(QRandomGenerator::global() -> bounded(240),
                          QRandomGenerator::global() -> bounded(256),
                          QRandomGenerator::global() -> bounded(256));

    // [!] Теперь при создании MyView мы сразу создаём внутренний storage
    MyStorage * storage; // хранит все наши объекты

    MyView(QGraphicsScene * scene, MyStorage * storage,
           QWidget * parent = nullptr): QGraphicsView(scene, parent) {


        setMouseTracking(true);
        setBackgroundBrush(Qt::black);
        setScene(scene);
        setRenderHint(QPainter::Antialiasing);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        this -> storage = storage;
    }

    void setItemMode(ItemType mode) {
        this -> itemMode = mode;
    }

    void clearHistoryCommands() {
        while (!(commandsHistory.empty())) {
            auto i = commandsHistory.top();
            delete i;
            commandsHistory.pop();
        }
    }
protected:
    void resizeEvent(QResizeEvent * event) override {
        QGraphicsView::resizeEvent(event);
        for (int i = 0; i < storage -> count(); i++) {
            IShape * item = storage -> getItem(i);
            if (!item)
                continue;
            if (!(item -> isShapeRectInView())) {
                item -> moveInViewBack();
            }
        }
    }
    void keyPressEvent(QKeyEvent * event) override {
        // Удаление выбраных элементов
        bool controlPressed = event -> modifiers() & Qt::ControlModifier;
        if (event -> key() == Qt::Key_Delete) {
            storage -> clearSelected();
        }
        if (event -> key() == Qt::Key_Space && controlPressed) {
            storage -> unGroupSelected();
        } else if (event -> key() == Qt::Key_Space) {
            storage -> createGroupFromSelected();
        }

        // Увеличение масштаба
        if (event -> key() == Qt::Key_Up) {
            for (IShape * item: storage -> getSelectedItems()) {
                if (!item)
                    continue;
                ICommand * scaleCommand = new ScaleCommand(1.15);
                scaleCommand -> execute(item);
                commandsHistory.push(scaleCommand);
                update();
            }
        }
        // Уменьшение масштаба
        int delta = 4;
        if (event -> key() == Qt::Key_Down) {
            for (IShape * item: storage -> getSelectedItems()) {
                if (!item)
                    continue;
                ICommand * scaleCommand = new ScaleCommand(0.85);
                scaleCommand -> execute(item);
                commandsHistory.push(scaleCommand);
                update();
            }
        }
        ICommand * moveCommand = nullptr;
        if (event -> key() == Qt::Key_W) {
                for (IShape * item: storage -> getSelectedItems()) {
                    if (item) {
                        moveCommand = new MoveCommand(0, -delta);
                        // item->move(0, -delta, this);
                        moveCommand -> execute(item);
                        commandsHistory.push(moveCommand);
                    }
                }
        }
        if (event -> key() == Qt::Key_S) {
                for (IShape * item: storage -> getSelectedItems()) {
                    if (item) {
                        moveCommand = new MoveCommand(0, delta);
                        moveCommand -> execute(item);
                        commandsHistory.push(moveCommand);
                    }
                }
        }
        if (event -> key() == Qt::Key_A) {

                for (IShape * item: storage -> getSelectedItems()) {
                    if (item) {
                        moveCommand = new MoveCommand(-delta, 0);
                        moveCommand -> execute(item);
                        commandsHistory.push(moveCommand);
                    }
                }
        }
        if (event -> key() == Qt::Key_D) {

                for (IShape * item: storage -> getSelectedItems()) {
                    if (item) {
                        moveCommand = new MoveCommand(delta, 0);
                        moveCommand -> execute(item);
                        commandsHistory.push(moveCommand);
                    }
                }

        }
        if (event -> key() == Qt::Key_Z && controlPressed) {
            if (!(commandsHistory.empty())) {
                qDebug() << commandsHistory.size() << Qt::endl;
                ICommand * lastCommand = commandsHistory.top();
                lastCommand -> unexecute();
                commandsHistory.pop();
                delete lastCommand;
            }
        }
        if (event -> key() == Qt::Key_C) {
            for (IShape * i: storage -> getSelectedItems()) {
                i -> changeColor(color);
            }
        }
    }

    void mousePressEvent(QMouseEvent * event) override {
        QPointF scenePos = mapToScene(event -> pos());


        if (event -> button() == Qt::RightButton) {
            addShape(scenePos);
        }

        if (event -> button() == Qt::LeftButton) {
            debug("Select items");
            bool controlPressed = event -> modifiers() & Qt::ControlModifier;

            // Если Ctrl не зажат - снимаем выделение со всех
            if (!controlPressed) {
                storage -> unselectAll();
            }

            // Пытаемся выбрать объект в точке нажатия
            selectItems(scenePos);
            lastMousePos = scenePos;
        }

        QGraphicsView::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent * event) override {

        QGraphicsView::mouseMoveEvent(event);
    }

private: QPointF lastMousePos;
    ItemType itemMode = ItemType::CRCL;
    double size = 50;
    stack < ICommand * > commandsHistory;

    // IFactoryShape *factory = nullptr; // фабрика для создания фигур

    bool isItemWillBeInView(QRectF itemRect) {

        QRectF viewRect =
            this -> mapToScene(this -> viewport() -> rect()).boundingRect();

        return viewRect.contains(itemRect);
    }

    void addShape(const QPointF& position) {

        QRect newItemRect =
            QRect(position.toPoint().x(), position.toPoint().y(), size, size);

        if (isItemWillBeInView(newItemRect) == false)
            return;

        IShape* shape = ShapeCreator::shapeByStr(itemMode, this, position, size);
        if (shape==nullptr) debug("555");

        if (shape != nullptr) {
            storage -> addItem(shape);
            shape->setNewPos(position.toPoint().x(), position.toPoint().y());
            shape->changeSize(newItemRect);
            shape -> changeColor(color);
            shape -> draw();

        }
    }

    // Функция выбора объектов мышкой
    void selectItems(const QPointF & scenePos, bool selectAll = false) {

        for (int i = storage -> count() - 1; i >= 0; i--) {
            IShape * item = storage -> getItem(i);
            if (item -> isFocused(scenePos)) {
                item -> setMovable(true);
                item -> showSelected();
                if (selectAll == false)
                    return;
            }
        }
    }
};

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow: public QMainWindow {
    Q_OBJECT

public:

    MyStorage * storage;

    ~MainWindow() {
        storage -> save();
    }

    Ui::MainWindow * ui;
    MainWindow(QWidget * parent = nullptr): QMainWindow(parent) {
        ui = new Ui::MainWindow;
        ui -> setupUi(this);

        QGraphicsScene * scene = new QGraphicsScene(this);
        // scene->setSceneRect(0, 0, 700, 700);
        scene -> setSceneRect(0, 0, 1000, 1000);

        storage = new MyStorage();

        this -> view = new MyView(scene, storage, this);
        storage -> setView(view);
        QSplitter * mainSplitter = new QSplitter(Qt::Vertical, this);

        QPushButton * creatingCircles = new QPushButton("Создавать круги", this);
        connect(creatingCircles, & QPushButton::clicked,
                [this]() {
                    this -> view -> setItemMode(ItemType::CRCL);
                });

        QPushButton * creatingRects =
            new QPushButton("Создавать квадраты", this);
        connect(creatingRects, & QPushButton::clicked,
                [this]() {
                    this -> view -> setItemMode(ItemType::SQUARE);
                });

        QPushButton * selectColor = new QPushButton("Выбрать цвет", this);
        connect(selectColor, & QPushButton::clicked,
                [this]() {
                    this -> view -> color = QColorDialog::getColor();
                });

        QPushButton * creatingTriangles =
            new QPushButton("Создавать треугольники", this);
        connect(creatingTriangles, & QPushButton::clicked,
                [this]() {
                    this -> view -> setItemMode(ItemType::TRNGL);
                });

        QPushButton * creatingLines = new QPushButton("Создавать линии", this);
        connect(creatingLines, & QPushButton::clicked,
                [this]() {
                    this -> view -> setItemMode(ItemType::LINE);
                });

        QPushButton * creatingTrapezoids =
            new QPushButton("Создавать Трапеции", this);
        connect(creatingTrapezoids, & QPushButton::clicked,
                [this]() {
                    this -> view -> setItemMode(ItemType::TRPZD);
                });

        // fileLabel = new QLabel("Текущий файл: Нет", this);

        QPushButton * loadFromFile =
            new QPushButton("Открыть", this);
        connect(loadFromFile, & QPushButton::clicked,
                [this]() {
                    QString filename = QFileDialog::getOpenFileName(
                        this, tr("Открыть текстовый файл"), "",
                        tr("Текстовые файлы (*.txt);;Все файлы (*.*)"));
                    storage -> filename = filename.toStdString();
                    // fileLabel->setText("Текущий файл " + filename);
                    view -> clearHistoryCommands();
                    storage -> load();

                });

        QPushButton * saveToFile = new QPushButton("Сохранить", this);
        connect(saveToFile, &QPushButton::clicked, [this]() {
            QString filename = QFileDialog::getSaveFileName(
                this,
                tr("Сохранить файл"),
                "",
                tr("Текстовые файлы (*.txt);;Все файлы (*.*)")
                );
            if (!filename.isEmpty()) {  // Проверяем, что пользователь не отменил диалог
                storage->filename = filename.toStdString();
                storage->save();
                // Можно обновить метку с именем файла, если есть:
                // fileLabel->setText("Текущий файл: " + filename);
            }
        });

        QPushButton * clearShapes =
            new QPushButton("Удалить все фигуры с рабочей области", this);
        connect(clearShapes, & QPushButton::clicked,
                [this]() {
                    storage -> clearAll();
                });

        QWidget * centralWidget = new QWidget();
        QVBoxLayout * layout1 = new QVBoxLayout(centralWidget);

        QWidget * empty0 = new QWidget();
        QHBoxLayout * layout2 = new QHBoxLayout(empty0);

        layout2 -> addWidget(creatingCircles);
        layout2 -> addWidget(creatingRects);

        layout2 -> addWidget(creatingTriangles);
        layout2 -> addWidget(creatingLines);
        layout2 -> addWidget(creatingTrapezoids);

        // layout1->addWidget(fileLabel);
        // fileLabel->setAlignment(Qt::AlignCenter);
        layout1 -> addWidget(loadFromFile);
        layout1 -> addWidget(saveToFile);
        layout1 -> addWidget(clearShapes);
        layout1 -> addWidget(selectColor);
        // layout1->addWidget(changeFile);

        QWidget * empty = new QWidget(this);

        mainSplitter -> addWidget(view);

        mainSplitter -> addWidget(empty);

        layout1 -> addWidget(mainSplitter);
        layout1 -> addWidget(empty0);

        empty0 -> setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        // view->setMinimumSize(100, 100);

        setCentralWidget(centralWidget);
        setWindowTitle("ООП Лаба 6");
        resize(1000, 1000);
        // storage->load();
    }

protected:
private:
    QLabel * fileLabel;
    MyView * view;
};

// Точка входа
int main(int argc, char * argv[]) {
    setlocale(LC_ALL, "Russian");
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}

#include "main.moc"
