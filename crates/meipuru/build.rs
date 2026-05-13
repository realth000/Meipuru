fn main() {
    println!("cargo:rustc-link-search=native=build/Debug");
    println!("cargo:rustc-link-lib=static=MeipuruLib");

    // println!("cargo:rustc-link-search=native=build/taglib/taglib/Debug");
    // println!("cargo:rustc-link-lib=static=Tag");
}
