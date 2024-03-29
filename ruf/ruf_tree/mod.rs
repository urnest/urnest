// Copyright (c) 2021 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
extern crate ruf_assert;

use ruf_assert as assert;

/// A tree of Ts.
#[derive(PartialEq,Clone)]
pub struct Node<T>
{
    pub value : T,
    pub children : Vec<Node<T> >,
}

/// Selected list of sub-trees of a Node<T> that allows mutation of the
/// Node<T>. Use via Node<T>'s select* methods e.g.:
///   tree : Node<i32> = ...;
///   tree.mut_select_by_value(&|v:&i32| v == &6).prune(); // remove all 6s
///   tree.mut_select_by_value(&|v:&i32| v == &100)
///       .mut_extend_by_value(&|v:&i32| v < 50).prune(); // remove 100s and < 50s
///   
pub struct MutableSelection<'a,T>
{
    root : &'a mut Node<T>,
    /// paths from root of selected sub-trees, each being a non-empty
    /// list of indices capturing the path from root to the selected node
    selected_paths : Vec<Vec<usize> >
}

/// Selected list of sub-trees of a Node<T> that does not allow mutation of the
/// Node<T>. Use via Node<T>'s select* methods e.g.:
///   tree : Node<i32> = ...;
///   tree.select_by_value(&|v:&i32| v == &6).iter(); // iterate over all nodes with value 6
///   tree.select_by_value(&|v:&i32| v == &100)
///       .extend_by_value(&|v:&i32| v < 50).iter(); // iterate over all nodes with value 100 or < 50
///   
#[derive(PartialEq,Clone)]
pub struct Selection<'a,T>
{
    root : &'a Node<T>,
    /// paths from root of selected sub-trees, each being a non-empty
    /// list of indices capturing the path from root to the selected node
    selected_paths : Vec<Vec<usize> >
}

/// Disposition of a path selector.
pub struct Disposition
{
    // paths to select relative to the current node
    pub select: Vec<Vec<usize> >,
    pub recurse: bool
}
impl Disposition
{
    pub fn select_this_node_and_recurse(select_this_node: bool,
					recurse: bool) -> Disposition {
	match select_this_node {
	    true => Disposition{ select: vec![Vec::<usize>::new()],
				 recurse: recurse },
	    false => Disposition{ select: Vec::<Vec<usize>>::new(),
				  recurse: recurse }
	}
    }
}
    
impl<'a, 'b, T> MutableSelection<'a, T>
    where 'a : 'b
{
    /// Append descendants of self.root that are selected by selector to
    /// currently selected nodes returning resulting (extended) selection.
    pub fn extend_by_value<F>(self : &'b mut MutableSelection<'a, T>,
			      selector: &F) -> &'b mut MutableSelection<'a, T>
    where
        F: Fn(&T) -> bool
    {
	return self.extend_by_path(
	    &|_ancestors, _path, _starting_from, node : &Node<T>|
	    Disposition::select_this_node_and_recurse(
		selector(&node.value),
		true));
    }
    
    /// Append descendants of self.root that are selected by selector to
    /// currently selected nodes returning resulting (extended) selection.
    /// - selector gets:
    ///   ancestors from (including) root down to (excluding) node
    ///   path from index-of-child-of-root down to index-of-node
    ///   starting_from is how many of ancestors are outside the search
    ///    i.e. the select started at (ancestors + [node.value])[starting_from]
    ///   node in question
    pub fn extend_by_path<F>(self : &'b mut MutableSelection<'a, T>,
			     selector: &F) -> &'b mut MutableSelection<'a,T>
    where
	F: Fn(&[&T], //ancestors
	      &[usize], //path
	      usize, // starting_from
	      &Node<T> //node
             ) -> Disposition
    {
	let mut ancestors : Vec<&T> = vec![&self.root.value];
	let mut path : Vec<usize> = vec![];
        for (i, child) in self.root.children.iter().enumerate() {
	    path.push(i);
            self.selected_paths.extend(select_by_path(
		&mut ancestors, &mut path, 1,
		child, selector));
	    path.pop();
        }
        return self;
    }

    /// Refine selection to those of the currently selected nodes and their
    /// descendants that match selector.
    pub fn refine_by_value<F>(self : &'b mut MutableSelection<'a, T>,
                              selector: &F) -> &'b mut MutableSelection<'a, T>
    where
        F: Fn(&T) -> bool
    {
	return self.refine_by_path(
	    &|_ancestors, _path, _starting_from, node : &Node<T>|
	    Disposition::select_this_node_and_recurse(
		selector(&node.value),
		true));
    }
    
    /// Refine selection to those of the currently selected nodes and their
    /// descendants that match selector.
    /// - selector gets:
    ///   ancestors from (including) root down to (excluding) node
    ///   path from index-of-child-of-root down to index-of-node
    ///   starting_from is how many of ancestors are outside the search
    ///    i.e. the select started at (ancestors + [node.value])[starting_from]
    ///   node in question
    pub fn refine_by_path<F>(self : &'b mut MutableSelection<'a, T>,
                             selector: &F) -> &'b mut MutableSelection<'a, T>
    where
	F: Fn(&[&T], //ancestors
	      &[usize], //path
	      usize, // starting_from
	      &Node<T> //node
             ) -> Disposition
    {
        let mut was = Vec::<Vec<usize>>::new();
	std::mem::swap(&mut was, &mut self.selected_paths);
	for mut path in was {
	    assert::not_equal(&path.len(), &0);
	    let mut ancestors : Vec<&T> = vec![];
	    let node = follow_path(&mut ancestors,
				   &self.root,
				   &path[..]);
	    let starting_from = ancestors.len();
	    assert::equal(&ancestors.len(), &path.len());
            self.selected_paths.extend(select_by_path(
		&mut ancestors, &mut path, starting_from, node, selector));
	}
	return self;
    }

    /// Prune selected nodes (subtrees) from tree, returning
    /// non-nested removed nodes (with nested selected nodes left
    /// within their removed subtree root nodes).
    /// Result order mirrors selection order.
    pub fn prune(self : &'a mut MutableSelection<'a, T>) -> Vec<Node<T> >
    {
        let mut result : Vec<Node<T>> = vec![];
        self.selected_paths.sort_unstable();
        if self.selected_paths.len() != 0 {
            while self.selected_paths.len() > 1 {
                let p : Vec<usize> = self.selected_paths.pop().unwrap();
                if !contains(self.selected_paths.last().unwrap(), &p) {
                    result.push(self.remove(&p));
                }
            }
	    let p = self.selected_paths.pop().unwrap();
            result.push(self.remove(&p));
            self.selected_paths.pop();
        }
        assert::equal(&self.selected_paths, &vec![]);
	result.reverse();
        return result;
    }

    // Remove node at specified path.
    fn remove(self : &mut MutableSelection<'a, T>,
              path: &[usize]) -> Node<T> {
        let mut parent: &mut Node<T> = self.root;
        for i in 0..(path.len()-1) {
            parent = &mut parent.children[path[i]];
        }
        return parent.children.remove(*path.last().unwrap());
    }
}

impl<'a, T> MutableSelection<'a, T>
    where T: std::marker::Copy
{
    /// Copy values of all selected nodes, in selection order.
    pub fn copy_selected_values(self : &MutableSelection<'a,T>) -> Vec<T>
    {
        let mut result : Vec<T> = vec![];
        for p in self.selected_paths.as_slice() {
            result.push(self.copy_value(&p));
        }
        return result;
    }
    fn copy_value(self : &MutableSelection<'a,T>, p: &[usize]) -> T
    {
        let mut result : &Node<T> = self.root;
        for i in p {
            result = &result.children[*i];
        }
        return result.value;
    }
}

pub struct Iter<'a, T>
{
    root : &'a Node<T>,
    current: std::slice::Iter<'a, Vec<usize>>
}
impl<'a,T> std::iter::Iterator for Iter<'a, T>
{
    type Item = &'a Node<T>;
    fn next(&mut self) -> Option<&'a Node<T>>
    {
        match self.current.next() {
            None => None,
            Some(path) => {
                let mut ancestors = Vec::new();
                Some(follow_path(&mut ancestors,
		                 &self.root,
		                 &path[..]))
            }
        }
    }
}

impl<'a, T> Selection<'a, T>
{
    /// Append descendants of self.root that are selected by selector to
    /// currently selected nodes returning resulting (extended) selection.
    pub fn extend_by_value<F>(self : &Selection<'a, T>,
			      selector: &F) -> Selection<'a, T>
    where
        F: Fn(&T) -> bool
    {
        
	return self.extend_by_path(
	    &|_ancestors, _path, _starting_from, node : &Node<T>|
	    Disposition::select_this_node_and_recurse(
		selector(&node.value),
		true));
    }
    
    /// Append descendants of self.root that are selected by selector to
    /// currently selected nodes returning resulting (extended) selection.
    /// - selector gets:
    ///   ancestors from (including) root down to (excluding) node
    ///   path from index-of-child-of-root down to index-of-node
    ///   starting_from is how many of ancestors are outside the search
    ///    i.e. the select started at (ancestors + [node.value])[starting_from]
    ///   node in question
    pub fn extend_by_path<F>(self : &Selection<'a, T>,
			     selector: &F) -> Selection<'a,T>
    where
	F: Fn(&[&T], //ancestors
	      &[usize], //path
	      usize, // starting_from
	      &Node<T> //node
             ) -> Disposition
    {
	let mut ancestors : Vec<&T> = vec![&self.root.value];
	let mut path : Vec<usize> = vec![];
        let mut result:Selection<'a, T> = Selection{ root:self.root,
                                                     selected_paths: self.selected_paths.clone()};
        for (i, child) in self.root.children.iter().enumerate() {
	    path.push(i);
            result.selected_paths.extend(select_by_path(
		&mut ancestors, &mut path, 1,
		child, selector));
	    path.pop();
        }
        return result;
    }

    /// Refine selection to those of the currently selected nodes and their
    /// descendants that match selector.
    pub fn refine_by_value<F>(self : Selection<'a, T>,
                              selector: &F) -> Selection<'a, T>
    where
        F: Fn(&T) -> bool
    {
	return self.refine_by_path(
	    &|_ancestors, _path, _starting_from, node : &Node<T>|
	    Disposition::select_this_node_and_recurse(
		selector(&node.value),
		true));
    }
    
    /// Refine selection to those of the currently selected nodes and their
    /// descendants that match selector.
    /// - selector gets:
    ///   ancestors from (including) root down to (excluding) node
    ///   path from index-of-child-of-root down to index-of-node
    ///   starting_from is how many of ancestors are outside the search
    ///    i.e. the select started at (ancestors + [node.value])[starting_from]
    ///   node in question
    pub fn refine_by_path<F>(self : &Selection<'a, T>,
                             selector: &F) -> Selection<'a, T>
    where
	F: Fn(&[&T], //ancestors
	      &[usize], //path
	      usize, // starting_from
	      &Node<T> //node
             ) -> Disposition
    {
        let mut result = Selection::<'a, T>{ root: self.root, selected_paths: Vec::<Vec<usize>>::new() };
	for path in self.selected_paths.iter() {
	    assert::not_equal(&path.len(), &0);
	    let mut ancestors : Vec<&T> = vec![];
	    let node = follow_path(&mut ancestors,
				   &self.root,
				   &path[..]);
	    let starting_from = ancestors.len();
	    assert::equal(&ancestors.len(), &path.len());
            result.selected_paths.extend(select_by_path(
		&mut ancestors, &mut path.clone(), starting_from, node, selector));
	}
	return result;
    }
    pub fn iter(self: &'a Selection<'a, T>) -> Iter<'a, T> {
        Iter::<'a, T>{ root: self.root,current: self.selected_paths.iter() }
    }
}

impl<'a, T> Selection<'a, T>
    where T: std::marker::Copy
{
    /// Copy values of all selected nodes, in selection order.
    pub fn copy_selected_values(self : &Selection<'a,T>) -> Vec<T>
    {
        let mut result : Vec<T> = vec![];
        for p in self.selected_paths.as_slice() {
            result.push(self.copy_value(&p));
        }
        return result;
    }
    fn copy_value(self : &Selection<'a,T>, p: &[usize]) -> T
    {
        let mut result : &Node<T> = self.root;
        for i in p {
            result = &result.children[*i];
        }
        return result.value;
    }
}

impl<'a, T> Node<T>
{
    /// Select descendants of self per selector.
    /// - includes nested matches
    /// - includes parents before their ancestors
    /// - includes matching siblings in left-to-right order
    pub fn select_by_value<F>(self : &'a Node<T>,
                                  selector: &F) -> Selection<'a,T>
    where F: Fn(&T) -> bool
    {
	let result = Selection::<'a, T>{
	    root: self,
	    selected_paths : vec![] };
	return result.extend_by_value(selector);
    }

    /// Select descendants of self per selector.
    /// - includes nested matches
    /// - includes parents before their ancestors
    /// - includes matching siblings in left-to-right order
    /// - selector gets:
    ///   ancestors from (including) root down to (excluding) node
    ///   path from index-of-child-of-root down to index-of-node
    ///   starting_from is how many of ancestors are outside the search
    ///    i.e. the select started at (ancestors + [node.value])[starting_from]
    ///   node in question
    pub fn select_by_path<F>(self : &'a Node<T>,
                             selector: &F) -> Selection<'a,T>
    where
	F: Fn(&[&T],  // ancestors
	      &[usize], // path
	      usize, // starting_from
	      &Node<T>  // node
             ) -> Disposition
    {
	let result = Selection::<'a, T>{
	    root: self,
	    selected_paths : vec![] };
	return result.extend_by_path(selector);
    }

    /// Select descendants of self per selector.
    /// - includes nested matches
    /// - includes parents before their ancestors
    /// - includes matching siblings in left-to-right order
    pub fn mut_select_by_value<F>(self : &'a mut Node<T>,
                                  selector: &F) -> MutableSelection<'a,T>
    where F: Fn(&T) -> bool
    {
	let mut result = MutableSelection::<'a, T>{
	    root: self,
	    selected_paths : vec![] };
	result.extend_by_value(selector);
	return result;
    }

    /// Select descendants of self per selector.
    /// - includes nested matches
    /// - includes parents before their ancestors
    /// - includes matching siblings in left-to-right order
    /// - selector gets:
    ///   ancestors from (including) root down to (excluding) node
    ///   path from index-of-child-of-root down to index-of-node
    ///   starting_from is how many of ancestors are outside the search
    ///    i.e. the select started at (ancestors + [node.value])[starting_from]
    ///   node in question
    pub fn mut_select_by_path<F>(self : &'a mut Node<T>,
                             selector: &F) -> MutableSelection<'a,T>
    where
	F: Fn(&[&T],  // ancestors
	      &[usize], // path
	      usize, // starting_from
	      &Node<T>  // node
             ) -> Disposition
    {
	let mut result = MutableSelection::<'a, T>{
	    root: self,
	    selected_paths : vec![] };
	result.extend_by_path(selector);
	return result;
    }
}

// Does a contain (or equal) b?
pub fn contains(a : &[usize], b: &[usize]) -> bool {
    (a.len() >= b.len()) && (a[..] == b[0..a.len()])
}

// Get paths from root of those of node and its descendants that are
// selected by selector.
// - selector gets:
//     ancestors from (including) root down to (excluding) node
//     path from index-of-child-of-root down to index-of-node
//     starting_from is how many of ancestors are outside the search
//       i.e. the select started at (ancestors + [node.value])[starting_from]
//     node in question
//  ... note neither ancestors nor path are empty
fn select_by_path<'a, T, F>(
    ancestors: &mut Vec<&'a T>,
    path : &mut Vec<usize>,
    starting_from: usize,
    node: &'a Node<T>,
    selector: &F) -> Vec<Vec<usize>>
where
    F: Fn(&[&T], // ancestors
	  &[usize], // path
	  usize, // starting_from
	  &Node<T> // node
         ) -> Disposition
{
    assert::not_equal(&ancestors.len(), &0);
    assert::not_equal(&path.len(), &0);
    
    let mut result : Vec<Vec<usize>> = Vec::new();
    let disposition = selector(ancestors, path, starting_from, node);
    for rel_path in &disposition.select {
	assert_path_valid(node, &rel_path[..]);
	let mut p = path.clone();
	p.extend(rel_path);
        result.push(p);
    }
    if disposition.recurse {
	for (i, child) in node.children.iter().enumerate() {
	    path.push(i);
	    ancestors.push(&node.value);
            let mut selected_descendants = select_by_path(
		ancestors, path, starting_from, child, selector);
	    for s in &mut selected_descendants {
		result.push(Vec::<usize>::new());
		std::mem::swap(s, result.last_mut().unwrap());
	    }
	    ancestors.pop();
	    path.pop();
	}
    }
    return result;
}
fn assert_path_valid<T>(node: &Node<T>, path: &[usize])
{
    match path.len() {
	0 => (),
	_ => {
	    let i = path[0];
	    assert::less(&i, &node.children.len());
	    assert_path_valid(&node.children[i], &path[1..])
	}
    }
}

// get leaf of path from node, appending
// each traversed node's value to ancestors
fn follow_path<'a,T>(ancestors: &mut Vec<&'a T>,
		     mut node: &'a Node<T>,
		     path: &[usize]) -> &'a Node<T>
{
    if path.len() > 0 {
	ancestors.push(&node.value);
	node = follow_path(ancestors,
			   &node.children[path[0]],
			   &path[1..]);
    }
    return node;
}
