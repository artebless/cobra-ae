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

#include "kernel/kernel.h"
#include "dsp/dsp.h"

int main() {
    SIGNAL_ blank_wave = build_blank_wave(120, 41100);
    printf("%lu", blank_wave.signal_size);
    free(blank_wave.ptr);
    blank_wave.ptr = NULL;

    SIGNAL_ sin_wave = build_sin_wave(1.0f, 0.0f, 110.0f, 41100.0f, 5.0f);
    for (size_t i = 0; i < sin_wave.signal_size; i++)
    {
        printf("%lu -> %f \n", i, sin_wave.ptr[i]);
    }
    free(sin_wave.ptr);
    sin_wave.ptr = NULL;
    return 0;
}
