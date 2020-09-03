//
// Copyright (c) 2020 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "BufferManager.h"

#include "common/AQUARIUM_ASSERT.h"

BufferManager::BufferManager()
    : mBufferPoolSize(BUFFER_POOL_MAX_SIZE), mUsedSize(0), mCount(0) {
}

BufferManager::~BufferManager() {
  destroyBufferPool();
}

bool BufferManager::resetBuffer(RingBuffer *ringBuffer, size_t size) {
  size_t index = find(ringBuffer);

  if (index >= mEnqueuedBufferList.size()) {
    return false;
  }

  size_t oldSize = ringBuffer->getSize();

  bool result = ringBuffer->reset(size);
  // If the size is larger than the ring buffer size, reset fails and the ring
  // buffer retains.
  // If the size is equal or smaller than the ring buffer size, reset success
  // and the used size need to be updated.
  if (!result) {
    return false;
  } else {
    mUsedSize = mUsedSize - oldSize + size;
  }

  return true;
}

bool BufferManager::destoryBuffer(RingBuffer *ringBuffer) {
  size_t index = find(ringBuffer);

  if (index >= mEnqueuedBufferList.size()) {
    return false;
  }

  mUsedSize -= ringBuffer->getSize();
  ringBuffer->destory();
  mEnqueuedBufferList.erase(mEnqueuedBufferList.begin() + index);

  return true;
}

size_t BufferManager::find(RingBuffer *ringBuffer) {
  size_t index = 0;
  for (auto buffer : mEnqueuedBufferList) {
    if (buffer == ringBuffer) {
      break;
    }
    index++;
  }
  return index;
}

// Flush copy commands in buffer pool
void BufferManager::flush() {
  for (auto buffer : mEnqueuedBufferList) {
    buffer->flush();
  }
}
