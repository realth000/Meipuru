# meipuru

Rust bindings for TagLib v2.3.

## State

WIP

## Features

- Basic tag.
  - [x] Read
  - [ ] Write
- ID3v2 tag.
  - [ ] Read
  - [ ] Write

where base tag is

```rust
#[derive(Debug)]
pub struct BaseTag {
    pub year: u32,
    pub track: u32,
    pub album_total_track: u32,
    pub bit_rate: i32,
    pub sample_rate: i32,
    pub channels: i32,
    pub length: i32,

    pub file_path: String,
    pub file_name: String,
    pub title: String,
    pub artist: String,
    pub album_title: String,
    pub album_artist: String,
    pub genre: String,
    pub comment: String,
}
```

## Example

See [./crates/meipuru/bin/meipuru.rs](./crates/meipuru/bin/meipuru.rs) for details.

```rust
let mut resource = match meipuru::load_resource(file_path) {
    Some(v) => v,
    None => {
        println!("failed to load file {}", file_path);
        return;
    }
};

match resource.read_base_tag() {
    Ok(v) => println!("base_tag: {v:#?}"),
    Err(e) => println!("failed to read base tag: {e:?}"),
};
```

output

```console
base_tag: BaseTag {
    year: 2026,
    track: 2,
    album_total_track: 10,
    bit_rate: 131,
    sample_rate: 48000,
    channels: 2,
    length: 253,
    file_path: "./test_data/test1.mp3",
    file_name: "test1.mp3",
    title: "星降る海",
    artist: "Aqu3ra / 月見ヤチヨ(cv.早見沙織)",
    album_title: "超かぐや姫！",
    album_artist: "Aqu3ra",
    genre: "Music",
    comment: "",
}
```

## Build

```bash
# Debug build
cmake --preset debug
cmake --build --preset debug
cargo r --bin runner

# Release build
cmake --preset release
cmake --build --preset release
cargo r -r --bin runner
```

## Testing

Run rust example:

```bash
# Run in debug mode
cargo r --bin runner <path/to/mp3>

# Run in release mode
cargo r -r --bin runner <path/to/mp3>
```

## License

Source code in this project is licensed under MIT license, except code in [./bindings/taglib](./bindings/taglib/).

This project uses [TagLib](https://taglib.org/) which is licensed under LGPL and MPL. TagLib artifacts are built from [this repo](https://github.com/realth000/taglib/) and dynamically linked.
