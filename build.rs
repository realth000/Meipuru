use std::env;
use std::path::PathBuf;

#[cfg(target_os = "windows")]
fn setup_link_args(profile: &str, dst: &PathBuf, build_dynamic: bool) {
    // $dst/build/Debug/
    println!(
        "cargo:rustc-link-search=native={}",
        dst.join("build").join(profile).display()
    );
    // $dst/build/taglib/taglib/Debug/
    println!(
        "cargo:rustc-link-search=native={}",
        dst.join("build")
            .join("taglib")
            .join("taglib")
            .join(profile)
            .display()
    );

    if build_dynamic {
        println!("cargo:rustc-link-lib=dylib=MeipuruLib");
        println!("cargo:rustc-link-lib=dylib=Tag");
    } else {
        println!("cargo:rustc-link-lib=static=MeipuruLib");
        println!("cargo:rustc-link-lib=static=Tag");
    }
}

#[cfg(not(target_os = "windows"))]
fn setup_link_args(_profile: &str, dst: &PathBuf, build_dynamic: bool) {
    // $dst/build
    println!(
        "cargo:rustc-link-search=native={}",
        dst.join("build").display()
    );
    // $dst/lib
    println!(
        "cargo:rustc-link-search=native={}",
        dst.join("lib").display()
    );

    if build_dynamic {
        println!("cargo:rustc-link-lib=dylib=MeipuruLib");
        println!("cargo:rustc-link-lib=dylib=tag");
    } else {
        println!("cargo:rustc-link-lib=static=MeipuruLib");
        println!("cargo:rustc-link-lib=static=tag");
    }
}

fn main() {
    // Build TagLib and bindings.
    let mut bindings = cmake::Config::new("bindings");
    let build_dynamic = env::var("CARGO_FEATURE_DYNAMIC").is_ok();
    let profile = env::var("PROFILE")
        .map(|p| {
            if p.to_lowercase() == "release" {
                "Release"
            } else {
                "Debug"
            }
        })
        .unwrap_or("Debug");

    bindings.define("BUILD_TESTING", "OFF");

    if build_dynamic {
        bindings.define("MEIPURU_BUILD_DYNAMIC_LIBS", "ON");
    }

    let dst = bindings.build();

    println!("cargo:rustc-link-search=native={}", dst.display());
    setup_link_args(profile, &dst, build_dynamic);
}
