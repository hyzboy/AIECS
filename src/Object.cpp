#include "Object.h"

Object::ObjectID Object::nextObjectId = 1;

Object::Object(const std::string& name)
    : objectId(nextObjectId++), objectName(name) {
}
