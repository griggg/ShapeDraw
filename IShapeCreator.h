#ifndef ISHAPECREATOR_H
#define ISHAPECREATOR_H

#include "IShape.h"

class IShapeCreator {
public: virtual void setType(std::string type) = 0;
    virtual void setInFile(std::ifstream & inFile) = 0;
    virtual IShape * createShape() = 0;
    virtual~IShapeCreator() {};

};

#endif // ISHAPECREATOR_H
