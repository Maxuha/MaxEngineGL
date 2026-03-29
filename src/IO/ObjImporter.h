//
// Created by zykov on 3/27/2026.
//

#ifndef MAXENGINE_OBJIMPRORTER_H
#define MAXENGINE_OBJIMPRORTER_H
#include "../graphics/Mesh.h"

class ObjImporter {
public:
    static Mesh Import(const char * fileName);

};



#endif //MAXENGINE_OBJIMPRORTER_H
