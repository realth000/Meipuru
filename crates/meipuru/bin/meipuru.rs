use std::{env, process::exit};

fn main() {
    let args = env::args().collect::<Vec<_>>();
    let resource_path = args.last().unwrap();
    let mut resource = match meipuru::load_resource(resource_path) {
        Some(v) => v,
        None => {
            println!("failed to load file {}", resource_path);
            exit(1);
        }
    };

    match resource.read_base_tag() {
        Ok(v) => println!("base_tag: {v:#?}"),
        Err(e) => println!("failed to read base tag: {e:?}"),
    };
}
