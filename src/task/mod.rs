use crate::prelude::*;

use crate::{
    cpu::Registers,
    memory::paging::{pagedirectory::PageDirectory, PAGE_ACCESS_ALL, PAGE_IS_PRESENT},
};

mod r#extern;
pub mod process;

use process::Process;

// static mut CURRENT_TASK: Global<Dyn<TaskNode>> =
//     Global::new(|| Task::new(Process::idle()), "CURRENT_TASK");

static mut TASK_HEAD: Global<Option<Dyn<TaskNode>>> = Global::new(|| None, "TASK_HEAD");
static mut TASK_TAIL: Global<Option<Dyn<TaskNode>>> = Global::new(|| None, "TASK_TAIL");

pub struct Task {
    page_directory: PageDirectory,
    registers: Registers,
    process: Process,
}

struct TaskNode {
    task: Dyn<Task>,
    next: Dyn<Task>,
    prev: Dyn<Task>,
}

impl Task {
    pub fn new(process: Process) -> Dyn<Self> {
        let page_directory = PageDirectory::new(PAGE_IS_PRESENT | PAGE_ACCESS_ALL);
        let registers = Registers::default();
        let task = Dyn::new(Self {
            page_directory,
            registers,
            process,
        });

        // let (mut current, mut head, mut tail) =
        //     unsafe { (lock!(CURRENT_TASK), lock!(TASK_HEAD), lock!(TASK_TAIL)) };
        // if head.is_none() {
        //     return task;
        // }

        // match head.inner() {
        //     Some(head) => {}
        //     None => {}
        // }

        // task_tail->next = task;
        // task->prev = task_tail;
        // task_tail = task;

        todo!()
    }
}
