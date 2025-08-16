#ifndef COMMAND_H
#define COMMAND_H

#include "IShape.h"
#include "ICommand.h"


class MoveCommand: public ICommand {
private: IShape * shape;
    int dx,
        dy;
    QGraphicsView * view = nullptr;

public: MoveCommand(int dx, int dy) {
        this -> dx = dx;
        this -> dy = dy;
    }
    void execute(IShape * shape) {
        shape -> move(dx, dy);
    }
    void unexecute() {
        shape -> move(-dx, -dy);
    }
    MoveCommand * clone() {
        return new MoveCommand(dx, dy);
    }
    ~MoveCommand() {}
};

class ScaleCommand: public ICommand {
private: IShape * shape;
    double coef;
    double invertCoef;
    QGraphicsView * view;

public:
    ScaleCommand(double coef) {
        this -> coef = coef;
        this -> invertCoef = 1 - coef;
    }
    void execute(IShape * shape) {
        shape -> scale(coef);
    }
    void unexecute() {
        shape -> scale(1 + invertCoef);
    }
    ScaleCommand * clone() {
        return new ScaleCommand(coef);
    }
    ~ScaleCommand() {}
};

#endif // COMMAND_H
