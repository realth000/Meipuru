use std::env;

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
