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

#include "kernel.h"

#define THREAD_POOL_SIZE = 8;

static pthread_mutex_t kernel_mutex = PTHREAD_MUTEX_INITIALIZER;
volatile int16_t thread_n = 0;

THREAD_ thread_init_(void *func_ptr, const size_t poolin_id)
{
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&thread, &attr, func_ptr, NULL);
    const THREAD_ build = {
        thread,
        attr,
        poolin_id
    };
    pthread_mutex_lock(&kernel_mutex);
    thread_n += 1;
    printf("%i / ID - %lu \n", thread_n, poolin_id);
    pthread_mutex_unlock(&kernel_mutex);
    return build;
}

THREAD_ thread_cancel_(const THREAD_ thread)
{
    const size_t thread_cancel = pthread_cancel(thread.thread);
    if (thread_cancel != 0)
    {
        perror("Panic!");
    }
    else
    {
        pthread_cancel(thread.thread);
        pthread_mutex_lock(&kernel_mutex);
        thread_n -= 1;
        printf("%i \n", thread_n);
        pthread_mutex_unlock(&kernel_mutex);
    }
}
