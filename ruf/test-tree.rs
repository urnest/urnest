
#[path="./mod.rs"] mod ruf;

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
    let orig = tree::Node::<i32> {
	value : 1,
	children : vec![
	    n(2),
	    nc(3, vec![
		nc(4, nk() ),
		nc(5, nk() )] ),
	    nc(6, vec![
		nc(3, nk() ),
		nc(3, vec![
		    nc(4, nk() ),
		    nc(5, nk() )] ),
		nc(7, nk() )] ) ] };

    let mut x = orig.clone();

    let is_ten = |value:&i32| value.eq(&10);
    let selection = x.select_by_value(&is_ten);

    assert::equal(&selection.get_selected_values(), &Vec::<i32>::new());

    let mut selection = x.select_by_value(&|value:&i32| value==&3);
    assert::equal(&selection.get_selected_values(), &vec![3,3,3]);

    let removed = selection.prune();

    let y = tree::Node::<i32> {
	value : 1,
	children : vec![
	    n(2),
	    nc(6, vec![
		nc(7, nk() )] ) ] };

    assert::equal(&x, &y);
    
    let r = vec![
	nc(3, vec![
	    nc(4, nk() ),
	    nc(5, nk() )] ),
	nc(3, nk() ),
	nc(3, vec![
	    nc(4, nk() ),
	    nc(5, nk() )] )];

    assert::equal(&removed, &r);

    let mut x = orig.clone();

    let removed = x.select_by_value(&|value:&i32| value==&2)
	.extend(&|value:&i32| value==&3).prune();
    
    let y = tree::Node::<i32> {
	value : 1,
	children : vec![
	    nc(6, vec![
		nc(7, nk() )] ) ] };

    assert::equal(&x, &y);
    
    let r = vec![
	n(2),
	nc(3, vec![
	    nc(4, nk() ),
	    nc(5, nk() )] ),
	nc(3, nk() ),
	nc(3, vec![
	    nc(4, nk() ),
	    nc(5, nk() )] ) ];

    assert::equal(&removed, &r);

}
