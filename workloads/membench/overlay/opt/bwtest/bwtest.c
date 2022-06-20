// Copyright (c) 2022 Björn Gottschall
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdio.h>
#include <stdint.h>


#define KiB(x) (1024ULL * x)
#define MiB(x) (1024ULL * KiB(x))
#define GiB(x) (1024ULL * MiB(x))

#define STACK_SIZE (MiB(64))
#define STREAM_TYPE uint64_t

#define STREAM_SIZE ((uint64_t) GiB(1))


#ifdef STRIDED
#ifdef STRIDE_OFFSET
#define STRIDE_ELEMENTS (STRIDE_OFFSET / sizeof(STREAM_TYPE))
#else
#define STRIDE_ELEMENTS (64U / sizeof(STREAM_TYPE))
#endif
#else
#define STRIDE_ELEMENTS (1U)
#endif

#define STREAM_PARTITION_START ((uint64_t) (sizeof(STREAM_TYPE) * 8U * STRIDE_ELEMENTS))

#ifdef BAREMETAL
#include "firesim_encoding.h"
#define myTime() rdcycle()
#define MEMORY_BASE 0x81000000ULL
#else
#include <sys/time.h>
static struct timeval tp;
static struct timezone tz;
#define myTime() ({                                                    \
        gettimeofday(&tp, &tz);                                         \
        (((uint64_t) tp.tv_sec * 1000000) + (uint64_t) tp.tv_usec); \
    })

static STREAM_TYPE MEMORY_BASE[STACK_SIZE / sizeof(STREAM_TYPE)];
#endif

static STREAM_TYPE *streamA = (STREAM_TYPE *) MEMORY_BASE;
static STREAM_TYPE *streamB = (STREAM_TYPE *) MEMORY_BASE + (STACK_SIZE / (2 * sizeof(STREAM_TYPE)));


#define getCopyElements(memorySize, kernelLoad) ((memorySize) / (sizeof(STREAM_TYPE) * STRIDE_ELEMENTS))
#define getCopyIterations(memorySize, streamSize) (((streamSize) + (((memorySize) / STRIDE_ELEMENTS) - 1)) / ((memorySize) / STRIDE_ELEMENTS))


#define loadKernel(memorySize, streamSize) {                                           \
        uint64_t const copyElements = getCopyElements(memorySize, 1);                  \
        uint64_t const copyIterations = getCopyIterations(memorySize, streamSize);     \
        stime = myTime();                                                              \
        for (n = 0; n < copyIterations; n++) {                                         \
            for (stream = 0; stream < copyElements; stream += (8 * STRIDE_ELEMENTS)) { \
                _use += streamA[stream + (0 * STRIDE_ELEMENTS)] +                      \
                    streamA[stream + (1 * STRIDE_ELEMENTS)] +                          \
                    streamA[stream + (2 * STRIDE_ELEMENTS)] +                          \
                    streamA[stream + (3 * STRIDE_ELEMENTS)] +                          \
                    streamA[stream + (4 * STRIDE_ELEMENTS)] +                          \
                    streamA[stream + (5 * STRIDE_ELEMENTS)] +                          \
                    streamA[stream + (6 * STRIDE_ELEMENTS)] +                          \
                    streamA[stream + (7 * STRIDE_ELEMENTS)];                           \
            }                                                                          \
        }                                                                              \
        etime = myTime();                                                              \
    }

#define storeKernel(memorySize, streamSize) {                                          \
        uint64_t const copyElements = getCopyElements(memorySize, 1);                  \
        uint64_t const copyIterations = getCopyIterations(memorySize, streamSize);     \
        stime = myTime();                                                              \
        for (n = 0; n < copyIterations; n++) {                                         \
            for (stream = 0; stream < copyElements; stream += (8 * STRIDE_ELEMENTS)) { \
                streamA[stream + (0 * STRIDE_ELEMENTS)] = 1;                           \
                streamA[stream + (1 * STRIDE_ELEMENTS)] = 1;                           \
                streamA[stream + (2 * STRIDE_ELEMENTS)] = 1;                           \
                streamA[stream + (3 * STRIDE_ELEMENTS)] = 1;                           \
                streamA[stream + (4 * STRIDE_ELEMENTS)] = 1;                           \
                streamA[stream + (5 * STRIDE_ELEMENTS)] = 1;                           \
                streamA[stream + (6 * STRIDE_ELEMENTS)] = 1;                           \
                streamA[stream + (7 * STRIDE_ELEMENTS)] = 1;                           \
            }                                                                          \
        }                                                                              \
        etime = myTime();                                                              \
    }



#define loadStoreKernel(memorySize, streamSize) {                                                  \
        uint64_t const copyElements = getCopyElements(memorySize, 2);                              \
        uint64_t const copyIterations = getCopyIterations(memorySize, streamSize);                 \
        stime = myTime();                                                                          \
        for (n = 0; n < copyIterations; n++) {                                                     \
            for (stream = 0; stream < copyElements; stream += (8 * STRIDE_ELEMENTS)) {             \
                streamA[stream + (0 * STRIDE_ELEMENTS)] = streamB[stream + (0 * STRIDE_ELEMENTS)]; \
                streamA[stream + (1 * STRIDE_ELEMENTS)] = streamB[stream + (1 * STRIDE_ELEMENTS)]; \
                streamA[stream + (2 * STRIDE_ELEMENTS)] = streamB[stream + (2 * STRIDE_ELEMENTS)]; \
                streamA[stream + (3 * STRIDE_ELEMENTS)] = streamB[stream + (3 * STRIDE_ELEMENTS)]; \
                streamA[stream + (4 * STRIDE_ELEMENTS)] = streamB[stream + (4 * STRIDE_ELEMENTS)]; \
                streamA[stream + (5 * STRIDE_ELEMENTS)] = streamB[stream + (5 * STRIDE_ELEMENTS)]; \
                streamA[stream + (6 * STRIDE_ELEMENTS)] = streamB[stream + (6 * STRIDE_ELEMENTS)]; \
                streamA[stream + (7 * STRIDE_ELEMENTS)] = streamB[stream + (7 * STRIDE_ELEMENTS)]; \
            }                                                                                      \
        }                                                                                          \
        etime = myTime();                                                                          \
    }

#define loadWarmup(memorySize) { \
        uint64_t const copyElements = memorySize / sizeof(STREAM_TYPE); \
        for (stream = 0; stream < copyElements; stream++) {             \
            _use += streamA[stream];                                    \
        }                                                               \
    }

#define storeWarmup(memorySize) { \
        uint64_t const copyElements = memorySize / sizeof(STREAM_TYPE); \
        for (stream = 0; stream < copyElements; stream++) {             \
            streamA[stream] = 1;                                        \
        }                                                               \
    }

#define loadStoreWarmup(memorySize) { \
        uint64_t const copyElements = memorySize / (2 * sizeof(STREAM_TYPE));   \
        for (stream = 0; stream < copyElements; stream++) {             \
            streamA[stream] = streamB[stream];                          \
        }                                                               \
    }

int main() {
    uint64_t memorySize = STREAM_PARTITION_START;
    uint64_t stream, n;
    uint64_t stime, etime;
    uint64_t _use = 0;

    printf("partition;size;load;store;load_store\n");

    while (memorySize <= STACK_SIZE) {

#ifdef DEBUG
    printf("[DEBUG][%lu B] copyElements   %lu\n", memorySize, getCopyElements(memorySize, 1));
    printf("[DEBUG][%lu B] copyIterations %lu\n", memorySize, getCopyIterations(memorySize, STREAM_SIZE));
#endif

        printf("%lu;%lu", memorySize, STREAM_SIZE);
        loadWarmup(memorySize);
        loadKernel(memorySize, STREAM_SIZE);
        printf(";%lu", (etime-stime));

        storeWarmup(memorySize);
        storeKernel(memorySize, STREAM_SIZE);
        printf(";%lu", (etime-stime));

        loadStoreWarmup(memorySize);
        loadStoreKernel(memorySize, STREAM_SIZE);
        printf(";%lu\n", (etime-stime));
		memorySize *= 2;
    }
    printf("# [%lu]\n", _use);
    return 0;
}
