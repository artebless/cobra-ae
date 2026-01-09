cmake_minimum_required(VERSION 3.20)
project(cobra_ae)
add_library(cobra_ae
        main.c
        kernel/kernel.c
        kernel/kernel.h
        dsp/dsp.c
        dsp/dsp.h
        audio/audio.c
        audio/audio.h
)
target_link_libraries(cobra_ae PRIVATE m)
