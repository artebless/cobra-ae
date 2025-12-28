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
use std::ptr::{null, null_mut};
use libc::{free, pthread_cond_t, pthread_mutex_t, PTHREAD_COND_INITIALIZER, PTHREAD_MUTEX_INITIALIZER};

static DEFAULT_RING_BUFFER_SIZE: usize = 2048;
#[repr(C)]
pub struct SIGNAL_ {
    pub ptr: *mut c_float,
    pub signal_size: usize,
    pub flag: bool
}

#[repr(C)]
pub struct SIGNAL_RING_B_ {
    pub b: [c_float; DEFAULT_RING_BUFFER_SIZE],
    pub r_index: usize,
    pub w_index: usize,
    pub b_mutex: pthread_mutex_t,
    pub is_b_empty: pthread_cond_t,
    pub is_b_full: pthread_cond_t,
    pub flag: i8,
}

#[link(name = "cobra_ae", kind = "static")]
unsafe extern "C" {
    fn build_blank_wave_(signal: *mut SIGNAL_, sample_rate: c_float, t: c_float);
    fn build_sin_wave_(signal: *mut SIGNAL_, s_amplitude: c_float, s_phase: c_float,
        s_frequency: c_float, s_sample_rate: c_float, t: c_float);
    fn init_ring_b_(ring_b: *mut SIGNAL_RING_B_) -> usize;
    fn charge_ring_b_(ring_b: *mut SIGNAL_RING_B_, samples_ptr: *mut SIGNAL_) -> usize;
    fn read_ring_b_(ring_b: *mut SIGNAL_RING_B_, samples_ptr: *mut SIGNAL_) -> usize;
    fn destroy_ring_b_(ring_b: *mut SIGNAL_RING_B_) -> usize;
}

pub fn test_ring_b_() {
    unsafe {
        let mut signal = SIGNAL_ {
            ptr: null_mut(),
            signal_size: 0,
            flag: false
        };

        build_sin_wave_(&mut signal as *mut SIGNAL_, 1.0, 0.0, 770.0, 44110.0, 15.0);

        let mut buff: [c_float; DEFAULT_RING_BUFFER_SIZE] = [0.0; 2048];
        let p: pthread_mutex_t = PTHREAD_MUTEX_INITIALIZER;
        let is_b_e_: pthread_cond_t = PTHREAD_COND_INITIALIZER;
        let is_b_f_: pthread_cond_t = PTHREAD_COND_INITIALIZER;
        let mut ring_b = SIGNAL_RING_B_ {
            b: buff,
            r_index: 0,
            w_index: 0,
            b_mutex: p,
            is_b_empty: is_b_e_,
            is_b_full: is_b_f_,
            flag: 0
        };

        init_ring_b_(&mut ring_b as *mut SIGNAL_RING_B_);
        let samples_in: usize = charge_ring_b_(&mut ring_b as *mut SIGNAL_RING_B_, &mut signal as *mut SIGNAL_);
        let samples_read: usize = read_ring_b_(&mut ring_b as *mut SIGNAL_RING_B_, &mut signal as *mut SIGNAL_);
        println!("{}", samples_in);
        println!("{}", samples_read);
        destroy_ring_b_(&mut ring_b as *mut SIGNAL_RING_B_);
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

        build_blank_wave_(&mut signal as *mut SIGNAL_, 44110.0, 2.0);

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

        build_sin_wave_(&mut signal_two as *mut SIGNAL_, 1.0, 0.0, 440.0, 44110.0, 10.0);

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