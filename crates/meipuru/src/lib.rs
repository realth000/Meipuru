use std::{ffi::CString, fs, ptr::null_mut};

use crate::{
    base_tag::BaseTag,
    binding::{MeipuruResource, MeipuruTagBuffer},
    id3v2_tag::ID3v2Tag,
};

pub mod base_tag;
mod binding;
mod buffer_like;
pub mod id3v2_tag;

#[derive(PartialEq, Eq)]
enum TagType {
    /// No tag loaded.
    None,

    /// Base tag.
    Base,

    /// ID3v2 tag.
    ID3v2,
}

pub struct Resource {
    raw: *mut MeipuruResource,
    tag_type: TagType,
    tag_buffer: *mut MeipuruTagBuffer,
}

impl Drop for Resource {
    fn drop(&mut self) {
        unsafe {
            self.unload_tag_buffer();
            binding::meipuruFreeResource(self.raw);
        }
    }
}

impl Resource {
    pub fn read_base_tag(&mut self) -> Result<BaseTag, String> {
        if self.tag_type == TagType::None {
            // First time load.
            unsafe {
                self.load_base_tag_buffer();
            }
        } else if self.tag_type != TagType::Base {
            // Force reload
            unsafe {
                self.unload_tag_buffer();
                self.load_base_tag_buffer();
            }
        }

        BaseTag::try_from(self.tag_buffer)
    }

    pub fn read_id3v2_tag(&mut self) -> Result<ID3v2Tag, String> {
        if self.tag_type == TagType::None {
            // First time load.
            unsafe {
                self.load_id3v2_tag_buffer();
            }
        } else if self.tag_type != TagType::ID3v2 {
            // Force reload
            unsafe {
                self.unload_tag_buffer();
                self.load_id3v2_tag_buffer();
            }
        }

        ID3v2Tag::try_from(self.tag_buffer)
    }

    unsafe fn load_base_tag_buffer(&mut self) {
        unsafe {
            self.unload_tag_buffer();
            self.tag_buffer = binding::meipuruGetReadonlyBaseTag(self.raw);
        }
        self.tag_type = TagType::Base;
    }

    unsafe fn load_id3v2_tag_buffer(&mut self) {
        unsafe {
            self.unload_tag_buffer();
            self.tag_buffer = binding::meipuruGetReadonlyID3v2Tag(self.raw);
        }
        self.tag_type = TagType::ID3v2;
    }

    unsafe fn unload_tag_buffer(&mut self) {
        unsafe {
            match self.tag_type {
                TagType::None => {
                    /* Do nothing */
                    return;
                }
                _ => binding::meipuruFreeTagBuffer(self.tag_buffer),
            }
        }
        self.tag_type = TagType::None;
    }
}

pub fn load_resource(file_path: &str) -> Option<Resource> {
    if !fs::exists(file_path).unwrap_or(false) {
        return None;
    }
    let c = CString::new(file_path).expect("failed to build api");

    let _raw = unsafe { binding::meipuruMakeResource(c.as_ptr()) };

    Some(Resource {
        raw: _raw,
        tag_type: TagType::None,
        tag_buffer: null_mut(),
    })
}
