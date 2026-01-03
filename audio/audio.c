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

#include <bits/types/cookie_io_functions_t.h>

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

ssize_t _init_pipeline__(SIGNAL_PIPELINE_* pl_)
{
    return 0;
}

/*
 * Ima trying to make the pipeline here, but right now it isn't working
 * cuz I didn't make a write / read algorithm, but rn the base is almost ready.
 */

ssize_t _charge_pipeline__(SIGNAL_PIPELINE_* pl_, SIGNAL_FRAME_* signal_)
{
    size_t buffer_size = pl_->frame->fb_size;
    size_t max_try = 4;

    for (size_t t = 0; t < DEFAULT_BUFFER_SLICES; t++)
    {
        for (size_t i = 0; i < max_try; i++)
        {
            MAIN_STATE_ maybe = atomic_load_explicit(&pl_->slice_state_arr[t].main_state_, memory_order_acquire);
            if (maybe != SLICE_FREE && maybe != SLICE_VALID)
            {
                // Nothing to do, try to lock again...
            } else {
                if (atomic_compare_exchange_strong_explicit(&pl_->slice_state_arr[t].main_state_, &maybe, SLICE_INUSE_W, memory_order_acquire, memory_order_relaxed))
                {
                    size_t written;
                    for (written = 0; written < buffer_size; written++)
                    {
                        // Write logic here, I'll make it soon.
                    }
                    atomic_store_explicit(&pl_->slice_state_arr[t].main_state_, SLICE_LISTED, memory_order_release);
                    return written;
                }
            }
        }
    }
    return 0;
};

ssize_t _read_pipeline__(SIGNAL_PIPELINE_* pl_, SIGNAL_FRAME_* signal_)
{
    size_t buffer_size = pl_->frame->fb_size;
    size_t max_try = 4;

    for (size_t t = 0; t < DEFAULT_BUFFER_SLICES; t++)
    {
        for (size_t i = 0; i < max_try; i++)
        {
            MAIN_STATE_ maybe = atomic_load_explicit(&pl_->slice_state_arr[t].main_state_, memory_order_acquire);
            if (maybe != SLICE_LISTED)
            {
                // Read comment in line [ 58 ]
            } else {
                if (atomic_compare_exchange_strong_explicit(&pl_->slice_state_arr[t].main_state_, &maybe, SLICE_INUSE_R, memory_order_acquire, memory_order_relaxed))
                {
                    size_t read;
                    for (read = 0; read < buffer_size; read++)
                    {
                        // Read logic here, I'll make it soon.
                    }
                    atomic_store_explicit(&pl_->slice_state_arr[t].main_state_, SLICE_LISTED, memory_order_release);
                    return read;
                }
            }
        }
    }
    return 0;
}

