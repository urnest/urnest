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
extern crate ruf_assert;

use std::ptr;
use std::collections::HashMap;

use ruf_assert as assert;
use ruf_tree as tree;
use ruf_newtype as newtype;

pub mod ast;
pub mod all_of;

use all_of::all_of;

// parse some of text {text} with parser {p}
//
// This is the primary parsing interface. It is also available via
// Ref.parse method. See test.mod tutorial() for overview of use.
//
// Note that where the parse succeeds, the left over text is implied
// by what was matched, either result.matched (where p is a LeafParser) or
// result.matched.unwrap() (where p is a composite parser). Note that in the Leaf case
// result.matched must be interpreted considering result.then; the composite case
// is explicit since matched is an Option.
//
// Use pre-defined end_of_input() parser to ensure all text is consumed.
//
// See Stock Parses below for other pre-defined parsers; test.rs has example use of all.
//
// Implement Parser (below) to define your own parsers, though combining pre-defined
// parses via + and | should cover most uses.
//
pub fn parse<'parser, 'text>(
    parser: &'parser dyn Parser, text: &'text str
) -> Parsed<'text, 'parser>
    where 'text: 'parser
{
    let mut cache = vec!();
    cache.resize_with(text.len()+1, ||HashMap::new());
    Parsed {
        goal: Goal { parser, text },
        outcome: parser.parse_some_of(text, &mut cache)
    }
}

pub type AST<'text> = tree::Node<ast::Item<'text>>;

impl<'parser> PartialEq for &'parser (dyn Parser+Send+Sync) {
    fn eq(&self, other: &&'parser (dyn Parser+Send+Sync)) -> bool {
        ptr::eq(*self, *other)
    }
    fn ne(&self, other: &&'parser (dyn Parser+Send+Sync)) -> bool {
        !ptr::eq(*self, *other)
    }
}

impl<'parser> PartialEq for &'parser dyn Parser {
    fn eq(&self, other: &&'parser dyn Parser) -> bool {
        ptr::eq(*self, *other)
    }
    fn ne(&self, other: &&'parser dyn Parser) -> bool {
        !ptr::eq(*self, *other)
    }
}

#[derive(PartialEq, Clone)]
pub struct Goal<'text, 'parser> {
    pub parser: &'parser dyn Parser,
    pub text: &'text str
}

#[derive(PartialEq, Clone)]
pub enum Unexpected {
    Char,
    EndOfInput
}

#[derive(PartialEq, Clone)]
pub struct Parsed<'text, 'parser> {
    pub goal: Goal<'text, 'parser>,
    pub outcome: Outcome<'text, 'parser>
}

#[derive(PartialEq, Clone)]
pub enum Outcome<'text, 'parser> {
    Leaf {
        matched: &'text str,
        then: Option<Unexpected>
    },
    Composite {
        matched: Option<&'text str>,  // None if parse failed, otherwise the match
        components: Vec< (Goal<'text, 'parser>, Outcome<'text, 'parser>) >
    }
}

use Outcome::{Leaf,Composite};

#[derive(PartialEq, Clone)]
pub struct Context<'text> {
    pub tag: &'static str,
    pub text: &'text str
}

#[derive(PartialEq, Clone)]
pub struct ParseFailed<'text> {
    pub at: &'text str,
    pub why: Unexpected,
    pub context: Vec<Context<'text>>    // outermost is last
}

impl<'text, 'parser> Parsed<'text, 'parser> {
    // get parsed ast if parse succeeded:
    // - result AST root node has tag {root_ast_tag} and entire text attempted
    //   (including any left over after parsing succeeded)
    // - tree has nodes for all tagged() parsers that succeeded
    // ... otherwise ParseFailed has the failure that was furthest through
    // plus all its active tagged parsers at the point of failure as context
    // (outermost i.e. root_ast_tag is last)
    pub fn get_ast<'parsed_ref>(
        &'parsed_ref self,
        root_ast_tag: &'static str
    ) -> Result<AST<'text>, ParseFailed<'text>>
    where 'text: 'parser, 'text: 'parsed_ref, 'parser: 'parsed_ref
    {
        match self.outcome {
            Composite{ matched: Some(ref matched), .. } |
            Leaf{ref matched, then: None} =>
                Ok(AST::<'text>{
                    value: ast::Item{ tag: root_ast_tag, text: matched },
                    children: get_component_asts(&vec!(
                        (self.goal.clone(), self.outcome.clone())))
                }),
            
            Composite{ matched: None, .. } => {
                let mut f = find_best_failure(self.goal.text, &vec!(
                    (self.goal.clone(), self.outcome.clone())));
                f.context.push(Context{
                    tag: root_ast_tag,
                    text: self.goal.text
                });
                Err(f)
            },
            
            Leaf{matched, then: Some(ref unexpected)} =>
                Err(ParseFailed{
                    at: all_of(self.goal.text).after(matched),
                    why: unexpected.clone(),
                    context: vec!(Context{
                        tag: root_ast_tag,
                        text: self.goal.text
                    })
                })
        }
    }
}
// pre: components.last succeeded
fn get_component_asts<'text, 'parser>(
    components: &Vec< (Goal<'text, 'parser>, Outcome<'text, 'parser>) >
)
    -> Vec<AST<'text>>
{
    // go through components and convert each to AST(s) ignoring failed components
    components.iter().filter_map(
        |ref component| {
            match component {
                // comp ok, not tagged -> all of childrens asts
                // comp ok, tagged -> add node with tag and child asts
                (Goal{parser, ..},
                 Composite{
                     matched: Some(matched),
                     components
                 }) => {
                    let child_asts = get_component_asts(components);
                    match parser.tag() {
                        None => Some(child_asts),
                        Some(tag) =>
                            Some(vec!(AST::<'text>{
                                value: ast::Item::<'text>{ tag, text: matched },
                                children: child_asts
                            }))
                    }
                },
                // comp fail -> ignore
                // leaf -> ignore (it is implicitly untagged and has no children
                (Goal{..},
                 Composite{ matched: None, .. }) |
                (Goal{..}, Leaf{ matched: _, then: Some(_)}) |
                (Goal{..}, Leaf{ matched: _, then: None}) =>
                    None
            }
        }).collect::<Vec<Vec<AST<'text>>>>().concat()
}
// pre: components.last failed
fn find_best_failure<'text, 'parser>(
    text: &'text str,
    components: &Vec< (Goal<'text, 'parser>, Outcome<'text, 'parser>) >
)
    -> ParseFailed<'text>
{
    let mut result = match components.last().unwrap() {
        (Goal{parser, text}, Composite{ matched: None, components }) => {
            let mut r = find_best_failure(text, &components);
            match parser.tag() {
                None => r,
                Some(tag) => {
                    r.context.push(Context{tag, text: text});
                    r
                }
            }
        },
        (Goal{parser, text}, Leaf{ matched, then: Some(why) }) => 
            ParseFailed{
                at: all_of(text).after(matched),
                why: why.clone(),
                context: match parser.tag() {
                    None => vec!(),
                    Some(tag) => vec!(Context{tag, text: text})
                }
            },
        (Goal{..}, Composite{ matched: Some(_), .. }) |
        (Goal{..}, Leaf{ matched: _, then: None }) =>
            assert::never_reached("see pre: above")
    };
    components[0..components.len()-1].iter().for_each(|component| {
        let f = match component {
            (Goal{parser, text}, Composite{ matched: None, components }) => {
                let mut r = find_best_failure(text, &components);
                match parser.tag() {
                    None => Some(r),
                    Some(tag) => {
                        r.context.push(Context{tag, text: text});
                        Some(r)
                    }
                }
            },
            (Goal{parser, text}, Leaf{ matched, then: Some(unexpected) }) =>
                Some(
                    ParseFailed {
                        at: all_of(text).after(matched),
                        why: unexpected.clone(),
                        context: match parser.tag() {
                            None => vec!(),
                            Some(tag) => vec!(Context{tag,text: text})
                        }
                    }),
            (Goal{..}, Composite{ matched: Some(_matched), .. }) |
            (Goal{..}, Leaf{ matched: _matched, then: None }) =>
                None  // a succeeded in a & b where b failed, a's success is irrelevant
        };
        match f {
            None=> (),
            Some(f) => {
                if all_of(text).up_to(f.at).len() > all_of(text).up_to(result.at).len() {
                    result = f;
                }
            }
        }
    });
    result
}

pub type Cache<'text, 'parser> = HashMap<usize, Outcome<'text, 'parser> >;

pub trait Parser
{
    // parse some of {text}
    // - implement this function to define a custom parser
    // - note result will be cached by parse_some_of
    // - pass cache to any nested invocations of parse_some_of
    // pre: cache.size() == text.size()+1
    //
    fn parse_some_of_<'text, 'parser>(
        &'parser self,
        text: &'text str,
        cache: &mut [Cache<'text, 'parser>]
    ) -> Outcome<'text, 'parser>
    where 'text: 'parser;

    // parse some of {text}
    // pre: cache.size() == text.size()+1
    fn parse_some_of<'text, 'parser>(
        &'parser self,
        text: &'text str,
        cache: &mut [Cache<'text, 'parser>]
    ) -> Outcome<'text, 'parser>
    where 'text: 'parser
    {
        assert::equal(&cache.len(), &(text.len()+1));
        let p : *const Self = self;
        match cache[0].get(&p.addr()) {
            Some(v) => v.clone(),
            None => {
                let v = self.parse_some_of_(text, cache);
                cache[0].insert(p.addr(), v.clone());
                v
            }
        }
    }
    fn tag(&self) -> Option<&'static str>
    {
        None
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
    pub fn parse<'text, 'self_ref>(
        &'self_ref self,
        text: &'text str
    ) -> Parsed<'text, 'self_ref>
    where 'text: 'self_ref, 'parser: 'self_ref
    {
        crate::parse(&*self.x, text)
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

// Stock parsers:

// always matches, consumes nothing
pub fn none() -> Ref<'static>
{
    Ref::new(parsers::None{})
}

// parses c, literally
pub fn char(c: char) -> Ref<'static>
{
    Ref::new(parsers::Char{x: c})
}

// parses x, literally
pub fn literal(x: &'static str) -> Ref<'static>
{
    Ref::new(parsers::Literal{x: x})
}

// parses end-of-input i.e. check that we've consumed all text
pub fn end_of_input() -> Ref<'static>
{
    Ref::new(parsers::EndOfInput{})
}

// wraps AST produced by content in AST with tag
pub fn tagged<'parser>(tag: &'static str, content: Ref<'parser>) -> Ref<'parser>
{
    Ref::new(parsers::Tagged{ tag: tag, content: content.x })
}

// any character
pub fn any_char() -> Ref<'static> { Ref::new(parsers::AnyChar{}) }

// 0..9
pub fn digit() -> Ref<'static>
{
    Ref::new(parsers::Digit{})
}

// at least one x
pub fn at_least_one<'parser>(x: Ref<'parser>) -> Ref<'parser>{Ref::new(parsers::AtLeastOne{x: x.x})}

// zero or more x
pub fn zero_or_more<'parser>(x: Ref<'parser>) -> Ref<'parser>{Ref::new(parsers::ZeroOrMore{x: x.x})}

// CharSet is any string but a-f anywhere in the string is interpreted as abcdef, note
//   f-a (anywhere in the string) just means the three characters f, - and a
//   f-f (anywhere in the string) just means f
//   - at beginning or end of string just means the - character
pub struct CharSet_; impl newtype::Tag for CharSet_ { type BaseType = &'static str;}
pub type CharSet = newtype::T<CharSet_>;

