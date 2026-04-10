// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
extern crate ruf_tree;

use ruf_tree as tree;

#[derive(PartialEq, Clone, Copy)]
pub struct Item<'text>
{
    pub tag: &'static str,
    pub text: &'text str
}
pub type AST<'text> = tree::Node<Item<'text>>;
