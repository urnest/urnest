
#[path="./mod.rs"]
mod ruf;

use ruf::tree;

fn n(value:i32) -> tree::Node<i32> {
    let result = tree::Node::<i32>{ value : value, children : vec![] };
    return result;
}
fn nc(value:i32, children:Vec<tree::Node<i32>>) -> tree::Node<i32> {
    tree::Node::<i32>{ value, children }
}

fn nk() -> Vec<tree::Node<i32>> {
    vec![]
}

fn main() {
    let mut x = tree::Node::<i32> {
	value : 1,
	children : vec![
	    n(2),
	    tree::Node::<i32>{ value : 3,
		       children : vec![
			   tree::Node::<i32>{ value : 4, children: nk() },
			   tree::Node::<i32>{ value : 5, children: nk() }] } ] };

    let mut p = tree::Path {
	root : &mut x,
	indices_from_root : vec![] };

    assert_eq!(p.target().value, 1);
}
