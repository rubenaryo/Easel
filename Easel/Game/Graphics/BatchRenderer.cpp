#include "BatchRenderer.h"

namespace Rendering {
void BatchRenderer::Init(ID3D11Device* device)
{
    for (uint32_t i = 0; i != NUM_THREADS; ++i)
    {
        device->CreateDeferredContext(0, &Workers[i].DeferredContext);
        Workers[i].FinalCommandList = NULL;
    }
}
void BatchRenderer::SubmitJob(RenderFunction job, void* data)
{

}

void BatchRenderer::Cleanup()
{
    for (uint32_t i = 0; i != NUM_THREADS; ++i)
        Workers[i].DeferredContext->Release();
}

}

