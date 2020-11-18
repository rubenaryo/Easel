#ifndef BATCHRENDERER_H
#define BATCHRENDERER_H

#include "DXCore.h"

#include <thread>
#include <EASTL/bonus/fixed_ring_buffer.h>

namespace Rendering {

typedef void (*RenderFunction)(ID3D11DeviceContext* context);

struct RenderJob
{
    RenderFunction Job;
    void* Args;
};

struct EaselWorker
{
    std::thread             Thread;
    ID3D11DeviceContext*    DeferredContext;    // Owning pointer. BatchRenderer is in charge of releasing this.
    ID3D11CommandList*      FinalCommandList;   // Once the Deferred Context is done recording commands, it writes it into this list every frame.

    void Dispatch(RenderJob job);
};

#define NUM_THREADS 4U
#define MAX_PENDING_JOBS 2U * NUM_THREADS

class BatchRenderer
{
public:
    // Populates the workers by creating a deferred context for each one
    void Init(ID3D11Device* device);
    
    // Enqueues a job into the ring buffer, which the workers draw out from
    void SubmitJob(RenderFunction job, void* data);

    void Cleanup();

private:
    EaselWorker Workers[NUM_THREADS];

    eastl::fixed_ring_buffer<RenderJob, MAX_PENDING_JOBS> JobQueue;
};

}

#endif