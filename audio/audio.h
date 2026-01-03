/*
Copyright 2025 Nazarenko Mykyta

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef AUDIO_H
#define AUDIO_H

#include <pthread.h>
#include <tgmath.h>
#include <stdatomic.h>
#include <stdint.h>
#include "../dsp/dsp.h"
#include <stddef.h>

#define DEFAULT_RING_BUFFER_SIZE 2048
#define DEFAULT_CHANNELS 2
#define DEFAULT_BUFFER_SLICES 4
#define DEFAULT_BUILDERS 3
#define DEFAULT_CHECKERS 1
#define DEFAULT_CACHE_LINE 128

typedef enum
{
    SLICE_FREE,
    SLICE_INUSE_W,
    SLICE_VALID,
    SLICE_INUSE_R,
    SLICE_LISTED
} MAIN_STATE_;

typedef struct
{
    float_t left_channel[2048];
    float_t right_channel[2048];
    size_t fb_size;
} __attribute__((aligned(DEFAULT_CACHE_LINE))) SIGNAL_FRAME_;

typedef struct
{
    atomic_int builder_id_uniq_;
    atomic_int checker_id_uniq_;
    atomic_int main_state_;
    atomic_int processed_;
} SLICE_STATE_;

typedef struct
{
    SIGNAL_FRAME_ *frame;
    SLICE_STATE_ slice_state_arr[DEFAULT_BUFFER_SLICES];
} SIGNAL_PIPELINE_;

SLICE_STATE_ _init_slice_state_builder__(size_t builder_id_uniq_);
SLICE_STATE_ _init_slice_state_checker__(size_t checker_id_uniq_);
ssize_t _init_pipeline__(SIGNAL_PIPELINE_* pl_);
ssize_t _charge_pipeline__(SIGNAL_PIPELINE_* pl_, SIGNAL_FRAME_* signal_);
ssize_t _read_pipeline__(SIGNAL_PIPELINE_* pl_, SIGNAL_FRAME_* signal_);

#endif
