#[repr(C)]
pub struct MeipuruResource {
    _p: [u8; 0],
}

#[link(name = "MeipuruLib")]
unsafe extern "C" {

    pub fn meipuruMakeResource(file_path: *const std::ffi::c_char) -> *mut MeipuruResource;
}
