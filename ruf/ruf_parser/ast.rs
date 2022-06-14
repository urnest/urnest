extern crate ruf_tree;

use ruf_tree as tree;

#[derive(PartialEq)]
pub struct Item<'text>
{
    pub tag: Option<&'static str>,
    pub text: &'text str
}
pub type AST<'text> = tree::Node<Item<'text>>;
