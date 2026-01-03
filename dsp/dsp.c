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

#include "dsp.h"

const float_t DEFAULT_SIGNAL_AMPLITUDE = 1.0f;
const float_t DEFAULT_PI_CONST = 3.14159265f;

void build_blank_wave_(SIGNAL_ *signal, const float_t sample_rate, const float_t t)
{
    const size_t signal_size = sample_rate * t;
    float_t *ptr = calloc(signal_size, sizeof(float_t));
    if (!ptr)
    {
        signal->ptr = NULL;
        signal->signal_size = 0;
        signal->flag = 1;
    }
    signal->ptr = ptr;
    signal->signal_size = signal_size;
    signal->flag = 0;
}

void build_sin_wave_(SIGNAL_ *signal, float_t s_amplitude, const float_t s_phase,
const float_t s_frequency, const float_t s_sample_rate, const float_t t)
{
    if (s_amplitude == 0)
    {
        s_amplitude = DEFAULT_SIGNAL_AMPLITUDE;
    }

    const float_t s_radian_frequency = 2.0f * DEFAULT_PI_CONST * s_frequency;
    const int64_t s_samples_count = (int64_t)(s_sample_rate * t);
    const float_t delta_phase = s_radian_frequency / s_sample_rate;

    float_t *ptr = malloc(s_samples_count * sizeof(float_t));
    float_t current_phase = s_phase;

    for (size_t i = 0; i < s_samples_count; i++)
    {
        ptr[i] = s_amplitude * cosf(current_phase);
        current_phase += delta_phase;
        if (current_phase > DEFAULT_PI_CONST)
        {
            current_phase -= 2.0f * DEFAULT_PI_CONST;
        } else if ( current_phase <= -DEFAULT_PI_CONST)
        {
            current_phase += 2.0f * DEFAULT_PI_CONST;
        }
    }
    signal->ptr = ptr;
    signal->signal_size = s_samples_count;
    signal->flag = 0;
}

ssize_t init_ring_b_(SIGNAL_RING_B_* ring_b)
{
    if (!ring_b) return -1;
    ring_b->r_index = 0;
    ring_b->w_index = 0;
    ring_b->flag = 0;
    if (pthread_mutex_init(&ring_b->b_mutex, NULL) != 0) return -2;
    return 0;
}

ssize_t charge_ring_b_(SIGNAL_RING_B_ *ring_b, SIGNAL_ *samples_ptr)
{
    if (!ring_b || !samples_ptr || !samples_ptr->ptr) return (size_t)-1;
    if (pthread_mutex_lock(&ring_b->b_mutex) != 0) return (size_t)-1;
    size_t samples_in = 0;
    for (size_t i = 0; i < samples_ptr->signal_size; i++)
    {
        ring_b->b[ring_b->w_index] = samples_ptr->ptr[i];
        ring_b->w_index = (ring_b->w_index + 1) % DEFAULT_RING_BUFFER_SIZE;
        if (ring_b->w_index == ring_b->r_index)
            ring_b->r_index = (ring_b->r_index + 1) % DEFAULT_RING_BUFFER_SIZE;
        samples_in++;
    }
    ring_b->flag = 1;
    pthread_mutex_unlock(&ring_b->b_mutex);
    return samples_in;
}

ssize_t read_ring_b_(SIGNAL_RING_B_ *ring_b, SIGNAL_ *samples_ptr_out)
{
    if (!ring_b || !samples_ptr_out || !samples_ptr_out->ptr) return (size_t)-1;
    if (pthread_mutex_lock(&ring_b->b_mutex) != 0) return (size_t)-1;
    size_t samples_read = 0;
    for (size_t i = 0; i < samples_ptr_out->signal_size && ring_b->w_index != ring_b->r_index; i++)
    {
        samples_ptr_out->ptr[i] = ring_b->b[ring_b->r_index];
        ring_b->r_index = (ring_b->r_index + 1) % DEFAULT_RING_BUFFER_SIZE;
        samples_read++;
    }
    ring_b-> flag = 2;
    pthread_mutex_unlock(&ring_b->b_mutex);
    return samples_read;
}

ssize_t destroy_ring_b_(SIGNAL_RING_B_* ring_b)
{
    if (!ring_b) return -1;
    ring_b->r_index = 0;
    ring_b->w_index = 0;
    ring_b->flag = 0;
    if (pthread_mutex_destroy(&ring_b->b_mutex) != 0) return -2;
    return 0;
}

