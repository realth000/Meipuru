use std::{env, process::exit};

use meipuru::{base_tag::BaseTag, id3v2_tag::ID3v2Tag};

fn print_usage() {
    println!(
        r#"usage:
    print base tags:
        read <path-to-file>

    print id3v2 tags:
        read --id3v2 <path-to-file>
"#
    );
}

fn parse_base_tag(file_path: &str) -> Result<BaseTag, String> {
    let mut resource = match meipuru::load_resource(file_path) {
        Some(v) => v,
        None => {
            return Err(format!("failed to load file {}", file_path));
        }
    };

    resource.read_base_tag()
}

fn parse_id3v2_tag(file_path: &str) -> Result<ID3v2Tag, String> {
    let mut resource = match meipuru::load_resource(file_path) {
        Some(v) => v,
        None => {
            return Err(format!("failed to load file {}", file_path));
        }
    };

    resource.read_id3v2_tag()
}

fn main() {
    let args = env::args().collect::<Vec<_>>();
    if args.len() < 2 {
        print_usage();
        return;
    }

    let file_path = args.last().unwrap().trim_matches(|c| c == '"');

    if args.len() == 2 {
        // Print basic tag.
        match parse_base_tag(&file_path) {
            Ok(v) => {
                println!("base_tag: {v:#?}");
                return;
            }
            Err(e) => {
                println!("failed to read base tag: {e:?}");
                exit(1)
            }
        }
    } else {
        match args[1].as_str() {
            "--id3v2" => match parse_id3v2_tag(&file_path) {
                Ok(v) => {
                    println!("id3v2_tag: {v:#?}");
                    return;
                }
                Err(e) => {
                    println!("failed to read base tag: {e:?}");
                    exit(1)
                }
            },
            v => {
                println!("unknown tag type {v:?}");
                exit(1);
            }
        }
    }
}
