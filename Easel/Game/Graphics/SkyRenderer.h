/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/9
Description : Handles the rendering of the sky backdrop
----------------------------------------------*/
#include "DXCore.h"

namespace Game
{
class Mesh;
}

namespace Graphics
{
class Material;
class Texture;
}

namespace Graphics {

class SkyRenderer
{
public:
    SkyRenderer(const Game::Mesh* pCubeMesh, const Material* pSkyMaterial, ID3D11Device* device);
    ~SkyRenderer();

    void Render(ID3D11DeviceContext* context) const;
    const Material* GetMaterial() const { return mpSkyMaterial; }

private:
    ID3D11RasterizerState*      mpRSState;
    ID3D11DepthStencilState*    mpDSState;
    
    const Game::Mesh* mpCubeMesh;
    const Material*   mpSkyMaterial;

// Delete auto generated constructors
public:
    SkyRenderer()                               = delete;
    SkyRenderer(SkyRenderer const&)             = delete;
    SkyRenderer& operator=(SkyRenderer const&)  = delete;
};

}