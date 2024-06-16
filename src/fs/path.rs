use crate::memory::Vec;

pub struct _Path<'a>(Vec<&'a str>);

pub struct Path<'a> {
    disk_id: Option<usize>, // If this is None, the path is empty or invalid
    parts: _Path<'a>,
}

impl<'a> Path<'a> {
    pub fn new(path: &'a str) -> Self {
        let disk_id = match path.chars().nth(0) {
            Some('0') => Some(0),
            _ => None,
        };

        let parts = path.split('/').collect();

        Self {
            disk_id,
            parts: _Path(parts),
        }
    }
}
