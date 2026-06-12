//
// Created by zykov on 5/30/2026.
//

#ifndef MAXENGINE_IWINDOW_H
#define MAXENGINE_IWINDOW_H
#include "Scene.h"


class IWindow {
public:
    virtual void Open() = 0;
    virtual void Update() = 0;
    virtual void Close() const = 0;
    virtual bool IsOpen() const = 0;
    virtual void AddView(Scene& scene) = 0;
  //  virtual void* GetNativeHandle() = 0;
};


#endif //MAXENGINE_IWINDOW_H
