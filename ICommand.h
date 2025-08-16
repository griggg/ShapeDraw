#ifndef ICOMMAND_H
#define ICOMMAND_H

#include "IShape.h"

class ICommand {
public: virtual void execute(IShape * shape) = 0;
    virtual void unexecute() = 0;
    virtual ICommand * clone() = 0;
    virtual~ICommand() {}
};

#endif // ICOMMAND_H
