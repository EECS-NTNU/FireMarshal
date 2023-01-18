// Copyright (c) 2022 Björn Gottschall
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#define DEBUG

#include <stdint.h>
#include <stdio.h>

#define KiB(x) (1024ULL * x)
#define MiB(x) (1024ULL * KiB(x))
#define GiB(x) (1024ULL * MiB(x))

#ifndef STACK_SIZE
#define STACK_SIZE (KiB(128))
#endif

#define STREAM_TYPE uint64_t

#ifndef STREAM_SIZE
#define STREAM_SIZE ((uint64_t)KiB(128))
#endif

// In case one wants to only access e.g. once per cache line
#ifdef STRIDED
#ifdef STRIDE_OFFSET
#define STRIDE_ELEMENTS ((uint64_t)(STRIDE_OFFSET / sizeof(STREAM_TYPE)))
#else
#define STRIDE_ELEMENTS ((uint64_t)(64U / sizeof(STREAM_TYPE)))
#endif
#else
#define STRIDE_ELEMENTS ((uint64_t)(1U))
#endif

#ifndef STRIDE_OFFSET
#define STRIDE_OFFSET 8
#endif

#ifdef ONE_LOOP
#define STREAM_PARTITION_START STACK_SIZE

#else
#define STREAM_PARTITION_START                                                 \
  ((uint64_t)(sizeof(STREAM_TYPE) * 8U * STRIDE_ELEMENTS * 2))
#endif

#ifdef BAREMETAL
#include "firesim_encoding.h"
#define myTime() rdcycle()
#define MEMORY_BASE 0x81000000ULL
#else
#include <sys/time.h>
static struct timeval tp;
static struct timezone tz;
#define myTime()                                                               \
  ({                                                                           \
    gettimeofday(&tp, &tz);                                                    \
    (((uint64_t)tp.tv_sec * 1000000) + (uint64_t)tp.tv_usec);                  \
  })

static STREAM_TYPE MEMORY_BASE[STACK_SIZE / sizeof(STREAM_TYPE)];
#endif

static STREAM_TYPE *streamA = (STREAM_TYPE *)MEMORY_BASE;
static STREAM_TYPE *streamB =
    (STREAM_TYPE *)MEMORY_BASE + (STACK_SIZE / (2 * sizeof(STREAM_TYPE)));

#define getCopyElements(memorySize, kernelLoad)                                \
  ((memorySize) / (kernelLoad * sizeof(STREAM_TYPE)))
#ifdef ONE_LOOP
#define getCopyIterations(memorySize, kernelLoad)                              \
  (((STREAM_SIZE / sizeof(STREAM_TYPE))) /                                     \
   getCopyElements(memorySize, kernelLoad))
#else
#define getCopyIterations(memorySize, kernelLoad)                              \
  (((STREAM_SIZE / sizeof(STREAM_TYPE)) * STRIDE_ELEMENTS) /                   \
   getCopyElements(memorySize, kernelLoad))
#endif

#define loadKernel(memorySize)                                                 \
  {                                                                            \
    uint64_t const copyElements = getCopyElements(memorySize, 1);              \
    uint64_t const copyIterations = getCopyIterations(memorySize, 1);          \
    stime = myTime();                                                          \
    for (n = 0; n < copyIterations; n++) {                                     \
      for (stream = 0; stream < copyElements;                                  \
           stream += (8 * STRIDE_ELEMENTS)) {                                  \
        _use += streamA[stream + (0 * STRIDE_ELEMENTS)] +                      \
                streamA[stream + (1 * STRIDE_ELEMENTS)] +                      \
                streamA[stream + (2 * STRIDE_ELEMENTS)] +                      \
                streamA[stream + (3 * STRIDE_ELEMENTS)] +                      \
                streamA[stream + (4 * STRIDE_ELEMENTS)] +                      \
                streamA[stream + (5 * STRIDE_ELEMENTS)] +                      \
                streamA[stream + (6 * STRIDE_ELEMENTS)] +                      \
                streamA[stream + (7 * STRIDE_ELEMENTS)];                       \
      }                                                                        \
    }                                                                          \
    etime = myTime();                                                          \
  }

#define loadKernelNoAdd(memorySize)                                            \
  {                                                                            \
    uint64_t const copyElements = getCopyElements(memorySize, 1);              \
    uint64_t const copyIterations = getCopyIterations(memorySize, 1);          \
    STREAM_TYPE *stream_base = streamA;                                        \
    int _tmp = 0;                                                              \
    stime = myTime();                                                          \
    for (n = 0; n < copyIterations; n++) {                                     \
      for (stream = 0; stream < copyElements;                                  \
           stream += (8 * STRIDE_ELEMENTS)) {                                  \
        asm volatile("ld %0, %4(%2)\n"                                         \
                     "ld %0, %5(%2)\n"                                         \
                     "ld %0, %3(%2)\n"                                         \
                     "ld %0, %6(%2)\n"                                         \
                     "ld %0, %7(%2)\n"                                         \
                     "ld %0, %8(%2)\n"                                         \
                     "ld %0, %9(%2)\n"                                         \
                     "ld %0, %10(%2)\n"                                        \
                     "addi %1, %1, %11"                                        \
                     : "=r"(_tmp), "=r"(stream_base)                           \
                     : "r"(stream_base), "i"(STRIDE_OFFSET * 0),               \
                       "i"(STRIDE_OFFSET * 1), "i"(STRIDE_OFFSET * 2),         \
                       "i"(STRIDE_OFFSET * 3), "i"(STRIDE_OFFSET * 4),         \
                       "i"(STRIDE_OFFSET * 5), "i"(STRIDE_OFFSET * 6),         \
                       "i"(STRIDE_OFFSET * 7),                                 \
                       "i"(STRIDE_OFFSET * sizeof(STREAM_TYPE)));              \
      }                                                                        \
      stream_base = streamA;                                                   \
    }                                                                          \
    etime = myTime();                                                          \
  }

#define storeKernel(memorySize)                                                \
  {                                                                            \
    uint64_t const copyElements = getCopyElements(memorySize, 1);              \
    uint64_t const copyIterations = getCopyIterations(memorySize, 1);          \
    stime = myTime();                                                          \
    for (n = 0; n < copyIterations; n++) {                                     \
      for (stream = 0; stream < copyElements;                                  \
           stream += (8 * STRIDE_ELEMENTS)) {                                  \
        streamA[stream + (0 * STRIDE_ELEMENTS)] = 1;                           \
        streamA[stream + (1 * STRIDE_ELEMENTS)] = 1;                           \
        streamA[stream + (2 * STRIDE_ELEMENTS)] = 1;                           \
        streamA[stream + (3 * STRIDE_ELEMENTS)] = 1;                           \
        streamA[stream + (4 * STRIDE_ELEMENTS)] = 1;                           \
        streamA[stream + (5 * STRIDE_ELEMENTS)] = 1;                           \
        streamA[stream + (6 * STRIDE_ELEMENTS)] = 1;                           \
        streamA[stream + (7 * STRIDE_ELEMENTS)] = 1;                           \
      }                                                                        \
    }                                                                          \
    etime = myTime();                                                          \
  }

#define loadStoreKernel(memorySize)                                            \
  {                                                                            \
    uint64_t const copyElements = getCopyElements(memorySize, 2);              \
    uint64_t const copyIterations = getCopyIterations(memorySize, 2);          \
    stime = myTime();                                                          \
    for (n = 0; n < copyIterations; n++) {                                     \
      for (stream = 0; stream < copyElements;                                  \
           stream += (8 * STRIDE_ELEMENTS)) {                                  \
        streamA[stream + (0 * STRIDE_ELEMENTS)] =                              \
            streamB[stream + (0 * STRIDE_ELEMENTS)];                           \
        streamA[stream + (1 * STRIDE_ELEMENTS)] =                              \
            streamB[stream + (1 * STRIDE_ELEMENTS)];                           \
        streamA[stream + (2 * STRIDE_ELEMENTS)] =                              \
            streamB[stream + (2 * STRIDE_ELEMENTS)];                           \
        streamA[stream + (3 * STRIDE_ELEMENTS)] =                              \
            streamB[stream + (3 * STRIDE_ELEMENTS)];                           \
        streamA[stream + (4 * STRIDE_ELEMENTS)] =                              \
            streamB[stream + (4 * STRIDE_ELEMENTS)];                           \
        streamA[stream + (5 * STRIDE_ELEMENTS)] =                              \
            streamB[stream + (5 * STRIDE_ELEMENTS)];                           \
        streamA[stream + (6 * STRIDE_ELEMENTS)] =                              \
            streamB[stream + (6 * STRIDE_ELEMENTS)];                           \
        streamA[stream + (7 * STRIDE_ELEMENTS)] =                              \
            streamB[stream + (7 * STRIDE_ELEMENTS)];                           \
      }                                                                        \
    }                                                                          \
    etime = myTime();                                                          \
  }

#define loadWarmup(memorySize)                                                 \
  {                                                                            \
    uint64_t const copyElements = getCopyElements(memorySize, 1);              \
    for (stream = 0; stream < copyElements; stream += STRIDE_ELEMENTS) {       \
      _use += streamA[stream];                                                 \
    }                                                                          \
  }

#define storeWarmup(memorySize)                                                \
  {                                                                            \
    uint64_t const copyElements = getCopyElements(memorySize, 1);              \
    for (stream = 0; stream < copyElements; stream += STRIDE_ELEMENTS) {       \
      streamA[stream] = 1;                                                     \
    }                                                                          \
  }

#define loadStoreWarmup(memorySize)                                            \
  {                                                                            \
    uint64_t const copyElements = getCopyElements(memorySize, 2);              \
    for (stream = 0; stream < copyElements; stream += STRIDE_ELEMENTS) {       \
      streamA[stream] = streamB[stream];                                       \
    }                                                                          \
  }

int main() {
  uint64_t memorySize = STREAM_PARTITION_START;
  uint64_t stream, n;
  uint64_t stime, etime;
  uint64_t _use = 0;

  printf("partition;size;load;\n");

#ifndef ONE_LOOP
  while (memorySize <= STACK_SIZE) {
#endif
#ifdef DEBUG
    printf("[DEBUG] stacksize [%llu B]\n", STACK_SIZE);
    printf("[DEBUG][%lu B] strideElements %lu\n", memorySize, STRIDE_ELEMENTS);
    printf("[DEBUG][%lu B] copyElements   %lu\n", memorySize,
           getCopyElements(memorySize, 1));
    printf("[DEBUG][%lu B] copyIterations %lu\n", memorySize,
           getCopyIterations(memorySize, 1));
#endif

    printf("%lu;%lu", memorySize, STREAM_SIZE);
    loadWarmup(memorySize);
    loadKernelNoAdd(memorySize);
    printf(";%lu", (etime - stime));

    /*
    printf("%lu;%lu", memorySize, STREAM_SIZE);
    loadWarmup(memorySize);
    loadKernel(memorySize);
    printf(";%lu", (etime - stime));
    */

    /*
    storeWarmup(memorySize);
    storeKernel(memorySize);
    printf(";%lu", (etime - stime));

    loadStoreWarmup(memorySize);
    loadStoreKernel(memorySize);
    printf(";%lu\n", (etime - stime));
    */

#ifndef ONE_LOOP
    memorySize *= 2;
  }
  printf("# [%lu]\n", _use);
#endif
  printf("Done\n");
  return 0;
}
