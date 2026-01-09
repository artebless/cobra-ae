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

use std::ffi::{c_float, c_void};
use std::ptr::null_mut;
use std::sync::atomic;
use std::{mem, ptr, thread};
use std::time::Duration;

#[allow(unused_imports)]
use libc::{free, pthread_cond_t, pthread_mutex_t, PTHREAD_COND_INITIALIZER, PTHREAD_MUTEX_INITIALIZER};
use libc::{pthread_attr_t, pthread_t, size_t, ssize_t};

#[allow(dead_code)]
static DEFAULT_PIPELINE_SIZE: usize = 2048;

// Addition for commit 3926d29. Just renaming.

#[allow(dead_code)]
static DEFAULT_PIPELINE_SLICES: usize = 4;

// Addition for commit 3926d29. Just renaming.

#[repr(C)]
pub struct SIGNAL_ {
    pub ptr: *mut c_float,
    pub signal_size: usize,
    pub flag: bool
}

// typedef enum
// {
//    SLICE_FREE,
//    SLICE_INUSE_W,
//    SLICE_VALID,
//    SLICE_INUSE_R,
//    SLICE_LISTED
// } MAIN_STATE_;

#[allow(non_camel_case_types)]
#[allow(dead_code)]
#[repr(C)]
pub enum MAIN_STATE_ {
    SLICE_FREE,
    SLICE_INUSE_W,
    SLICE_VALID,
    SLICE_INUSE_R,
    SLICE_LISTED
}

// typedef struct
// {
//    float_t left_channel[2048];
//    float_t right_channel[2048];
//    size_t fb_size;
// } __attribute__((aligned(DEFAULT_CACHE_LINE))) SIGNAL_FRAME_;

#[allow(non_camel_case_types)]
#[allow(dead_code)]
#[repr(C, align(128))]
pub struct SIGNAL_FRAME_ {
    pub left_channel: [c_float; 2048],
    pub right_channel: [c_float; 2048],
    pub fb_size: usize
}

// typedef struct
// {
//    atomic_int builder_id_uniq_;
//    atomic_int checker_id_uniq_;
//    atomic_enum main_state_;
//    atomic_int processed_;
// } SLICE_STATE_;

#[allow(non_camel_case_types)]
#[allow(dead_code)]
#[repr(C)]
pub struct SLICE_STATE_ {
    pub build_id_uniq_: atomic::AtomicI32,
    pub checker_id_uniq: atomic::AtomicI32,
    pub main_state_: atomic::AtomicI32,
    pub processed_: atomic::AtomicI32
}

#[allow(non_camel_case_types)]
#[allow(dead_code)]
#[repr(C)]
pub struct SIGNAL_PIPELINE_ {
    pub signal_frame: *mut SIGNAL_FRAME_,
    pub slice_state_arr: [SLICE_STATE_; DEFAULT_PIPELINE_SLICES]
}

unsafe extern "C" {
    fn _build_blank_wave__(signal: *mut SIGNAL_, sample_rate: c_float, t: c_float, stereo: bool);
    fn _build_sin_wave__(signal: *mut SIGNAL_, s_amplitude: c_float, s_phase: c_float,
        s_frequency: c_float, s_sample_rate: c_float, t: c_float, stereo: bool);
    fn _init_slice_state_builder__(builder_id_uniq_: usize) -> SLICE_STATE_; // Fix, due to function returning SLICE_STATE_
    fn _init_slice_state_checker__(checker_id_uniq_: usize) -> SLICE_STATE_; // Fix, due to function returning SLICE_STATE_
    fn _charge_pipeline__(pl_: *mut SIGNAL_PIPELINE_, signal_: *mut SIGNAL_FRAME_) -> *mut c_void;
    fn _read_pipeline__(pl_: *mut SIGNAL_PIPELINE_, signal_: *mut SIGNAL_FRAME_) -> *mut c_void;
    
    // fn thread_init_() and thread_cancel_() was fully removed from kernel.c
}

#[allow(dead_code)]
#[allow(unused_unsafe)]
#[allow(unused_variables)]
pub fn test_pipeline_() {
    unsafe {
        let mut num_one: SLICE_STATE_ = _init_slice_state_builder__(1);
        let mut num_two: SLICE_STATE_ = _init_slice_state_builder__(2);
        let mut num_three: SLICE_STATE_ = _init_slice_state_builder__(3);
        let mut num_four: SLICE_STATE_ = _init_slice_state_checker__(4);
        let mut left_channel: [c_float; DEFAULT_PIPELINE_SIZE] = [0.111; DEFAULT_PIPELINE_SIZE];
        let mut right_channel: [c_float; DEFAULT_PIPELINE_SIZE] = [0.222; DEFAULT_PIPELINE_SIZE];
        let mut fb_size = DEFAULT_PIPELINE_SIZE;
        let mut slice_state_arr: [SLICE_STATE_; DEFAULT_PIPELINE_SLICES] = [num_one, num_two, num_three, num_four];

        let mut frame = SIGNAL_FRAME_ {
            left_channel,
            right_channel,
            fb_size
        };

        let mut pipeline = SIGNAL_PIPELINE_ {
            signal_frame: &mut frame,
            slice_state_arr: slice_state_arr
        };
    }
}

#[warn(unused_assignments)]
pub fn test_waves_gen_() {
    unsafe {
        let mut signal = SIGNAL_ {
            ptr: null_mut(),
            signal_size: 0,
            flag: false
        };

        _build_blank_wave__(&mut signal as *mut SIGNAL_, 44110.0, 2.0, true);

        if !signal.ptr.is_null() && signal.signal_size > 0 {
            let first_var: c_float = *signal.ptr;
            println!("{}", first_var);
            println!("{}", signal.signal_size);
        }

        if signal.ptr != null_mut() {
            free(signal.ptr as *mut c_void);
            signal.ptr = null_mut();
        }

        let mut signal_two = SIGNAL_ {
            ptr: null_mut(),
            signal_size: 0,
            flag: false
        };

        _build_sin_wave__(&mut signal_two as *mut SIGNAL_, 1.0, 0.0, 440.0, 44110.0, 10.0, true);

        if !signal_two.ptr.is_null() && signal_two.signal_size > 0 {
            let slice: &[c_float] = std::slice::from_raw_parts(signal_two.ptr, signal_two.signal_size);
            for (y, &r) in slice.iter().enumerate() {
                println!("[{}] -> {}", y, r);
            }
        }

        if signal_two.ptr != null_mut() {
            free(signal_two.ptr as *mut c_void);
            signal_two.ptr = null_mut();
        }
    }
}