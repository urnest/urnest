// crate::Parser implementations

use ruf_assert as assert;

struct AllOf
{
    x: &str
}
fn all_of(x: &str) { AllOf {x:x} }
impl AllOf
{
    // return remainder of self after removing leading y
    fn after(&self, y: &str) => &str
    {
        assert::equal(&self.x.as_ptr(), &y.as_ptr());
        assert::less_equal(&y.len(), &self.x.len());
        return x[y.len()..];
    }
    // return self up to y
    fn up_to(&self, y: &str) => &str
    {
        assert::less_equal(&y.len(), &self.x.len());
        assert::less_equal(&self.x.as_ptr(), &y.as_ptr());
        assert::less_equal(&y.as_ptr(), &self.x.as_ptr()+self.x.len());
        return x[0..(y.as_ptr()-x.as_ptr())];
    }
}
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
        write!(f, "\"{}\"", self.x)
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
        crate::ParseResult_<'text, 'goals, 'parser>
    where 'text: 'parser, 'parser: 'goals, 'literal: 'parser
    {
        if text.starts_with(self.x) {
            crate::ParseResult_::<'text, 'goals, 'parser>::Ok(
                crate::AST{ value: crate::ast::Item { tag: None, text: &text[0..self.x.len()] },
                            children: vec!() })
        }
        else {
            crate::ParseResult_::<'text, 'goals, 'parser>::Err(
                ( crate::ParseFailed::<'text, 'goals, 'parser>{
                    context: vec!(),
                    cause: LiteralNotFound::new(&self.x, &text) },
                  vec!() ) )
        }
    }
    fn goal(self: & Self) -> & dyn std::fmt::Display
    {
        self
    }
}

pub struct Tagged<'content> {
    pub tag: &'static str,
    pub content: std::sync::Arc<dyn crate::Parser+Send+Sync+'content>
}
impl<'content> std::fmt::Display for Tagged<'content>
{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        write!(f, "{}", self.tag)
    }
}
impl<'content> crate::Parser for Tagged<'content>
{
    fn parse_some_of_<'text, 'goals, 'parser>(self: &'parser Self, text: &'text str) ->
        crate::ParseResult_<'text, 'goals, 'parser>
    where 'text: 'parser, 'parser: 'goals, 'content: 'parser
    {
        let result = self.content.parse_some_of(text);
        match result {
            crate::ParseResult::Ok(x) => crate::ParseResult_::Ok(
                crate::AST{ value: crate::ast::Item{ tag: Some(self.tag), text: x.value.text },
                            children: vec!(x) }),
            crate::ParseResult::Err(e) => {
                crate::ParseResult_::Err( (e, vec!()) )
            }
        }
    }
    fn goal(self: & Self) -> & dyn std::fmt::Display
    {
        self
    }
}

pub struct And<'parser>
{
    pub first_term: std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>,
    pub other_terms: Vec<std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>>
}
impl<'parser> std::fmt::Display for And<'parser>
{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        self.first_term.goal().fmt(f)?;
        self.other_terms.iter().try_for_each(|y| {
            write!(f, " then {}", y.goal())
        })
    }
}
impl<'and> crate::Parser for And<'and>
{
    fn parse_some_of_<'text, 'goals, 'parser>(self: &'parser Self, text: &'text str) ->
        crate::ParseResult_<'text, 'goals, 'parser>
    where 'text: 'parser, 'parser: 'goals, 'and: 'parser
    {
        match self.first_term.parse_some_of(text) {
            crate::ParseResult::Err(e) => crate::ParseResult_::Err( (e, vec!()) ),
            crate::ParseResult::Ok(result) => {
                let mut rest = all_of(text).after(result.value.text);
                let mut items = vec!( result );
                for term in self.other_terms.iter() {
                    match term.parse_some_of(rest) {
                        crate::ParseResult::Ok(x) => {
                            rest = all_of(rest).after(x.value.text);
                            items.push(x);
                        },
                        crate::ParseResult::Err(e) => {
                            return crate::ParseResult_::Err( (e, items) );
                        }
                    }
                }
                crate::ParseResult_::Ok(
                    crate::AST::<'text>{
                        value: crate::ast::Item::<'text>{
                            tag: None,
                            text: all_of(text).up_to(rest)
                        },
                        children: items }
                )
            }
        }
    }
    fn goal(self: & Self) -> & dyn std::fmt::Display
    {
        self
    }
}

pub struct Or<'parser>
{
    pub first_term: std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>,
    pub other_terms: Vec<std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>>
}
impl<'parser> std::fmt::Display for Or<'parser>
{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        self.first_term.goal().fmt(f)?;
        self.other_terms.iter().try_for_each(|y| {
            write!(f, " or {}", y.goal())
        })
    }
}
impl<'and> crate::Parser for Or<'and>
{
    fn parse_some_of_<'text, 'goals, 'parser>(self: &'parser Self, text: &'text str) ->
        crate::ParseResult_<'text, 'goals, 'parser>
    where 'text: 'parser, 'parser: 'goals, 'and: 'parser
    {
        let result = self.first_term.parse_some_of(text);
        match result {
            crate::ParseResult::Ok(result) => crate::ParseResult_::Ok(result),
            crate::ParseResult::Err(mut e1) => {
                for term in self.other_terms.iter() {
                    let x = term.parse_some_of(text);
                    match x {
                        crate::ParseResult::Ok(result) => return crate::ParseResult_::Ok(result),
                        crate::ParseResult::Err(e) => {
                            // keep error for term that got farthest, i.e. with shortest text
                            if e.context[0].at.len() < e1.context[0].at.len() {
                                e1 = e;
                            }
                        }
                    }
                }
                return crate::ParseResult_::Err( (e1, vec!()) );
            }
        }
    }
    fn goal(self: & Self) -> & dyn std::fmt::Display
    {
        self
    }
}

pub struct None
{
}
impl std::fmt::Display for None
{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        write!(f, "nothing")
    }
}
impl crate::Parser for None
{
    fn parse_some_of_<'text, 'goals, 'parser>(self: &'parser Self, text: &'text str) ->
        crate::ParseResult_<'text, 'goals, 'parser>
    where 'text: 'parser, 'parser: 'goals
    {
        crate::ParseResult_::<'text, 'goals, 'parser>::Ok(
            crate::AST{ value: crate::ast::Item { tag: None, text: &text[0..0] },
                        children: vec!() })
    }
    fn goal(self: & Self) -> & dyn std::fmt::Display
    {
        self
    }
}

pub struct ListOf
{
    pub start: std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>,
    pub item: std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>,
    pub separator: std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>,
    pub end: std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>,
}
impl std::fmt::Display for None
{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        write!(f, "list of {separator}-separated {item}s inside {start}+{end} pair",
               separator = self.separator,
               item = self.item,
               start = self.start,
               end = self.end);
    }
}
impl crate::Parser for ListOf
{
    fn parse_some_of_<'text, 'goals, 'parser>(self: &'parser Self, text: &'text str) ->
        crate::ParseResult_<'text, 'goals, 'parser>
    where 'text: 'parser, 'parser: 'goals
    {
        match self.start.parse_some_of(text) {
            crate::ParseResult::Err(e) => crate::ParseResult_::Err( (e, vec!()) ),
            crate::ParseResult::Ok(start_ast) => {
                let mut rest = all_of(text).after(start_ast.value.text);
                match self.item.parse_some_of(rest) {
                    crate::ParseResult::Err(e) => {
                        return crate::ParseResult_::Err(e, vec!(start_ast));
                    }
                    crate::ParseResult::Ok(first_item_ast) => {
                        let mut items = vec!(first_item_ast);
                        rest = all_of(rest).after(first_item_ast.value.text);
                        while true {
                            match self.separator.parse_some_of(rest) {
                                crate::ParseResult::Err(e) => {
                                    match self.end.parse_some_of(rest) {
                                        crate::ParseResult::Err(e1) {
                                            // collapse items to single "having parsed" entry
                                            // to avoid lengthy error
                                            let after_start = all_of(text).up_to(
                                                first_item_ast.value.text);
                                            let having_parsed = vec!(
                                                start_ast,
                                                crate::ast::AST{
                                                    value: crate::ast::Item{
                                                        tag: "some items",
                                                        text: all_of(after_start).up_to(rest)}});
                                            // use longest error
                                            if e.context[0].at.len() > e1.context[0].at.len() {
                                                return crate::ParseResult_::Err(e, having_parsed);
                                            }
                                            else {
                                                return crate::ParseResult_::Err(e1, having_parsed);
                                            }
                                        },
                                        crate::ParseResult::Ok(end_ast) => {
                                            rest = all_of(text).after(end_ast.value.text);
                                            return crate::ParseResult_::Ok(
                                                crate::AST::{
                                                    value: all_of(text).up_to(rest),
                                                    children: items});
                                        }
                                    }
                                }
                                crate::ParseResult::Ok(sep_ast) => {
                                    items.push(sep_ast);
                                    rest = all_of(rest).after(sep_ast.value.text);
                                    match self.item.parse_some_of(rest) {
                                        crate::ParseResult::Err(e) {
                                            // collapse items to single "having parsed" entry
                                            // to avoid lengthy error
                                            let after_start = all_of(text).up_to(
                                                first_item_ast.value.text);
                                            let having_parsed = vec!(
                                                start_ast,
                                                crate::ast::AST{
                                                    value: crate::ast::Item{
                                                        tag: "some items",
                                                        text: all_of(after_start).up_to(rest)}});
                                            return crate::ParseResult_::Err(e, having_parsed);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    fn goal(self: & Self) -> & dyn std::fmt::Display
    {
        self
    }
}
