// Copyright (c) 2021 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

use ruf::assert;

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
///   tree.select_by_value(&|v:&i32| v == &6).prune(); // remove all 6s
///   tree.select_by_value(&|v:&i32| v == &100)
///       .extend_by_value(&|v:&i32| v < 50).prune(); // remove 100s and < 50s
///   
pub struct MutableSelection<'a,T>
{
    root : &'a mut Node<T>,
    /// paths from root of selected sub-trees, each being a non-empty
    /// list of indices capturing the path from root to the selected node
    selected_paths : Vec<Vec<usize> >
}

/// Disposition of a path selector.
pub struct Disposition
{
    pub select: bool,
    pub recurse: bool
}

impl<'a, T> MutableSelection<'a, T>
{
    /// Append descendants of self.root that are selected by selector to
    /// currently selected nodes returning resulting (extended) selection.
    pub fn extend_by_value<F>(self : MutableSelection<'a, T>,
			      selector: &F) -> MutableSelection<'a, T>
    where
        F: Fn(&T) -> bool
    {
	return self.extend_by_path(
	    &|_ancestors, _path, node : &Node<T>|
	    Disposition { select: selector(&node.value),
			  recurse: true });
    }
    
    /// Append descendants of self.root that are selected by selector to
    /// currently selected nodes returning resulting (extended) selection.
    /// - selector gets:
    ///   ancestors from (including) root down to (excluding) node
    ///   path from index-of-child-of-root down to index-of-node
    ///   node in question
    pub fn extend_by_path<F>(mut self : MutableSelection<'a, T>,
			     selector: &F) -> MutableSelection<'a,T>
    where
	F: Fn(&Vec<&T>, //ancestors
	      &Vec<usize>, //path
	      &Node<T> //node
             ) -> Disposition
    {
	let mut ancestors : Vec<&T> = vec![&self.root.value];
	let mut path : Vec<usize> = vec![];
        for i in 0..self.root.children.len() {
	    path.push(i);
            self.selected_paths.extend(select_by_path(
		&mut ancestors, &mut path, &self.root.children[i], selector));
	    path.pop();
        }
        return self;
    }

    /// Refine selection to those of the currently selected nodes and their
    /// descendants that match selector.
    pub fn refine_by_value<F>(self : MutableSelection<'a, T>,
                              selector: &F) -> MutableSelection<'a, T>
    where
        F: Fn(&T) -> bool
    {
	return self.refine_by_path(
	    &|_ancestors, _path, node : &Node<T>|
	    Disposition { select: selector(&node.value),
			  recurse: true });
    }
    
    /// Refine selection to those of the currently selected nodes and their
    /// descendants that match selector.
    /// - selector gets:
    ///   ancestors from (including) root down to (excluding) node
    ///   path from index-of-child-of-root down to index-of-node
    ///   node in question
    pub fn refine_by_path<F>(mut self : MutableSelection<'a, T>,
                             selector: &F) -> MutableSelection<'a, T>
    where
	F: Fn(&Vec<&T>, //ancestors
	      &Vec<usize>, //path
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
	    assert::equal(&ancestors.len(), &path.len());
            self.selected_paths.extend(select_by_path(
		&mut ancestors, &mut path, node, selector));
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
              path: &Vec<usize>) -> Node<T> {
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
    fn copy_value(self : &MutableSelection<'a,T>, p: &Vec<usize>) -> T
    {
        let mut result : &Node<T> = self.root;
        for i in p.as_slice() {
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
    pub fn select_by_value<F>(self : &'a mut Node<T>,
                              selector: &F) -> MutableSelection<'a,T>
    where F: Fn(&T) -> bool
    {
	let result = MutableSelection::<'a, T>{
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
    ///   node in question
    pub fn select_by_path<F>(self : &'a mut Node<T>,
                             selector: &F) -> MutableSelection<'a,T>
    where
	F: Fn(&Vec<&T>,  // ancestors
	      &Vec<usize>, // path
	      &Node<T> // node
             ) -> Disposition
    {
	let result = MutableSelection::<'a, T>{
	    root: self,
	    selected_paths : vec![] };
	return result.extend_by_path(selector);
    }
}

// Does a contain (or equal) b?
fn contains(a : &Vec<usize>, b: &Vec<usize>) -> bool {
    (a.len() >= b.len()) && (a[..] == b[0..a.len()])
}

// Get paths from root of those of node and its descendants that are
// selected by selector.
// - selector gets:
//     ancestors from (including) root down to (excluding) node
//     path from index-of-child-of-root down to index-of-node
//     node in question
//  ... note neither ancestors nor path are empty
fn select_by_path<'a, T, F>(
    ancestors: &mut Vec<&'a T>,
    path : &mut Vec<usize>,
    node: &'a Node<T>,
    selector: &F) -> Vec<Vec<usize>>
where
    F: Fn(&Vec<&T>, // ancestors
	  &Vec<usize>, // path
	  &Node<T> // node
         ) -> Disposition
{
    assert::not_equal(&ancestors.len(), &0);
    assert::not_equal(&path.len(), &0);
    
    let mut result : Vec<Vec<usize>> = Vec::new();
    let disposition = selector(ancestors, path, node);
    if disposition.select {
        result.push( path.clone() );
    }
    if disposition.recurse {
	for i in 0..node.children.len() {
	    path.push(i);
	    ancestors.push(&node.value);
            let mut selected_descendants = select_by_path(
		ancestors, path, &node.children[i], selector);
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
