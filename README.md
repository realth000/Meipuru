# meipuru

Rust bindings for [TagLib](https://taglib.org/) v2.3.

## State

WIP

## Features

- Basic tag
  - [x] Read
  - [ ] Write
- ID3v2 tag
  - [x] Read
  - [ ] Write
- Linking TagLib
  - [x] Static linking (by default)
  - [x] Dynamic linking (opt-in by enable `dynamic` feature)

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

### Read tags

See [./examples/read.rs](./example/read.rs) for details.

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
cargo b

# Release build
cargo b -r
```

## Testing

Run rust example:

```bash
# Run example to read tag.
cargo r --example read path/to/file

# Run example to read id3v2 tag.
cargo r --exmple read --id3v2 path/to/file

# Run example to read tag with dynamic linking taglib.
cargo r --features=dynamic --example read path/to/file
```

## License

Source code in this project is licensed under MIT license.

### About TagLib

[TagLib](https://taglib.org/) is distributed under the **LGPL** and **MPL**.

- Source code, as a git submodule, is stored in [./bindings/taglib/](./bindings/taglib/) directory.
  - Upstream [taglib repo](https://github.com/taglib/taglib).
- Artifacts are compiled locally and **statically** linked by default.
  - To opt-in dynamic linking, enabl the `dynamic` feature.
