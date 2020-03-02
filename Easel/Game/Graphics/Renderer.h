/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Manager-level class for intelligently binding and drawing objects
----------------------------------------------*/
#ifndef RENDERER_H
#define RENDERER_H

#include "DXCore.h"
#include "Material.h"
#include "../Entity.h"
#include <unordered_map>
#include <vector>

namespace Graphics {

class Renderer final
{
public:
    Renderer();
    ~Renderer();

    void Init();

private:
    // Creates the necessary material keys within m_Map, 
    void InitMaterials();

private:
    // Maps a material
    std::unordered_map<Material*, std::vector<Game::Entity>> m_Map;



};

}

#endif