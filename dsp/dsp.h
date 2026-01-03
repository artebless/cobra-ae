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

#ifndef DSP_H
#define DSP_H

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <tgmath.h>
#include <stdatomic.h>

#define DEFAULT_RING_BUFFER_SIZE 2048

typedef struct
{
    float_t *ptr;
    size_t signal_size;
    bool flag;
} SIGNAL_;

typedef struct
{
    float_t b[DEFAULT_RING_BUFFER_SIZE];
    _Atomic size_t r_index;
    _Atomic size_t  w_index;
    pthread_mutex_t b_mutex;
    pthread_cond_t is_b_empty;
    pthread_cond_t is_b_full;
    int8_t flag;
} SIGNAL_RING_B_;

void build_blank_wave_(SIGNAL_ *signal, const float_t sample_rate, const float_t t);
void build_sin_wave_(SIGNAL_ *signal, float_t s_amplitude, const float_t s_phase,
const float_t s_frequency, const float_t s_sample_rate, const float_t t);
ssize_t init_ring_b_(SIGNAL_RING_B_* ring_b);
ssize_t charge_ring_b_(SIGNAL_RING_B_ *ring_b, SIGNAL_ *samples_ptr);
ssize_t read_ring_b_(SIGNAL_RING_B_ *ring_b, SIGNAL_ *samples_ptr_out);
ssize_t destroy_ring_b_(SIGNAL_RING_B_* ring_b);

#endif //DSP_H
