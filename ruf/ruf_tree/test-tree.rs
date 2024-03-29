// Copyright (c) 2021 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

extern crate ruf_tree;
extern crate ruf_assert;

use ruf_tree as tree;
use ruf_assert as assert;

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

    // immutable selection
    {
	let selection = orig.select_by_value(&|v| v==&4).extend_by_value(&|v| v>&6);
        let selected: Vec<&tree::Node<i32>> = selection.iter().collect();
        assert::equal(&selected,
                      &vec!(
                          &orig.children[1].children[0], //n(4) ),
                          &orig.children[2].children[1].children[0], //n(4) ),
                          &orig.children[2].children[2] //n(7)
                      ));
	let selection = orig.select_by_value(&|v| v==&6).refine_by_value(&|v| v>&6);
        let selected: Vec<&tree::Node<i32>> = selection.iter().collect();
        assert::equal(&selected,
                      &vec!(
                          &orig.children[2].children[2] //n(7)
                      ));

	let selection = orig.select_by_path(
	    &|_ancestors, path, starting_from, _node| {
		assert::equal(&starting_from, &1);
		tree::Disposition::select_this_node_and_recurse(
		    path == [1,1], // path-from-root
		    true)
	    });
        let selected: Vec<&tree::Node<i32>> = selection.iter().collect();
        assert::equal(&selected,
                      &vec!(
                          &orig.children[1].children[1]
                      ));
        
	let selection = orig.select_by_value(&|v| v==&6)
	    .refine_by_path(
		&|_ancestors, _path, starting_from, node| {
		    assert::equal(&starting_from, &1);
		    tree::Disposition::select_this_node_and_recurse(
			&3 == &node.value,
			true)
		});
        let selected: Vec<&tree::Node<i32>> = selection.iter().collect();
        assert::equal(&selected,
                      &vec!(
                          &orig.children[2].children[0],
                          &orig.children[2].children[1],
                      ));
    }                                                         
    {
	let mut x = orig.clone();
	
	let is_ten = |value:&i32| value.eq(&10);
	let selection = x.mut_select_by_value(&is_ten);
	
	assert::equal(&selection.copy_selected_values(), &Vec::<i32>::new());
	
	let mut selection = x.mut_select_by_value(&|v| v==&3);
	assert::equal(&selection.copy_selected_values(), &vec![3,3,3]);
	
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
	
	x.mut_select_by_value(&|v| v==&2).prune();
	let y = tree::Node::<i32> {
	    value : 1,
	    children : vec![
		nc(6, vec![
		    n(7)] ) ] };
	
	assert::equal(&x, &y);
    }

    // mut_select_by_value
    {
	let mut x = orig.clone();

	let mut s = x.mut_select_by_value(&|v| v==&2);
	let removed = s.extend_by_value(&|v| v==&3).prune();
	
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

    // mut_select_by_path using index
    {
	let mut x = orig.clone();
	
	let removed = x.mut_select_by_path(
	    &|_ancestors, path, starting_from, _node| {
		assert::equal(&starting_from, &1);
		tree::Disposition::select_this_node_and_recurse(
		    path == [1,1], // path-from-root
		    true)
	    }).prune();
	
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

    // mut_select_by_path using ancestor (parent in this case)
    {
	let mut x = orig.clone();
	
	let removed = x.mut_select_by_path(
	    &|ancestors, _path, _starting_from, _node|
	    tree::Disposition::select_this_node_and_recurse(
		3 == **ancestors.last().unwrap(), // parent
		true)).prune();
	
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
    // refine_by_path using value (parent in this case)
    {
	let mut x = tree::Node::<i32> {
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
	
	let removed = x.mut_select_by_value(&|v| v==&6)
	    .refine_by_path(
		&|_ancestors, _path, starting_from, node| {
		    assert::equal(&starting_from, &1);
		    tree::Disposition::select_this_node_and_recurse(
			&3 == &node.value,
			true)
		}).prune();
	
	let y = tree::Node::<i32> {
	    value : 1,
	    children : vec![
		n(2),
		nc(3, vec![
		    n(4),
		    n(5)]),
		nc(6, vec![
		    n(7)] ) ] };

	
	assert::equal(&x, &y);
	
	let r = vec![
	    n(3),
	    nc(3, vec![
		n(4),
		n(5)] ),
	];
	
	assert::equal(&removed, &r);
    }

    
    // refine_by_value
    {
	let mut x = tree::Node::<i32> {
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
	
	let removed = x.mut_select_by_value(&|v| v==&3)
	    .refine_by_value(&|v| v==&5).prune();
	
	let y = tree::Node::<i32> {
	    value : 1,
	    children : vec![
		n(2),
		nc(3, vec![
		    n(4)] ),
		nc(6, vec![
		    n(3),
		    nc(3, vec![
			n(4)] ),
		    n(7)] ) ] };

	
	assert::equal(&x, &y);
	
	let r = vec![
	    n(5),
	    n(5),
	];
	
	assert::equal(&removed, &r);
    }

    
}
