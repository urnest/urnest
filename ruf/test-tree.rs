
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

fn main() {
    let orig = tree::Node::<i32> {
	value : 1,
	children : vec![
	    n(2),
	    nc(3, vec![
		n(4),
		n(5)] ),
	    nc(6, vec![
		n(3),
		nc(3, vec![
		    n(4),
		    n(5)] ),
		n(7)] ) ] };

    {
	let mut x = orig.clone();
	
	let is_ten = |value:&i32| value.eq(&10);
	let selection = x.select_by_value(&is_ten);
	
	assert::equal(&selection.get_selected_values(), &Vec::<i32>::new());
	
	let mut selection = x.select_by_value(&|v| v==&3);
	assert::equal(&selection.get_selected_values(), &vec![3,3,3]);
	
	let removed = selection.prune();
	
	let y = tree::Node::<i32> {
	    value : 1,
	    children : vec![
		n(2),
		nc(6, vec![
		    n(7)] ) ] };
	
	assert::equal(&x, &y);
	
	let r = vec![
	    nc(3, vec![
		n(4),
		n(5)] ),
	    n(3),
	    nc(3, vec![
		n(4),
		n(5)] )];
	
	assert::equal(&removed, &r);
	
	x.select_by_value(&|v| v==&2).prune();
	let y = tree::Node::<i32> {
	    value : 1,
	    children : vec![
		nc(6, vec![
		    n(7)] ) ] };
	
	assert::equal(&x, &y);
    }

    // select_by_value
    {
	let mut x = orig.clone();
	
	let removed = x.select_by_value(&|v| v==&2)
	    .extend_by_value(&|v| v==&3).prune();
	
	let y = tree::Node::<i32> {
	    value : 1,
	    children : vec![
		nc(6, vec![
		    n(7)] ) ] };
	
	assert::equal(&x, &y);
	
	let r = vec![
	    n(2),
	    nc(3, vec![
		n(4),
		n(5)] ),
	    n(3),
	    nc(3, vec![
		n(4),
		n(5)] ) ];
	
	assert::equal(&removed, &r);
    }

    // select_by_path using index
    {
	let mut x = orig.clone();
	
	let removed = x.select_by_path(
	    &|_ancestors, path, _node|
	    tree::Disposition{ select: path == &vec![1, 1], // path-from-root
			       recurse: true}).prune();
	
	let y = tree::Node::<i32> {
	    value : 1,
	    children : vec![
		n(2),
		nc(3, vec![
		    n(4)] ),
		nc(6, vec![
		    n(3),
		    nc(3, vec![
			n(4),
			n(5)] ),
		    n(7)] ) ] };
	
	assert::equal(&x, &y);
	
	let r = vec![
	    n(5) ];
	
	assert::equal(&removed, &r);
    }

    // select_by_path using ancestor (parent in this case)
    {
	let mut x = orig.clone();
	
	let removed = x.select_by_path(
	    &|ancestors, _path, _node|
	    tree::Disposition{
		select: 3 == **ancestors.last().unwrap(), // parent
		recurse: true}).prune();
	
	let y = tree::Node::<i32> {
	    value : 1,
	    children : vec![
		n(2),
		nc(3, vec![] ),
		nc(6, vec![
		    n(3),
		    nc(3, vec![] ),
		    n(7)] ) ] };

	
	assert::equal(&x, &y);
	
	let r = vec![
	    n(4),
	    n(5),
	    n(4),
	    n(5) ];
	
	assert::equal(&removed, &r);
    }
}
