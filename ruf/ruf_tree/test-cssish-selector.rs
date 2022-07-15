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

type Disp = tree::Disposition;

#[derive(PartialEq)]
pub enum SelectorTerm
{
    Value(String),
    Star,
}

fn matches(node: &tree::Node<String>, term: &SelectorTerm) -> bool
{
    match term {
	SelectorTerm::Value(x) => &node.value == x,
	SelectorTerm::Star => true
    }
}

// Get paths from node to all nodes matching selector.
fn select(
    node: &tree::Node<String>,
    current_term : &(bool, SelectorTerm),
    rest_of_terms: &[(bool, SelectorTerm)]) -> Vec<Vec<usize>>
{
    let mut result : Vec<Vec<usize>> = vec![];

    let recurse = current_term.0;
    let term = &current_term.1;

    if matches(node, term) {
	match rest_of_terms.len() {
	    0 => {
		result.push(vec![]);
	    }
	    _ => {
		for (i, child) in node.children.iter().enumerate() {
		    for m in select(child, &rest_of_terms[0], &rest_of_terms[1..]) {
			let mut c : Vec<usize> = vec![i];
			c.extend(m.iter());
			result.push(c);
		    }
		}
	    }
	}
    }
    if recurse {
	for (i, child) in node.children.iter().enumerate() {
	    for m in select(child, current_term, &rest_of_terms){
		let mut c : Vec<usize> = vec![i];
		c.extend(m.iter());
		result.push(c);
	    }
	}
    }
    return result;
}


struct CssSelector
{
    first_term: (bool, SelectorTerm),
    rest_of_terms: Vec<(bool, SelectorTerm)>,
}

impl CssSelector
{
    pub fn new(first_term: (bool, SelectorTerm),
	       rest_of_terms: Vec<(bool, SelectorTerm)>) -> CssSelector
    {
	return CssSelector { first_term,
			     rest_of_terms };
    }

    // function for use with tree::mut_select_by_value
    pub fn tree_selector(self : &CssSelector,
			 ancestors: &[&String],
			 _path: &[usize],
			 starting_from: usize,
			 node: &tree::Node<String>) -> Disp
    {
	let mut matched_paths = std::collections::BTreeSet::<Vec::<usize>>::new();
	if starting_from == ancestors.len() {
	    // this node is a search-root, effectively a new search,
	    // so we use our css-ish select function to get matching nodes
	    // (amongst itself and its descendants) and keep them handy and
	    // guide the tree selector to each of them
	    
	    let matches = select(&node,
				 &self.first_term,
				 &self.rest_of_terms[..]);
	    for m in matches {
		matched_paths.insert(m);
	    }
	}
	return Disp { select: matched_paths.into_iter().collect(),
		      recurse: false };
    }
}

/* REVISIT: it would be nice to just pass a &CssSelector to 
   tree::mut_select_by_path, but it seems rust cannot do that yet, e.g.
   at Nov 2021 none of below works, and
    https://users.rust-lang.org/t/how-i-can-implement-fnonce-on-rust-stable/49854/3
impl Fn(&[&String], // ancestors
	    &[usize], // path
	    usize, // starting_from
	    &tree::Node<String> // node
) for CssSelector
{
    fn call(&self,
	    ancestors: &[&String],
	    path: &[usize],
	    starting_from: usize,
	    node: &tree::Node<String>
    ) -> tree::Disposition
    {
	return self.tree_selector(ancestors, path, starting_from, node);
    }
}

struct C1<'a>
{
    x : &'a CssSelector
}
impl<'a> Fn(&[&String], // ancestors
	    &[usize], // path
	    usize, // starting_from
	    &tree::Node<String> // node
           ) -> tree::Disposition for C1<'a>
{
    fn call(&'a self,
	    ancestors: &[&String], // ancestors
	    path: &[usize], // path
	    starting_from: usize, // starting_from
	    node: &tree::Node<String> // node
    ) -> tree::Disposition
    {
	self.x.tree_selector(ancestors, path, starting_from, node)
    }
}
 */

fn n(value:&str) -> tree::Node<String> {
    let result = tree::Node::<String>{ value : value.to_string(),
				       children : vec![] };
    return result;
}
fn nc(value:&str, children:Vec<tree::Node<String>>) -> tree::Node<String> {
    tree::Node::<String>{ value: value.to_string(), children }
}

fn main()
{
    assert::equal(&matches(&n(&"img".to_string()),
			   &SelectorTerm::Value("img".to_string())), &true);
    assert::equal(&matches(&n(&"img".to_string()),
			   &SelectorTerm::Value("p".to_string())), &false);
    assert::equal(&matches(&n(&"img".to_string()),
			   &SelectorTerm::Star), &true);
    let orig = tree::Node::<String> {
	value : "html".to_string(),
	children : vec![
	    n("head"),
	    nc("body", vec![
		n("h1"),
		n("p"),
		nc("p", vec![
		    n("img")]),
		nc("ul", vec![
		    n("li"),
		    n("img"),
		    n("li")] ),
		nc("p", vec![
		    nc("div", vec![
			n("div"),
			nc("div", vec![
			    n("img")])])] ) ])] };

    // p img
    {
	let cs = CssSelector::new(
	    (true, SelectorTerm::Value("p".to_string())),
	    vec![ (true, SelectorTerm::Value("img".to_string())) ]);
	assert::equal(
	    &select(&orig,
		    &cs.first_term,
		    &cs.rest_of_terms[..]),
	    &vec![vec![1, 2, 0],
		  vec![1, 4, 0, 1, 0]]);

	let mut x = orig.clone();
	let mut selection = x.mut_select_by_path(
	    &|ancestors, path, starting_from, node| {
		cs.tree_selector(ancestors, path, starting_from, node)
	    });
	selection.prune();
	
	let y = tree::Node::<String> {
	    value : "html".to_string(),
	    children : vec![
		n("head"),
		nc("body", vec![
		    n("h1"),
		    n("p"),
		    nc("p", vec![]),
		    nc("ul", vec![
			n("li"),
			n("img"),
			n("li")] ),
		    nc("p", vec![
			nc("div", vec![
			    n("div"),
			    nc("div", vec![])])] ) ])] };
	assert::equal(&x, &y);
    }
    // > p img
    {
	let cs = CssSelector::new(
	    (false, SelectorTerm::Value("p".to_string())),
	    vec![ (true, SelectorTerm::Value("img".to_string())) ]);
	
	assert::equal(
	    &select(&orig,
		    &cs.first_term,
		    &cs.rest_of_terms[..]),
	    &vec![]);

	let mut x = orig.clone();
	let mut selection = x.mut_select_by_path(
	    &|ancestors, path, starting_from, node| {
		cs.tree_selector(ancestors, path, starting_from, node)
	    });
	selection.prune();

	assert::equal(&x, &orig);
    }
    
    // p > img
    {
	let cs = CssSelector::new(
	    (true, SelectorTerm::Value("p".to_string())),
	    vec![ (false, SelectorTerm::Value("img".to_string())) ]);
	
	assert::equal(
	    &select(&orig,
		    &cs.first_term,
		    &cs.rest_of_terms[..]),
	    &vec![vec![1, 2, 0]]);

	let mut x = orig.clone();
	let mut selection = x.mut_select_by_path(
	    &|ancestors, path, starting_from, node| {
		cs.tree_selector(ancestors, path, starting_from, node)
	    });
	selection.prune();

	let y = tree::Node::<String> {
	    value : "html".to_string(),
	    children : vec![
		n("head"),
		nc("body", vec![
		    n("h1"),
		    n("p"),
		    nc("p", vec![]),
		    nc("ul", vec![
			n("li"),
			n("img"),
			n("li")] ),
		    nc("p", vec![
			nc("div", vec![
			    n("div"),
			    nc("div", vec![
				n("img")])])] ) ])] };

	assert::equal(&x, &y);
	
    }
    
    // div img
    {
	let cs = CssSelector::new(
	    (true, SelectorTerm::Value("div".to_string())),
	    vec![ (true, SelectorTerm::Value("img".to_string())) ]);
	assert::equal(
	    &select(&orig,
		    &cs.first_term,
		    &cs.rest_of_terms[..]),
	    &vec![vec![1, 4, 0, 1, 0],
		  vec![1, 4, 0, 1, 0]]);
	let mut x = orig.clone();
	let mut selection = x.mut_select_by_path(
	    &|ancestors, path, starting_from, node| {
		cs.tree_selector(ancestors, path, starting_from, node)
	    });
	selection.prune();

	let y = tree::Node::<String> {
	    value : "html".to_string(),
	    children : vec![
		n("head"),
		nc("body", vec![
		    n("h1"),
		    n("p"),
		    nc("p", vec![
			n("img")]),
		    nc("ul", vec![
			n("li"),
			n("img"),
			n("li")] ),
		    nc("p", vec![
			nc("div", vec![
			    n("div"),
			    nc("div", vec![])])] ) ])] };
	assert::equal(&x, &y);
    }
    
    // div > img
    {
	let cs = CssSelector::new(
	    (true, SelectorTerm::Value("div".to_string())),
	    vec![ (false, SelectorTerm::Value("img".to_string())) ]);
	assert::equal(
	    &select(&orig,
		    &cs.first_term,
		    &cs.rest_of_terms[..]),
	    &vec![vec![1, 4, 0, 1, 0]]);
	let mut x = orig.clone();
	let mut selection = x.mut_select_by_path(
	    &|ancestors, path, starting_from, node| {
		cs.tree_selector(ancestors, path, starting_from, node)
	    });
	selection.prune();

	let y = tree::Node::<String> {
	    value : "html".to_string(),
	    children : vec![
		n("head"),
		nc("body", vec![
		    n("h1"),
		    n("p"),
		    nc("p", vec![
			n("img")]),
		    nc("ul", vec![
			n("li"),
			n("img"),
			n("li")] ),
		    nc("p", vec![
			nc("div", vec![
			    n("div"),
			    nc("div", vec![])])] ) ])] };
	assert::equal(&x, &y);
    }
}
