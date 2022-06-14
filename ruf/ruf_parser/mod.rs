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

pub mod separated_by;

pub mod ast
{
    #[derive(PartialEq)]
    pub struct Item<'text>
    {
        pub tag: Option<&'static str>,
        pub text: &'text str
    }
}
pub type AST<'text> = tree::Node<ast::Item<'text>>;

pub struct Context<'text, 'goal>
{
    pub at: &'text str,
    pub goal: &'goal dyn std::fmt::Display
}

pub struct ParseFailed<'text, 'goals, 'parser>
{
    pub cause: Box<dyn std::fmt::Display+'parser>,
    pub context: Vec< Context<'text, 'goals> >    // outermost is last
}

#[derive(PartialEq)]
pub struct LineCol
{
    pub line: i64,
    pub col: i64
}
impl LineCol
{
    pub fn advance_through(&mut self, x: &str)
    {
        x.chars().for_each(|c|{
            match c {
                '\n' => { self.line += 1; self.col = 1; },
                '\r' => self.col = 1,
                '\t' => self.col = (self.col/8+1)*8,
                _c => { self.col += 1 },
            }
        });
    }
}

impl<'text, 'goals, 'parser> std::fmt::Display for ParseFailed<'text, 'goals, 'parser>
{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        if f.alternate() {
            // multi-line format with preceding line, col phrase with inline line, col info, e.g.
            // 3:5: failed to parse "fred" because
            // 3:5: "jock..." does not start with "fred"
            let mut lc = LineCol { line: 1, col: 1};
            let mut prev_context = self.context.iter().rev().next();
            self.context.iter().rev().try_for_each(|context| {
                let prev_text = prev_context.unwrap().at;
                lc.advance_through(prev_text.split_at(prev_text.len()-context.at.len()).0);
                prev_context = Some(context);
                write!(f, "{line}:{col}: failed to {goal} because\n",
                       goal = context.goal.to_string().as_str(),
                       line = lc.line,
                       col = lc.col)
            })?;
            write!(f, "{line}:{col}: {cause}", line = lc.line, col = lc.col, cause = self.cause)
        }
        else
        {
            // single line human readable phrase with inline line, col info, e.g.
            // failed to parse "fred" at line 3 column 5 because "jock..." does not start with "fred"
            let mut lc = LineCol { line: 1, col: 1};
            let mut prev_context = self.context.iter().rev().next();
            self.context.iter().rev().try_for_each(|context| {
                let prev_text = prev_context.unwrap().at;
                lc.advance_through(prev_text.split_at(prev_text.len()-context.at.len()).0);
                prev_context = Some(context);
                write!(f, "failed to {goal} at line {line} column {col} because ",
                       goal = context.goal.to_string().as_str(),
                       line = lc.line,
                       col = lc.col)
            })?;
            write!(f, "{}", self.cause)
        }
    }
}

pub type ParseResult<'text, 'goals, 'parser> = std::result::Result<
        ( crate::AST<'text>,  // what we parsed
          &'text str ),       // rest of text
    ParseFailed<'text, 'goals, 'parser> >;

pub trait Parser
{
    fn parse_some_of_<'text, 'goals, 'parser>(&'parser self, text: &'text str) ->
        ParseResult<'text, 'goals, 'parser>
    where 'text: 'parser, 'parser: 'goals;
    
    fn goal(self: & Self) -> & dyn std::fmt::Display;

    fn parse_some_of<'text, 'goals, 'parser>(&'parser self, text: &'text str) ->
        ParseResult<'text, 'goals, 'parser>
    where 'text: 'parser, 'parser: 'goals
    {
            let result = self.parse_some_of_::<'text, 'goals, 'parser>(text);
            match result {
                crate::ParseResult::Ok(x) => crate::ParseResult::Ok(x),
                crate::ParseResult::Err(mut e) => {
                    e.context.push( crate::Context::<'text, 'goals> { at: text, goal: self.goal() } );
                    crate::ParseResult::Err(e)
                }
            }
        
    }
}

mod parse;

#[derive(Clone)]
pub enum Operator<'parser>
{
    None,
    And(std::sync::Arc<parse::And<'parser>>)
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
        let and = std::sync::Arc::new(parse::And::<'parser>{ first_term: first_term,
                                                             other_terms: other_terms});
        Ref::<'parser>{ x: and.clone(), op: Operator::And(and) }
    }
}
impl<'parser> std::ops::Deref for Ref<'parser>
{
    type Target = dyn Parser+'parser;
    fn deref(&self) -> &Self::Target { &*self.x }
}

pub fn literal(x: &'static str) -> Ref<'static>
{
    Ref::new(parse::Literal{x: x})
}

pub fn tagged<'parser>(tag: &'static str, content: Ref<'parser>) -> Ref<'parser>
{
    Ref::new(parse::Tagged{ tag: tag, content: content.x })
}

impl<'parser> std::ops::Add for Ref<'parser> {
    type Output = Ref<'parser>;

    fn add(self, other: Ref<'parser>) -> Self::Output {
        match (self.op.clone(), other.op.clone()) {
            (Operator::None, Operator::None) => {
                Ref::<'parser>::new_and(self.x.clone(), vec!(other.x.clone()))
            },
            (Operator::None, Operator::And(and2)) => {
                let mut terms: Vec<std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>> = vec!(
                    and2.first_term.clone());
                terms.append(&mut and2.other_terms.clone());
                Ref::<'parser>::new_and(self.x.clone(), terms)
            },
            (Operator::And(and), Operator::None) => {
                let mut terms: Vec<std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>> = and.other_terms.clone();
                terms.push(other.x.clone());
                Ref::<'parser>::new_and(and.first_term.clone(), terms)
            },
            (Operator::And(and), Operator::And(and2)) => {
                let mut terms: Vec<std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>> = and.other_terms.clone();
                terms.push(and2.first_term.clone());
                terms.append(&mut and2.other_terms.clone());
                Ref::<'parser>::new_and(and.first_term.clone(), terms)
            }
        }
    }
}
