use crate::{string::String, trace, Vec};

pub struct Path<'a> {
    pub disk_id: Option<usize>, // If this is None, the path is empty or invalid
    parts: Vec<&'a str>,
}

impl<'a> Path<'a> {
    pub fn new(path: &'a str) -> Self {
        let disk_id = match path.chars().nth(0) {
            Some('0') => Some(0),
            _ => None,
        };

        let parts = path[2..]
            .split('/')
            .filter(|part| !part.is_empty())
            .collect();

        trace!("{:?}", parts);

        Self { disk_id, parts }
    }
    pub fn join(&mut self, _other: Path) {
        todo!()
    }
    pub fn parts(&self) -> &Vec<&'a str> {
        &self.parts
    }
}

impl<'a> From<&'a [u8]> for Path<'a> {
    fn from(_value: &'a [u8]) -> Self {
        todo!()
    }
}

pub struct PathBuf(String);

impl PathBuf {
    pub fn new(_path: &String) -> Self {
        todo!()
    }
}