use crate::{
    binding::MeipuruTagBuffer,
    buffer_like::{BufferWalker, walk_on_type},
};

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

impl TryFrom<*mut MeipuruTagBuffer> for BaseTag {
    type Error = String;

    fn try_from(value: *mut MeipuruTagBuffer) -> Result<Self, Self::Error> {
        if value.is_null() {
            return Err("null tag buffer pointer".into());
        }

        let mut walker = unsafe { BufferWalker::new(&*value) };
        let year = walk_on_type!(walker, u32, "year");
        let track = walk_on_type!(walker, u32, "track");
        let album_total_track = walk_on_type!(walker, u32, "album_total_track");
        let bit_rate = walk_on_type!(walker, i32, "bit_rate");
        let sample_rate = walk_on_type!(walker, i32, "sample_rate");
        let channels = walk_on_type!(walker, i32, "channels");
        let length = walk_on_type!(walker, i32, "length");
        let file_path = walk_on_type!(walker, String, "file_path");
        let file_name = walk_on_type!(walker, String, "file_name");
        let title = walk_on_type!(walker, String, "title");
        let artist = walk_on_type!(walker, String, "artist");
        let album_title = walk_on_type!(walker, String, "album_title");
        let album_artist = walk_on_type!(walker, String, "album_artist");
        let genre = walk_on_type!(walker, String, "genre");
        let comment = walk_on_type!(walker, String, "comment");

        Ok(Self {
            year,
            track,
            album_total_track,
            bit_rate,
            sample_rate,
            channels,
            length,
            file_path,
            file_name,
            title,
            artist,
            album_title,
            album_artist,
            genre,
            comment,
        })
    }
}
