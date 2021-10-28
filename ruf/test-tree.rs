mod tree;

use tree;

fn n(value:i32) -> Node<i32> {
    let result = Node<i32>{ value : value, children : vec![] };
    return result;
}
fn n(i32 value, Vec<Node<i32>> children){
    Node<i32>{ value, children }
}

fn main() {
    let nk : Vec<Node<i32>> = vec![];
    
    let x = Node<i32> {
	value : 1,
	children : vec![
	    n(2),
	    Node<i32>{ value : 3,
		       children : vec![
			   Node<i32>{ value : 4, children: nk },
			   Node<i32>{ value : 5, children: nk }] } ] };

    let p = Path {
	root : x,
	indices_from_root : vec![] };

    assert p.target() == &x;
}
