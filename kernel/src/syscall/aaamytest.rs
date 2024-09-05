// SPDX-License-Identifier: MPL-2.0

use core::fmt::Display;

use alloc::format;
use ostd::{arch::timer::Jiffies, early_println};

use super::SyscallReturn;
use crate::{prelude::*, thread::{kernel_thread::{KernelThreadExt, ThreadOptions}, Thread}};

use ostd::sync::RwLock;

fn wait_jiffies(value: u64, prompt: &str) {
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
            early_println!("Wait jiffies {}", prompt);
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

fn rwlock_read(thread_id: i32, lock: Arc<RwLock<i32>>) {
    Thread::spawn_kernel_thread(ThreadOptions::new(move || {
        let prompt = format!("read{}", thread_id);
        early_println!("start {}", prompt);
        wait_jiffies(1000, &prompt);
        let previous = print_jiffy(&prompt, 0);
        let r = lock.read();
        print_jiffy(&prompt, previous);
        drop(r);
        early_println!("end {}", prompt);
    })).as_kernel_thread();
}

fn test_rwlock_downgrade_performance(cnt: i32) {
    let prompt = "downgrade";
    early_println!("start {}", prompt);
    let lock = Arc::new(RwLock::new(5));
    let w = lock.write();

    for i in 0..cnt {
        rwlock_read(i, Arc::clone(&lock));
    }

    wait_jiffies(3000, &prompt);
    let previous = print_jiffy(&prompt, 0);
    let r = w.downgrade();
    print_jiffy(&prompt, previous);
    drop(r);
    early_println!("end {}", prompt);
}

pub fn sys_aaamytest(cnt: i32, _: &Context) -> Result<SyscallReturn> {
    early_println!("Aha, you created the syscall!");
    test_rwlock_downgrade_performance(cnt);
    early_println!("Bye!");
    Ok(SyscallReturn::Return(0))
}
