use core::slice;

pub(crate) trait BufferLike {
    /// The buffer pointer
    fn buffer(&self) -> *mut u8;

    /// Buffer total size in bytes.
    fn size(&self) -> usize;
}

pub(crate) struct BufferWalker<'b, B>
where
    B: BufferLike,
{
    buffer: &'b B,

    /// Current offset.
    offset: usize,
}

impl<'b, B> BufferWalker<'b, B>
where
    B: BufferLike,
{
    pub(crate) fn new(buffer: &'b B) -> Self {
        Self { buffer, offset: 0 }
    }

    fn has_size(&self, size: usize) -> bool {
        self.buffer.size() >= self.offset + size
    }

    pub(crate) fn read_u32(&mut self) -> Option<u32> {
        if self.has_size(4) {
            let arr: &[u8; 4] = unsafe {
                // SAFETY: It is guaranteed to have 4 or more bytes to read because we are in `has_size(4)`
                slice::from_raw_parts(self.buffer.buffer().add(self.offset), 4)
                    .as_array::<4>()
                    .unwrap()
            };
            let value = Some(u32::from_le_bytes(*arr));
            self.offset += 4;
            value
        } else {
            None
        }
    }

    pub(crate) fn read_i32(&mut self) -> Option<i32> {
        self.read_u32().map(|x| x as i32)
    }

    pub(crate) fn read_offset(&mut self) -> Option<u32> {
        self.read_u32()
    }

    pub(crate) fn read_string(&mut self) -> Option<String> {
        let str_offset = match self.read_offset() {
            Some(v) => v as usize,
            None => return None,
        };
        let str_length = match self.read_u32() {
            Some(v) => v as usize,
            None => return None,
        };

        if !self.has_size(str_length) {
            return None;
        }

        unsafe {
            let str_bytes = slice::from_raw_parts(self.buffer.buffer().add(str_offset), str_length);
            // SAFETY: These string bytes brought by C side is guaranteed to be UTF-8 valid.
            let value = str::from_utf8_unchecked(str_bytes);
            Some(value.to_string())
        }
    }
}
