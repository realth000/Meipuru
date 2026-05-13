use std::ffi::CString;

mod binding;

pub fn load_resource(file_path: &str) -> u64 {
    unsafe {
        binding::meipuruMakeResource(
            CString::new(file_path)
                .expect("failed to build api")
                .as_ptr(),
        );
    }
    0
}
