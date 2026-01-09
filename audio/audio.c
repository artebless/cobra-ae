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

#include "audio.h"

#include <stdio.h>

SLICE_STATE_ _init_slice_state_builder__(size_t builder_id_uniq_)
{
    SLICE_STATE_ uniq_builder = {
        builder_id_uniq_,
        -1,
        SLICE_FREE
    };
    return uniq_builder;
}

SLICE_STATE_ _init_slice_state_checker__(size_t checker_id_uniq_)
{
    SLICE_STATE_ uniq_checker = {
        -1,
        checker_id_uniq_,
        SLICE_FREE
    };
    return uniq_checker;
}

void *_charge_pipeline__(SIGNAL_PIPELINE_* pl_, SIGNAL_FRAME_* signal_)
{
    size_t buffer_size = pl_->frame->fb_size;
    size_t max_try = 4;

    for (size_t t = 0; t < DEFAULT_PIPELINE_SLICES; t++)
    {
        for (size_t i = 0; i < max_try; i++)
        {
            MAIN_STATE_ maybe = atomic_load_explicit(&pl_->slice_state_arr[t].main_state_, memory_order_acquire);
            if (maybe != SLICE_FREE && maybe != SLICE_LISTED)
            {
                // Nothing to do, try to lock again...
                // If i == max_try -> try to lock another slice.
            } else {
                if (atomic_compare_exchange_strong_explicit(&pl_->slice_state_arr[t].main_state_, &maybe, SLICE_INUSE_W, memory_order_acquire, memory_order_relaxed))
                {
                    size_t need_to_write = DEFAULT_PIPELINE_SIZE / DEFAULT_PIPELINE_SLICES;
                    size_t slice_in = need_to_write * t;
                    size_t slice_end = slice_in + need_to_write;
                    size_t written;
                    for (written = slice_in; written < slice_end; written++)
                    {
                        pl_->frame->left_channel[written] = signal_->left_channel[written];
                        pl_->frame->right_channel[written] = signal_->right_channel[written];
                        atomic_fetch_add_explicit(&pl_->slice_state_arr[t].processed_, 1, memory_order_release);
                    }
                    atomic_store_explicit(&pl_->slice_state_arr[t].main_state_, SLICE_VALID, memory_order_release);
                    return NULL;
                }
            }
        }
    }
    return NULL;
};

void *_read_pipeline__(SIGNAL_PIPELINE_* pl_, SIGNAL_FRAME_* signal_)
{
    size_t buffer_size = pl_->frame->fb_size;
    size_t max_try = 4;

    for (size_t t = 0; t < DEFAULT_PIPELINE_SLICES; t++)
    {
        for (size_t i = 0; i < max_try; i++)
        {
            MAIN_STATE_ maybe = atomic_load_explicit(&pl_->slice_state_arr[t].main_state_, memory_order_acquire);
            if (maybe != SLICE_VALID)
            {
                // Read comment in line [ 58 and 59 ]
            } else {
                if (atomic_compare_exchange_strong_explicit(&pl_->slice_state_arr[t].main_state_, &maybe, SLICE_INUSE_R, memory_order_acquire, memory_order_relaxed))
                {
                    size_t need_to_read = DEFAULT_PIPELINE_SIZE / DEFAULT_PIPELINE_SLICES;
                    size_t slice_in = need_to_read * t;
                    size_t slice_end = slice_in + need_to_read;
                    size_t read;
                    for (read = slice_in; read < slice_end; read++)
                    {
                        signal_->left_channel[read] = pl_->frame->left_channel[read];
                        signal_->right_channel[read] = pl_->frame->right_channel[read];
                        atomic_fetch_add_explicit(&pl_->slice_state_arr[t].processed_, 1, memory_order_release);
                    }
                    atomic_store_explicit(&pl_->slice_state_arr[t].main_state_, SLICE_LISTED, memory_order_release);
                    return NULL;
                }
            }
        }
    }
    return NULL;
}
