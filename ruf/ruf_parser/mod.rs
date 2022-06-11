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
    
    fn goal<'parser, 'goal>(self: &'parser Self) -> &'goal dyn std::fmt::Display
        where 'parser: 'goal;

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

#[derive(Clone)]
pub struct Ref<'parser>
{
    x: std::sync::Arc<dyn Parser+'parser>
}
impl<'parser> Ref<'parser>
{
    pub fn new<T>(data: T) -> Ref<'parser>
    where T: Parser+'parser
    {
        Ref::<'parser>{ x: std::sync::Arc::new(data) }
    }
}
impl<'parser> std::ops::Deref for Ref<'parser>
{
    type Target = dyn Parser+'parser;
    fn deref(&self) -> &Self::Target { &*self.x }
}

pub mod parse
{
    use crate::separated_by::IteratorSeparatedBy;
    
    pub struct Literal<'literal> {
        pub x: &'literal str
    }
    impl<'literal> std::fmt::Display for Literal<'literal>
    {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
        {
            // REVISIT: need a rust-escape-string formatting function so that e.g.
            // Literal{ x: "the \"big\" bird" } formats as:
            // parse "the \"big\" bird"
            write!(f, "parse \"{}\"", self.x)
        }
    }
    pub struct LiteralNotFound<'cause> {
        x: &'cause str,
        text: &'cause str
    }
    impl<'cause> LiteralNotFound<'cause>
    {
        fn new(x:&'cause str, text:&'cause str) -> Box<dyn std::fmt::Display+'cause>
        {
            Box::new(LiteralNotFound::<'cause>{
                x: &x,
                text: &text
            })
        }
    }
    
    impl<'cause> std::fmt::Display for LiteralNotFound<'cause> {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
        {
            // REVISIT: quite hard to produce "fred..." does not start with "sal" properly
            // due to unicode grapheme clusters, which below ignores
            let n = self.x.chars().count();
            if self.text.chars().count() <= n {
                return write!(f, "\"{}\" does not start with \"{}\"", self.text, self.x);
            }
            write!(f, "\"")?;
            self.text.chars().take(n).try_for_each(|c| write!(f, "{}", c))?;
            write!(f, "...\" does not start with \"{}\"", self.x)
        }
    }

    impl<'literal> crate::Parser for Literal<'literal>
    {
        fn parse_some_of_<'text, 'goals, 'parser>(self: &'parser Self, text: &'text str) ->
            crate::ParseResult<'text, 'goals, 'parser>
        where 'text: 'parser, 'parser: 'goals, 'literal: 'parser
        {
            if text.starts_with(self.x) {
                let (x, y) = text.split_at(self.x.len());
                crate::ParseResult::<'text, 'goals, 'parser>::Ok(
                    ( crate::AST{ value: crate::ast::Item { tag: None, text: x },
                                  children: vec!() }, y) )
            }
            else {
                crate::ParseResult::<'text, 'goals, 'parser>::Err(
                    crate::ParseFailed::<'text, 'goals, 'parser>{
                        context: vec!(),
                        cause: LiteralNotFound::new(&self.x, &text)
                    })
            }
        }
        fn goal<'parser, 'goal>(self: &'parser Self) -> &'goal dyn std::fmt::Display
        where 'parser: 'goal
        {
            self
        }
    }

    pub struct Tagged<'content> {
        pub tag: &'static str,
        pub content: crate::Ref<'content>
    }
    impl<'content> std::fmt::Display for Tagged<'content>
    {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
        {
            write!(f, "parse {}", self.tag)
        }
    }
    impl<'content> crate::Parser for Tagged<'content>
    {
        fn parse_some_of_<'text, 'goals, 'parser>(self: &'parser Self, text: &'text str) ->
            crate::ParseResult<'text, 'goals, 'parser>
        where 'text: 'parser, 'parser: 'goals, 'content: 'parser
        {
            let result = self.content.parse_some_of(text);
            match result {
                crate::ParseResult::Ok(x) => crate::ParseResult::Ok(
                    ( crate::AST{ value: crate::ast::Item{ tag: Some(self.tag), text: x.0.value.text },
                                  children: vec!(x.0) },
                      x.1 )),
                crate::ParseResult::Err(e) => {
                    crate::ParseResult::Err(e)
                }
            }
        }
        fn goal<'parser, 'goal>(self: &'parser Self) -> &'goal dyn std::fmt::Display
        where 'parser: 'goal
        {
            self
        }
    }

    pub struct And<'parser>
    {
        pub terms: Vec<crate::Ref<'parser> >
    }
    struct DisplayThen {}
    impl<'parser> std::fmt::Display for DisplayThen
    {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
        {
            write!(f, " then ")
        }
    }
    fn display_then() -> &'static dyn std::fmt::Display
    {
        static RESULT: DisplayThen = DisplayThen{};
        return &RESULT;
    }
    
    impl<'parser> std::fmt::Display for And<'parser>
    {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
        {
            self.terms.iter().map(|r| r.goal()).ruf_parser_separated_by(display_then()).try_for_each(
                |y| y.fmt(f))
        }
    }
    impl<'and> crate::Parser for And<'and>
    {
        fn parse_some_of_<'text, 'goals, 'parser>(self: &'parser Self, text: &'text str) ->
            crate::ParseResult<'text, 'goals, 'parser>
        where 'text: 'parser, 'parser: 'goals, 'and: 'parser
        {
            let mut rest = text;
            let mut result = crate::AST::<'text>{ value: crate::ast::Item::<'text> { tag: None, text: text },
                                                  children : vec!() };
            for term in self.terms.iter() {
                match term.parse_some_of(rest) {
                    crate::ParseResult::Ok(x) => {
                        result.children.push(x.0);
                        rest = x.1;
                    },
                    crate::ParseResult::Err(e) => {
                        return crate::ParseResult::Err(e);
                    }
                }
            }
            result.value.text = text.split_at(text.len() - rest.len()).0;
            return crate::ParseResult::Ok( (result, rest) );
        }
        fn goal<'parser, 'goal>(self: &'parser Self) -> &'goal dyn std::fmt::Display
        where 'parser: 'goal
        {
            self
        }
    }
}

pub fn literal(x: &'static str) -> Ref<'static>
{
    Ref::new(parse::Literal{x: x})
}

pub fn tagged<'parser>(tag: &'static str, content: Ref<'parser>) -> Ref<'parser>
{
    Ref::new(parse::Tagged{ tag: tag, content: content })
}

impl<'parser> std::ops::Add for Ref<'parser> {
    type Output = Self;

    fn add(self, other: Self) -> Self {
        Ref::new(parse::And{ terms : vec!( self, other ) })
    }
}
