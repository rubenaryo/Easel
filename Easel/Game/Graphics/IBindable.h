/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Interface for objects that get bound to the
Direct3D pipeline.
----------------------------------------------*/
#ifndef IBINDABLE_H
#define IBINDABLE_H

#include "DXCore.h"

namespace Graphics {

class IBindable
{
public:
    // Bind is the main function of IBindable.
    // Bind takes a context* and uses it to bind this resource to the pipeline.
    virtual void Bind(ID3D11DeviceContext* context) const = 0;

protected:
    IBindable()                             = default;
    IBindable(IBindable const&)             = delete;
    IBindable& operator=(const IBindable&)  = delete;
};

}

#endif