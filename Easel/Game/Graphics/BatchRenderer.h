#ifndef BATCHRENDERER_H
#define BATCHRENDERER_H

#include "DXCore.h"

#include <thread>
#include <EASTL/bonus/ring_buffer.h>

namespace Rendering
{
struct ConstantBufferBindPacket;
struct InstancedDrawContext;

class DeviceResources;
class EntityRenderer;
}

namespace Rendering {

struct DeferredJob
{
    struct DeferredArgs
    {
        InstancedDrawContext* pass;
        const ConstantBufferBindPacket* CameraPacket;
        const ConstantBufferBindPacket* LightPacket;
    };
    typedef void (*DeferredFunc)(DeferredArgs* args, DeviceResources* dr, ID3D11DeviceContext* def_context, EntityRenderer* entityRenderer, ID3D11CommandList** out_cmdList);

    DeferredFunc Job;
    DeferredArgs Args;

    DeferredJob() {};
    DeferredJob(DeferredFunc _job, DeferredArgs _args) : Job(_job), Args(_args) {}
};


struct EaselWorker
{
    std::thread             Thread;
    ID3D11DeviceContext*    DeferredContext;    // Owning pointer. BatchRenderer is in charge of releasing this.
    ID3D11CommandList*      FinalCommandList;   // Once the Deferred Context is done recording commands, it writes it into this list every frame.

    //void Dispatch(RenderJob job);
};

#define NUM_THREADS 2U
#define MAX_PENDING_JOBS 2U * NUM_THREADS

class BatchRenderer
{
public:
    // Populates the workers by creating a deferred context for each one
    void Init(ID3D11Device* device);
    
    // Enqueues a job into the ring buffer, which the workers draw out from
    void SubmitJob(DeferredJob::DeferredFunc func, DeferredJob::DeferredArgs data);

    // Executes the jobs in the ring buffer
    void Render(DeviceResources* dr, EntityRenderer* entityRenderer);

    void Shutdown();

private:
    EaselWorker Workers[NUM_THREADS];

    eastl::ring_buffer<DeferredJob> JobQueue;
};

}

#endif