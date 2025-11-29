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

#ifndef KERNEL_H
#define KERNEL_H

#include <stdio.h>
#include <math.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    pthread_t thread;
    pthread_attr_t attr;
    size_t pool_id;
} THREAD_;

THREAD_ thread_init_(void *func_ptr, const size_t poolin_id);
THREAD_ thread_cancel_(const THREAD_ thread);

#endif //KERNEL_H

