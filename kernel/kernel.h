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

#define DEFAULT_MAX_ARGS 64
#define DEFAULT_MAX_DISPATCHER_BUFFER_SIZE 1024
#define DEFAULT_MAX_COMMAND_SIZE 64
#define DEFAULT_MAX_ARG_SIZE 64

typedef void (*function_)(void);

// typedef struct {
//    pthread_t thread;
//    pthread_attr_t attr;
//    size_t pool_id;
// } THREAD_;

// Not necessary.

typedef struct
{
    char *command;
    char args[DEFAULT_MAX_ARG_SIZE][DEFAULT_MAX_ARGS];
    size_t args_c;
} DISPATCHER_OUT_;

DISPATCHER_OUT_ dispatcher_(const char buffer[DEFAULT_MAX_DISPATCHER_BUFFER_SIZE]);
void *_trmp__(void *func_ptr);
pthread_t _spawn_thread__(function_ f);
#endif //KERNEL_H
