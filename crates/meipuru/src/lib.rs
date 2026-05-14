use std::{ffi::CString, fs};

use crate::{
    base_tag::BaseTag,
    binding::{MeipuruBaseTagBuffer, MeipuruResource},
};

pub mod base_tag;
mod binding;
mod buffer_like;

pub struct Resource {
    raw: *mut MeipuruResource,
    base_tag_buffer: Option<*mut MeipuruBaseTagBuffer>,
}

impl Drop for Resource {
    fn drop(&mut self) {
        unsafe {
            self.unload_base_tag_buffer();
            binding::meipuruFreeResource(self.raw);
        }
    }
}

impl Resource {
    pub fn read_base_tag(&mut self) -> Result<BaseTag, String> {
        if self.base_tag_buffer.is_none() {
            unsafe {
                self.load_base_tag_buffer();
            }
        }

        BaseTag::try_from(self.base_tag_buffer.unwrap())
    }

    unsafe fn load_base_tag_buffer(&mut self) {
        unsafe {
            self.unload_base_tag_buffer();
            self.base_tag_buffer = Some(binding::meipuruGetReadonlyBaseTag(self.raw));
        }
    }

    unsafe fn unload_base_tag_buffer(&mut self) {
        unsafe {
            if self.base_tag_buffer.is_some() {
                binding::meipuruFreeBaseTag(self.base_tag_buffer.unwrap());
                self.base_tag_buffer = None;
            }
        }
    }
}

pub fn load_resource(file_path: &str) -> Option<Resource> {
    if !fs::exists(file_path).unwrap_or(false) {
        return None;
    }

    let _raw = unsafe {
        binding::meipuruMakeResource(
            CString::new(file_path)
                .expect("failed to build api")
                .as_ptr(),
        )
    };

    Some(Resource {
        raw: _raw,
        base_tag_buffer: None,
    })
}
