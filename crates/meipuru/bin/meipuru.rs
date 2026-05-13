use std::env;

fn main() {
    let args = env::args().collect::<Vec<_>>();
    meipuru::load_resource(args.last().unwrap());
}
