/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Entity is an instance of a mesh with a unique transform and material
----------------------------------------------*/

#include "Graphics/D3D11App.h"
#include "Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/Camera.h"
#include "Transform.h"

using namespace Graphics;
namespace Game {

class Entity
{
public:
    Entity(Mesh* a_pMesh, Material* a_pMaterial);
    Entity() = delete;
    ~Entity() = default;

    // Set constant buffer information and draw the underlying mesh
    void Draw(ID3D11DeviceContext* a_pContext, ID3D11Buffer* a_pVSCBuffer, Camera* a_pCamera);

    // Accessors
    inline Mesh*      GetMesh() const;
    inline Transform* GetTransform();
private:
    // Entity information
    Mesh*     m_pMesh;
    Material* m_pMaterial;
    Transform m_Transform;
};

}