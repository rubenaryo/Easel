/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Interface definitions for 
IDeviceNotify - Parent of Game, for apps that use deviceresources
DeviceResources - Member of Game, encapsulates the creation and setup of device resources
----------------------------------------------*/
#ifndef DEVICERESOURCES_H
#define DEVICERESOURCES_H

#include "D3D11App.h"

namespace Graphics {

// Interface for any device that uses device resources.
// This is because it needs to be able to handle loss of device
interface IDeviceNotify
{
    virtual void OnDeviceLost() = 0;
    virtual void OnDeviceRestored() = 0;

protected:
    ~IDeviceNotify() = default;
};

class DeviceResources
{

};

}
#endif