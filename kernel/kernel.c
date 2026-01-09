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

void *_trmp__(void *func_ptr_)
{
    function_ f = (function_)func_ptr_;
    f();
    return NULL;
}

pthread_t _spawn_thread__(function_ f)
{
    pthread_t thread;
    pthread_create(&thread, NULL, _trmp__, (void*)f);
    return thread;
}

DISPATCHER_OUT_ dispatcher_(const char buffer[DEFAULT_MAX_DISPATCHER_BUFFER_SIZE])
{
    const size_t buffer_len = strlen(buffer);
    char current_command[DEFAULT_MAX_COMMAND_SIZE];
    char current_args[DEFAULT_MAX_ARG_SIZE][DEFAULT_MAX_ARGS];

    size_t i = 0;
    size_t u = 0;
    size_t p = 0;

    while (i < buffer_len && u + 1 < DEFAULT_MAX_COMMAND_SIZE
        && buffer[i] != ' ')
    {
        current_command[u++] = buffer[i++];
    }

    current_command[u] = '\0';

    while (i < buffer_len && buffer[i] == ' ') i++;

    while (i < buffer_len && p < DEFAULT_MAX_ARGS)
    {
        size_t w = 0;
        while (i < buffer_len && buffer[i] != ' '
            && w + 1 < DEFAULT_MAX_ARG_SIZE)
        {
            current_args[p][w++] = buffer[i++];
        }
        current_args[p][w] = '\0';
        p++;

        while (i < buffer_len && buffer[i] != ' ') i++;
        while (i < buffer_len && buffer[i] == ' ') i++;
    }

    DISPATCHER_OUT_ out;
    out.command = current_command;
    out.args_c = p;
    for (size_t f = 0; f < p; f++)
    {
        strncpy(out.args[f], current_args[f], DEFAULT_MAX_ARG_SIZE - 1);
        out.args[f][DEFAULT_MAX_ARG_SIZE - 1] = '\0';
        printf("Arg -> %s \n", out.args[f]);
    }
    printf("Command -> %s \n", current_command);

    return out;
}