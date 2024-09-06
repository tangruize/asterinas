// SPDX-License-Identifier: MPL-2.0

use core::fmt::Display;

use alloc::format;
use ostd::{arch::timer::Jiffies, cpu::CpuSet, early_println};

use super::SyscallReturn;
use crate::{prelude::*, thread::{kernel_thread::{KernelThreadExt, ThreadOptions}, Thread}};

use ostd::sync::RwLock;

fn wait_jiffies(value: u64, _prompt: &str) {
    let mut previous = Jiffies::elapsed().as_u64();
    let ddl = previous + value;
    loop {
        let current = Jiffies::elapsed().as_u64();
        if current >= ddl {
            break;
        }
        if current - previous >= 100 {
            previous = current;
            // Thread::yield_now();
            // early_println!("Wait jiffies {}", _prompt);
        }
    }
}

fn print_jiffy<T: Display>(prompt: &T, previous: u64) -> u64 {
    let current = Jiffies::elapsed().as_u64();
    if previous == 0 {
        early_println!("Jiffy start {}: {}", prompt, current);
    } else {
        early_println!("Jiffy end {}: {} (duration: {})", prompt, current, current - previous);
    }
    current
}

fn spawn_kernel_thread_with_affinity<F>(thread_id: i32, func: F)
where F: Fn() + Send + Sync + 'static,
{
    let mut cpu_affinity = CpuSet::new_empty();
    cpu_affinity.add(thread_id as u32);
    let thread_option = ThreadOptions::new(func).cpu_affinity(cpu_affinity);
    Thread::spawn_kernel_thread(thread_option);
}

fn rwlock_read(thread_id: i32, lock: Arc<RwLock<i32>>) {
    spawn_kernel_thread_with_affinity(thread_id, move || {
        let prompt = format!("read{}", thread_id);
        // early_println!("start {}", prompt);
        wait_jiffies(1000, &prompt);
        // let previous = print_jiffy(&prompt, 0);
        let r = lock.read();
        // print_jiffy(&prompt, previous);
        drop(r);
        // early_println!("end {}", prompt);
    });
}

fn rwlock_write_downgrade(thread_id: i32, lock: Arc<RwLock<i32>>) {
    spawn_kernel_thread_with_affinity(thread_id, move || {
        let prompt = format!("downgrade{}", thread_id);
        // early_println!("start {}", prompt);
        let w = lock.write();
        wait_jiffies(5000, &prompt);
        let previous = print_jiffy(&prompt, 0);
        let r = w.downgrade();
        print_jiffy(&prompt, previous);
        drop(r);
        // early_println!("end {}", prompt);
    });
}

fn test_rwlock_downgrade_performance(cnt: i32) {
    let lock = Arc::new(RwLock::new(5));
    rwlock_write_downgrade(cnt, Arc::clone(&lock));
    for i in 1..cnt {  // set to 0 will not thread interleaving
        rwlock_read(i, Arc::clone(&lock));
    }
}

pub fn sys_aaamytest(cnt: i32, _: &Context) -> Result<SyscallReturn> {
    early_println!("Start rwlock test {}", cnt);
    test_rwlock_downgrade_performance(cnt);
    // early_println!("Bye!");
    Ok(SyscallReturn::Return(0))
}
