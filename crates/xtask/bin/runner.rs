use std::collections::HashMap;
use std::env;
use std::fs;
use std::path::PathBuf;
use std::process::Command;

fn main() -> Result<(), i32> {
    const IN_RELEASE_MODE: bool = !cfg!(debug_assertions);

    let manifest_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap());
    let target_dir = if IN_RELEASE_MODE {
        manifest_dir.join("../../target/release")
    } else {
        manifest_dir.join("../../target/debug")
    };
    let exe_name = if cfg!(windows) {
        "meipuru.exe"
    } else {
        "meipuru"
    };

    // Copy libraries.

    let dll_map = if IN_RELEASE_MODE {
        if cfg!(windows) {
            vec![
                (
                    "..\\..\\build-release\\bindings\\taglib\\taglib\\Release\\tag.dll",
                    "tag.dll",
                ),
                (
                    "..\\..\\build-release\\bindings\\taglib\\taglib\\Release\\tag.lib",
                    "tag.lib",
                ),
                (
                    "..\\..\\build-release\\Release\\MeipuruLib.lib",
                    "MeipuruLib.lib",
                ),
                (
                    "..\\..\\build-release\\Release\\MeipuruLib.dll",
                    "MeipuruLib.dll",
                ),
            ]
        } else {
            vec![
                (
                    "../../build-release/bindings/taglib/taglib/Release/libtag.so",
                    "libtag.so",
                ),
                (
                    "../../build-release/Release/libMeipuruLib.so",
                    "libMeipuruLib.so",
                ),
            ]
        }
    } else {
        if cfg!(windows) {
            vec![
                (
                    "..\\..\\build-debug\\bindings\\taglib\\taglib\\Debug\\tag.lib",
                    "tag.lib",
                ),
                (
                    "..\\..\\build-debug\\bindings\\taglib\\taglib\\Debug\\tag.dll",
                    "tag.dll",
                ),
                (
                    "..\\..\\build-debug\\Debug\\MeipuruLib.lib",
                    "MeipuruLib.lib",
                ),
                (
                    "..\\..\\build-debug\\Debug\\MeipuruLib.dll",
                    "MeipuruLib.dll",
                ),
            ]
        } else {
            vec![
                (
                    "../../build-debug/bindings/taglib/taglib/Debug/libtag.so",
                    "libtag.so",
                ),
                (
                    "../../build-debug/Debug/libMeipuruLib.so",
                    "libMeipuruLib.so",
                ),
            ]
        }
    };

    for (src_rel, dest_name) in dll_map {
        let src = manifest_dir.join(src_rel);
        let dest = target_dir.join(dest_name);

        if src.exists() {
            fs::copy(&src, &dest).expect("[xtask] failed to sync libraries");
        } else {
            println!("[xtask] library not found: {:?}", src);
            return Err(1);
        }
    }

    // Build example binary.

    let status = Command::new("cargo")
        .args([
            "build",
            "--bin",
            "meipuru",
            if IN_RELEASE_MODE { "-r" } else { "" },
        ])
        .status()
        .expect("[xtask] failed to run cargo build");

    if !status.success() {
        return Err(1);
    }
    let exe_path = target_dir.join(exe_name);
    if !exe_path.exists() {
        println!("[xtask] target not exists: {:?}", exe_path);
        return Err(1);
    }

    let args: Vec<String> = env::args().skip(1).collect();

    if args.is_empty() {
        println!("[xtask] no file specified");
        return Err(1);
    }

    let mut cmd_envs: HashMap<&'static str, String> = HashMap::new();

    if !cfg!(windows) {
        cmd_envs.insert("LD_LIBRARY_PATH", format!("{:?}", target_dir));
    }

    let mut child = Command::new(target_dir.join(exe_name))
        .args(&args)
        .envs(&cmd_envs)
        .spawn()
        .expect("[xtask] failed to launch binary");

    let status = child.wait().expect("[xtask] failed to wait for child");
    return status
        .code()
        .map(|x| if x == 0 { Ok(()) } else { Err(x) })
        .unwrap_or_else(|| Err(1));
}
