use crate::memory::Vec;

#[derive(Clone)]
pub struct String(Vec<u8>);

impl String {
    pub fn new() -> Self {
        Self(Vec::new())
    }
}

impl Default for String {
    fn default() -> Self {
        Self::new()
    }
}

impl From<&str> for String {
    fn from(value: &str) -> Self {
        Self(value.bytes().collect())
    }
}
