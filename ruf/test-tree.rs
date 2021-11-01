
#[path="./mod.rs"]
mod ruf;

use ruf::tree;
use ruf::assert;

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
			   nc(4, nk() ),
			   tree::Node::<i32>{ value : 5, children: nk() }] } ] };

    let is_ten = |value:&i32| value.eq(&10);
    let selection = tree::MutableSelection::<i32>::by_value(
	&mut x,
	&is_ten);

    assert::equal(&selection.get_selected_values(), &Vec::<i32>::new());

    let selection = x.select_by_value(&|value:&i32| value==&3);
    assert::equal(&selection.get_selected_values(), &vec![3]);
}
