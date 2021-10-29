#[path = "./mod.rs"]
mod ruf;

use ruf::fred;

fn main() {
    println!("Hello, {}", fred::f());
}
