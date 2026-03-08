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
extern crate ruf_newtype;
use std::ptr;

use ruf_tree as tree;

pub mod ast;

pub type AST<'text> = tree::Node<ast::Item<'text>>;

pub struct Goal<'text, 'parser> {
    pub parser: &'parser (dyn Parser+Send+Sync),
    pub text: &'text str
}
impl<'text, 'parser> PartialEq for Goal<'text, 'parser> {
    fn eq(&self, other: &Goal<'text, 'parser>) -> bool {
        ptr::eq(self.parser, other.parser) &&
            self.text == other.text
    }
    fn ne(&self, other: &Goal<'text, 'parser>) -> bool {
        !ptr::eq(self.parser, other.parser) ||
            self.text != other.text
    }
}

#[derive(PartialEq)]
pub enum Unexpected {
    Char,
    EndOfInput
}

#[derive(PartialEq)]
pub struct LeafResult<'text> {
    pub matched: &'text str,
    pub then: Option<Unexpected>
}

#[derive(PartialEq)]
pub struct CompositeResult<'text, 'parser> {
    pub matched: Option<&'text str>,  // None if parse failed, otherwise the match
    pub  components: Vec< (Goal<'text, 'parser>, ParseResult<'text, 'parser>) >
}

#[derive(PartialEq)]
pub enum ParseResult<'text, 'parser> {
    Leaf(LeafResult<'text>),
    Composite(CompositeResult<'text, 'parser>)
}


pub trait Parser
{
    fn parse_some_of<'text, 'parser>(
        &'parser self,
        text: &'text str
    ) -> ParseResult<'text, 'parser>
    where 'text: 'parser;
}

mod parsers;

#[derive(Clone)]
pub enum Operator<'parser>
{
    None,
    And(std::sync::Arc<parsers::And<'parser>>),
    Or(std::sync::Arc<parsers::Or<'parser>>)
}

#[derive(Clone)]
pub struct Ref<'parser>
{
    x: std::sync::Arc<dyn Parser+Send+Sync+'parser>,
    op: Operator<'parser>
}
impl<'parser> Ref<'parser>
{
    pub fn new<T>(data: T) -> Ref<'parser>
    where T: Parser+Send+Sync+'parser
    {
        Ref::<'parser>{ x: std::sync::Arc::new(data), op: Operator::None }
    }
    pub fn new_and(
        first_term: std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>,
        other_terms: Vec<std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>>) -> Ref<'parser>
    {
        let and = std::sync::Arc::new(parsers::And::<'parser>{ first_term: first_term,
                                                             other_terms: other_terms});
        Ref::<'parser>{ x: and.clone(), op: Operator::And(and) }
    }
    pub fn new_or(
        first_term: std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>,
        other_terms: Vec<std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>>) -> Ref<'parser>
    {
        let and = std::sync::Arc::new(parsers::Or::<'parser>{ first_term: first_term,
                                                              other_terms: other_terms});
        Ref::<'parser>{ x: and.clone(), op: Operator::Or(and) }
    }
}
impl<'parser> std::ops::Deref for Ref<'parser>
{
    type Target = dyn Parser+Sync+Send+'parser;
    fn deref(&self) -> &Self::Target { &*self.x }
}

impl<'parser> std::ops::Add for Ref<'parser> {
    type Output = Ref<'parser>;

    fn add(self, other: Ref<'parser>) -> Self::Output {
        match (self.op.clone(), other.op.clone()) {
            (Operator::And(and), Operator::And(and2)) => {
                let mut terms: Vec<std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>> = and.other_terms.clone();
                terms.push(and2.first_term.clone());
                terms.append(&mut and2.other_terms.clone());
                Ref::<'parser>::new_and(and.first_term.clone(), terms)
            },
            (Operator::And(and), _) => {
                let mut terms: Vec<std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>> = and.other_terms.clone();
                terms.push(other.x.clone());
                Ref::<'parser>::new_and(and.first_term.clone(), terms)
            },
            (_, Operator::And(and2)) => {
                let mut terms: Vec<std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>> = vec!(
                    and2.first_term.clone());
                terms.append(&mut and2.other_terms.clone());
                Ref::<'parser>::new_and(self.x.clone(), terms)
            },
            (_, _) => {
                Ref::<'parser>::new_and(self.x.clone(), vec!(other.x.clone()))
            }
        }
    }
}

impl<'parser> std::ops::BitOr for Ref<'parser> {
    type Output = Ref<'parser>;

    fn bitor(self, other: Ref<'parser>) -> Self::Output {
        match (self.op.clone(), other.op.clone()) {
            (Operator::Or(or), Operator::Or(or2)) => {
                let mut terms: Vec<std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>> = or.other_terms.clone();
                terms.push(or2.first_term.clone());
                terms.append(&mut or2.other_terms.clone());
                Ref::<'parser>::new_or(or.first_term.clone(), terms)
            },
            (Operator::Or(or), _) => {
                let mut terms: Vec<std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>> = or.other_terms.clone();
                terms.push(other.x.clone());
                Ref::<'parser>::new_or(or.first_term.clone(), terms)
            },
            (_, Operator::Or(or2)) => {
                let mut terms: Vec<std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>> = vec!(
                    or2.first_term.clone());
                terms.append(&mut or2.other_terms.clone());
                Ref::<'parser>::new_or(self.x.clone(), terms)
            },
            (_, _) => {
                Ref::<'parser>::new_or(self.x.clone(), vec!(other.x.clone()))
            }
        }
    }
}

// parses x, literally
pub fn literal(x: &'static str) -> Ref<'static>
{
    Ref::new(parsers::Literal{x: x})
}
