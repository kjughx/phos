use crate::prelude::*;
use crate::sync::shared::Shared;

pub trait ListNode {
    fn get_next(&self) -> Option<Box<dyn ListNode>>;
    fn get_prev(&self) -> Option<Box<dyn ListNode>>;
    fn set_next(&mut self, next: Box<dyn ListNode>);
    fn set_prev(&mut self, prev: Box<dyn ListNode>);
}

pub struct List<T> {
    head: Option<Shared<T>>,
    tail: Option<Shared<T>>,
    len: usize,
}

impl<T> Default for List<T> {
    fn default() -> Self {
        Self {
            head: None,
            tail: None,
            len: 0,
        }
    }
}

impl<T> List<T> {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn insert(&mut self, t: &T) {}
}
