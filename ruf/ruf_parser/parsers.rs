// crate::Parser implementations

extern crate ruf_assert;

use ruf_assert as assert;

struct AllOf<'x>
{
    x: &'x str
}
fn all_of<'x>(x: &'x str) -> AllOf<'x>{ AllOf {x:x} }

impl<'x> AllOf<'x>
{
    // return remainder of self after removing leading y
    fn after(&self, y: &str) -> &'x str
    {
        assert::equal(&self.x.as_ptr(), &y.as_ptr());
        assert::less_equal(&y.len(), &self.x.len());
        return &self.x[y.len()..];
    }
    // return self up to trailing y
    fn up_to(&self, y: &str) -> &'x str
    {
        assert::less_equal(&y.len(), &self.x.len());
        assert::less_equal(&self.x.as_ptr(), &y.as_ptr());
        return &self.x[0..self.x.len()-y.len()]
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
        // we would use starts_with but when it doesn't start with we want the longest
        // having_parsed we can in error, so we use our own loop.
        // REVISIT: this also ignores unicode grapheme clusters
        let mut i = self.x.char_indices();
        let mut j = text.char_indices();
        loop {
            match (i.next(), j.next()) {
                (None, None) => {
                    return crate::ParseResult_::<'text, 'goals, 'parser>::Ok(
                        crate::AST{ value: crate::ast::Item { tag: None, text: text },
                                    children: vec!() })
                },
                (None, Some((m, _))) => {
                    return crate::ParseResult_::<'text, 'goals, 'parser>::Ok(
                        crate::AST{ value: crate::ast::Item { tag: None, text: &text[0..m] },
                                    children: vec!() })
                },
                (Some( (n, c1)), b) => {
                    match b {
                        Some( (_, c2) ) => {
                            if c1 == c2 { continue; }
                        },
                        _ => ()
                    }
                    return crate::ParseResult_::<'text, 'goals, 'parser>::Err(
                        ( crate::ParseFailed::<'text, 'goals, 'parser>{
                            context: vec!(),
                            cause: LiteralNotFound::new(&self.x[n..], &text[n..]) },
                          match n {
                              0 => { vec!() },
                              _ => { vec!(crate::AST{
                                  value: crate::ast::Item{
                                      tag: None,
                                      text: &text[0..n]},
                                  children: vec!() } ) } } ));
                }
            }
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
                let mut rest: &str = all_of(text).after(result.value.text);
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
                            e1 = crate::best_of(e, e1);
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

pub struct ListOf<'p1>
{
    pub start: std::sync::Arc<dyn crate::Parser+Send+Sync+'p1>,
    pub item: std::sync::Arc<dyn crate::Parser+Send+Sync+'p1>,
    pub separator: std::sync::Arc<dyn crate::Parser+Send+Sync+'p1>,
    pub end: std::sync::Arc<dyn crate::Parser+Send+Sync+'p1>,
}

impl<'p1> std::fmt::Display for ListOf<'p1>
{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        write!(f, "list of {separator}-separated {item} items inside {start}..{end}",
               separator = self.separator.goal(),
               item = self.item.goal(),
               start = self.start.goal(),
               end = self.end.goal())
    }
}
impl<'p1> crate::Parser for ListOf<'p1>
{
    fn parse_some_of_<'text, 'goals, 'parser>(self: &'parser Self, text: &'text str) ->
        crate::ParseResult_<'text, 'goals, 'parser>
    where 'text: 'parser, 'parser: 'goals, 'p1: 'parser
    {
        let mut rest = text;
        match self.start.parse_some_of(rest) {
            crate::ParseResult::Err(e) => crate::ParseResult_::Err( (e, vec!()) ),
            crate::ParseResult::Ok(start_ast) => {
                rest = all_of(text).after(start_ast.value.text);
                let text_after_start_ast = rest;
                match self.item.parse_some_of(rest) {
                    crate::ParseResult::Err(e) => {
                        return crate::ParseResult_::Err( (e, vec!(start_ast)) );
                    }
                    crate::ParseResult::Ok(first_item_ast) => {
                        rest = all_of(rest).after(first_item_ast.value.text);
                        let mut items = vec!(start_ast, first_item_ast);
                        let err_ = |e: crate::ParseFailed::<'text, 'goals, 'parser>,
                                    mut items: Vec<crate::AST<'text>>,
                                    rest: &str|
                        {
                            // collapse items to single "having parsed" entry
                            // to avoid lengthy error
                            let start_ast = items.remove(0);
                            let having_parsed = vec!(
                                start_ast,
                                crate::AST{
                                    value: crate::ast::Item{
                                        tag: Some("some items"),
                                        text: all_of(text_after_start_ast).up_to(rest)},
                                    children: items });
                            return crate::ParseResult_::Err( (e, having_parsed) );
                        };
                        loop {
                            match self.separator.parse_some_of(rest) {
                                crate::ParseResult::Err(e) => {
                                    match self.end.parse_some_of(rest) {
                                        crate::ParseResult::Err(e1) => {
                                            return err_(crate::best_of(e, e1), items, rest);
                                        },
                                        crate::ParseResult::Ok(end_ast) => {
                                            rest = all_of(rest).after(end_ast.value.text);
                                            items.push(end_ast);
                                            return crate::ParseResult_::Ok(
                                                crate::AST{
                                                    value: crate::ast::Item{
                                                        tag: None,
                                                        text: all_of(text).up_to(rest)},
                                                    children: items});
                                        }
                                    }
                                }
                                crate::ParseResult::Ok(sep_ast) => {
                                    rest = all_of(rest).after(sep_ast.value.text);
                                    items.push(sep_ast);
                                    match self.item.parse_some_of(rest) {
                                        crate::ParseResult::Err(e) => {
                                            return err_(e, items, rest);
                                        },
                                        crate::ParseResult::Ok(item) => {
                                            rest = all_of(rest).after(item.value.text);
                                            items.push(item);
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
