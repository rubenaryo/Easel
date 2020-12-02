#include "BatchRenderer.h"

#include "DeviceResources.h"

namespace Rendering {
void BatchRenderer::Init(ID3D11Device* device)
{
    for (uint32_t i = 0; i != NUM_THREADS; ++i)
    {
        device->CreateDeferredContext(0, &Workers[i].DeferredContext);
        Workers[i].FinalCommandList = NULL;
        Workers[i].Thread = std::thread();
    }

    JobQueue.reserve(MAX_PENDING_JOBS);
}
void BatchRenderer::SubmitJob(DeferredJob::DeferredFunc func, DeferredJob::DeferredArgs data)
{
    JobQueue.push_front();
    JobQueue.front() = DeferredJob(func, data);
}

void BatchRenderer::Render(DeviceResources* dr, EntityRenderer* entityRenderer)
{
    // Give all threads jobs
    for(unsigned i = 0; i != NUM_THREADS; ++i)
    {
        if (JobQueue.empty())
            break;

        DeferredJob& job = JobQueue.front();

        // The job will populate the thread's command list.
        Workers[i].Thread = std::thread(job.Job, &job.Args, dr, Workers[i].DeferredContext, entityRenderer, &Workers[i].FinalCommandList);

        JobQueue.pop_front();
    }

    // Wait for those jobs to finish
    for(unsigned i = 0; i != NUM_THREADS; ++i)
    {
        Workers[i].Thread.join();
        dr->GetContext()->ExecuteCommandList(Workers[i].FinalCommandList, FALSE);
        Workers[i].FinalCommandList->Release();
    }
}

void BatchRenderer::Shutdown()
{
    for (uint32_t i = 0; i != NUM_THREADS; ++i)
        Workers[i].DeferredContext->Release();
}

}

