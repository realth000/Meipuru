use std::env;

fn main() {
    let profile = env::var("PROFILE")
        .map(|p| {
            if p.to_lowercase() == "release" {
                "Release"
            } else {
                "Debug"
            }
        })
        .unwrap_or("Debug");

    println!("cargo:rustc-link-search=native=target/{}", profile);
    println!("cargo:rustc-link-lib=dylib=MeipuruLib");
    println!("cargo:rustc-link-lib=dylib=Tag");
}
