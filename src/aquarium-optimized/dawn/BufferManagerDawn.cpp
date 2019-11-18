#include "BufferManagerDawn.h"

#include "common/AQUARIUM_ASSERT.h"

RingBufferDawn::RingBufferDawn(ContextDawn* context, size_t size)
    : RingBuffer(size), mSync(true), mContext(context), mappedData(nullptr)
{
    reset(size);
}

bool RingBufferDawn::push(wgpu::Buffer &destBuffer,
                                     size_t dest_offset,
	                                 void *pixels,
                                     size_t size)
{
    mTail = mHead + size;
    if (mTail > mSize)
    {
        return false;
    }

	if (mSync)
    {
        // TODO(yizhou): The mapped buffer sync can only be used once, we should try to use map write async.
		memcpy(mBufferMappedResult.data, pixels, size);
		mBufferMappedResult.buffer.Unmap();
		mEncoder.CopyBufferToBuffer(mBufferMappedResult.buffer, mHead, destBuffer, dest_offset, size);
	}
    else
	{
		memcpy(mappedData, pixels, size);
        mStagingBuffer.Unmap();
        mEncoder.CopyBufferToBuffer(mStagingBuffer, mHead, destBuffer, dest_offset, size);
	}

    mHead = mTail;
    return true;
}

// Reset current buffer and reuse the buffer.
bool RingBufferDawn::reset(size_t size)
{
    if (size > mSize)
        return false;

    mEncoder = mContext->getDevice().CreateCommandEncoder();
    mHead    = 0;
    mTail    = 0;

    if (mSync)
    {
        mBufferMappedResult = mContext->CreateBufferMapped(
            wgpu::BufferUsage::MapWrite | wgpu::BufferUsage::CopySrc, size);
    }
    else  // buffer mapping async
    {
        // mStagingBuffer.Release();
        mStagingBuffer =
            mContext->createBuffer(size, wgpu::BufferUsage::MapWrite | wgpu::BufferUsage::CopySrc);
    }
    return true;
}

void RingBufferDawn::MapWriteCallback(WGPUBufferMapAsyncStatus status,
                                                 void *data,
                                                 uint64_t,
                                                 void *userdata)
{
    ASSERT(status == WGPUBufferMapAsyncStatus_Success);
    ASSERT(data != nullptr);
    RingBufferDawn *ringBuffer = static_cast<RingBufferDawn *>(userdata);

    wgpu::CommandBuffer copy = ringBuffer->mEncoder.Finish();
    ringBuffer->mContext->queue.Submit(1, &copy);
}

void * RingBufferDawn::MapWriteAsyncAndWait(const wgpu::Buffer &buffer)
{
    buffer.MapWriteAsync(MapWriteCallback, this);

    while (mappedData == nullptr)
    {
        mContext->WaitABit();
    }

    void *resultPointer = mappedData;
    mappedData          = nullptr;

    return resultPointer;
}

void RingBufferDawn::flush()
{
    mHead = 0;
    mTail = 0;
    if (mSync)
    {
        wgpu::CommandBuffer copy = mEncoder.Finish();
        mContext->mCommandBuffers.emplace_back(copy);
    }
    else
    {
        MapWriteAsyncAndWait(mStagingBuffer);
    }
}

void RingBufferDawn::destory()
{
    mStagingBuffer = nullptr;
    mEncoder       = nullptr;
}

// Allocate new buffer from buffer pool.
RingBufferDawn *BufferManagerDawn::allocate(size_t size)
{
    if (mUsedSize + size > mBufferPoolSize)
    {
        return nullptr;
    }
    mUsedSize += size;

    // TODO(yizhou): Refactor ring buffer allocation. Get the last one and check if the ring buffer
    // is full. If the buffer can hold extra size space, use the last one directly.
    RingBufferDawn* ringBuffer = new RingBufferDawn(mContext, size);
    mBufferPool.emplace_back(ringBuffer);

    return ringBuffer;
}
