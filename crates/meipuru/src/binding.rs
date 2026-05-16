use std::{
    ffi::c_char,
    marker::{PhantomData, PhantomPinned},
};

use crate::buffer_like::BufferLike;

// To represent an opaque type in C, use zero-size and marker fields.
// ref: https://github.com/rust-lang/nomicon/blob/master/src/ffi.md#representing-opaque-structs
#[repr(C)]
pub(crate) struct MeipuruResource {
    _data: (),
    _marker: PhantomData<(*mut u8, PhantomPinned)>,
}

#[repr(C)]
pub(crate) struct MeipuruTagBuffer {
    buffer: *mut u8,
    size: u32,
}

impl BufferLike for MeipuruTagBuffer {
    fn buffer(&self) -> *mut u8 {
        self.buffer
    }

    fn size(&self) -> usize {
        self.size as usize
    }
}

#[link(name = "MeipuruLib")]
unsafe extern "C" {

    pub fn meipuruMakeResource(file_path: *const c_char) -> *mut MeipuruResource;
    pub fn meipuruFreeResource(resource: *mut MeipuruResource);

    pub fn meipuruFreeTagBuffer(tag_buffer: *mut MeipuruTagBuffer);

    pub fn meipuruGetReadonlyBaseTag(resource: *mut MeipuruResource) -> *mut MeipuruTagBuffer;
    pub fn meipuruGetReadonlyID3v2Tag(tag_buffer: *mut MeipuruResource) -> *mut MeipuruTagBuffer;
}
