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

// failed to parse {goal} at {at} because, having parsed {having_parsed}, ...
pub struct Context<'text, 'goal>
{
    pub at: &'text str,
    pub goal: &'goal dyn std::fmt::Display,
    pub having_parsed: Vec<AST<'text>>
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
                write!(f, "{line}:{col}: failed to parse {goal} because",
                       goal = context.goal.to_string().as_str(),
                       line = lc.line,
                       col = lc.col)?;
                if context.having_parsed.len() > 0 {
                    match &context.having_parsed[0].value.tag {
                        &None => write!(f, ", having parsed {}",
                                        context.having_parsed[0].value.text),
                        Some(tag) => write!(f, ", having parsed \"{}\"", tag)}?;
                    context.having_parsed[1..].iter().try_for_each(|ast| {
                        match &ast.value.tag {
                            &None => write!(f, " and parsed \"{}\"", ast.value.text),
                            Some(tag) => write!(f, " and parsed {}", tag)}})?;
                    write!(f, ",\n")?;
                }
                else {
                    write!(f, "\n")?;
                }
                std::fmt::Result::Ok( () )
            })?;
            write!(f, "{line}:{col}: {cause}", line = lc.line, col = lc.col, cause = self.cause)
        }
        else
        {
            // single line human readable phrase with inline line, col info, e.g.
            //  failed to parse "fred" at line 3 column 5 because "jock..." does not start with "fred"
            // ... another example with partial completion (wrapped here for readability):
            //  failed to parse "fred" then parse " alan" at line 3 column 5 because, having parsed
            //  "fred", failed to parse " alan" at line 3 column 9 because " jock..." does not start
            //  with " alan"
            let mut lc = LineCol { line: 1, col: 1};
            let mut prev_context = self.context.iter().rev().next();
            self.context.iter().rev().try_for_each(|context| {
                let prev_text = prev_context.unwrap().at;
                lc.advance_through(prev_text.split_at(prev_text.len()-context.at.len()).0);
                prev_context = Some(context);
                write!(f, "failed to parse {goal} at line {line} column {col} because",
                       goal = context.goal.to_string().as_str(),
                       line = lc.line,
                       col = lc.col)?;
                if context.having_parsed.len() > 0 {
                    match &context.having_parsed[0].value.tag {
                        &None => write!(f, ", having parsed {}",
                                        context.having_parsed[0].value.text),
                        Some(tag) => write!(f, ", having parsed \"{}\"", tag)}?;
                    context.having_parsed[1..].iter().try_for_each(|ast| {
                        match &ast.value.tag {
                            &None => write!(f, " and parsed \"{}\"", ast.value.text),
                            Some(tag) => write!(f, " and parsed {}", tag)}})?;
                    write!(f, ", ")?;
                }
                else {
                    write!(f, " ")?;
                }
                std::fmt::Result::Ok( () )
            })?;
            write!(f, "{}", self.cause)
        }
    }
}

pub type ParseResult<'text, 'goals, 'parser> = std::result::Result<
        crate::AST<'text>,  // what we parsed
    ParseFailed<'text, 'goals, 'parser> >;

type ParseResult_<'text, 'goals, 'parser> =
    std::result::Result<crate::AST<'text>,  // what we parsed
                        ( ParseFailed<'text, 'goals, 'parser>,
                          Vec<AST<'text>> // having_parsed
                        ) >;

pub trait Parser
{
    fn parse_some_of_<'text, 'goals, 'parser>(&'parser self, text: &'text str) ->
        ParseResult_<'text, 'goals, 'parser>
    where 'text: 'parser, 'parser: 'goals;
    
    fn goal(self: & Self) -> & dyn std::fmt::Display;

    fn parse_some_of<'text, 'goals, 'parser>(&'parser self, text: &'text str) ->
        ParseResult<'text, 'goals, 'parser>
    where 'text: 'parser, 'parser: 'goals
    {
        let result = self.parse_some_of_::<'text, 'goals, 'parser>(text);
        match result {
            crate::ParseResult_::Ok(x) => crate::ParseResult::Ok(x),
            crate::ParseResult_::Err( (mut e, having_parsed) ) => {
                e.context.push(
                    crate::Context::<'text, 'goals> {
                        at: text,
                        goal: self.goal(),
                        having_parsed: having_parsed }
                );
                crate::ParseResult::Err(e)
            }
        }
    }
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
    type Target = dyn Parser+'parser;
    fn deref(&self) -> &Self::Target { &*self.x }
}

pub fn literal(x: &'static str) -> Ref<'static>
{
    Ref::new(parsers::Literal{x: x})
}

pub fn tagged<'parser>(tag: &'static str, content: Ref<'parser>) -> Ref<'parser>
{
    Ref::new(parsers::Tagged{ tag: tag, content: content.x })
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

pub fn none() -> Ref<'static>
{
    Ref::new(parsers::None{})
}
