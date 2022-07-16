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

struct EndOfInput {}
impl std::fmt::Display for EndOfInput {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        write!(f, "end of input")
    }
}
fn end_of_input() -> Box<dyn std::fmt::Display> { Box::new(EndOfInput{}) }

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
fn literal_not_found<'cause>(x:&'cause str, text:&'cause str) -> Box<dyn std::fmt::Display+'cause>
{
    Box::new(LiteralNotFound::<'cause>{x: &x, text: &text})
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
                            cause: literal_not_found(&self.x[n..], &text[n..]) },
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
impl<'or> crate::Parser for Or<'or>
{
    fn parse_some_of_<'text, 'goals, 'parser>(self: &'parser Self, text: &'text str) ->
        crate::ParseResult_<'text, 'goals, 'parser>
    where 'text: 'parser, 'parser: 'goals, 'or: 'parser
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

struct WrongChar {
    wanted: char,
    got: char
}
fn wrong_char(wanted: char, got: char) -> Box<dyn std::fmt::Display>
{
    Box::new(WrongChar{wanted: wanted, got: got})
}

impl std::fmt::Display for WrongChar {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        // REVISIT: need a rust-escape-string formatting function so that e.g. \n
        // formats as '\n'
        write!(f, "expected '{x}' not '{y}'", x=self.wanted, y=self.got)
    }
}

pub struct Char {
    pub tag: Option<&'static str>,
    pub x: char
}
impl std::fmt::Display for Char
{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        match self.tag {
            Some(tag) => { write!(f, "{}", tag) },
            // REVISIT: need rust-escape-char function so \n displays as '\n'
            None => { write!(f, "'{}'", self.x) }
        }
    }
}
impl crate::Parser for Char
{
    fn parse_some_of_<'text, 'goals, 'parser>(self: &'parser Self, text: &'text str) ->
        crate::ParseResult_<'text, 'goals, 'parser>
    where 'text: 'parser, 'parser: 'goals
    {
        match text.chars().next() {
            Some(c) => {
                if c == self.x {
                    return crate::ParseResult_::<'text, 'goals, 'parser>::Ok(
                        crate::AST{ value: crate::ast::Item { tag: self.tag, text: &text[0..1] },
                                    children: vec!() });
                }
                crate::ParseResult_::<'text, 'goals, 'parser>::Err(
                    ( crate::ParseFailed::<'text, 'goals, 'parser>{
                        context: vec!(),
                        cause: wrong_char(self.x, c) },
                      vec!()))
            },
            None => {
                crate::ParseResult_::<'text, 'goals, 'parser>::Err(
                    ( crate::ParseFailed::<'text, 'goals, 'parser>{
                        context: vec!(),
                        cause: end_of_input() },
                      vec!()))
            }
        }
    }
    fn goal(self: & Self) -> & dyn std::fmt::Display
    {
        self
    }
}

struct NotDigit {
    got: char
}
fn not_digit(got: char) -> Box<dyn std::fmt::Display>
{
    Box::new(NotDigit{got: got})
}

impl std::fmt::Display for NotDigit {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        // REVISIT: need a rust-escape-string formatting function so that e.g. \n
        // formats as '\n'
        write!(f, "expected 0..9 not '{y}'", y=self.got)
    }
}

pub struct Digit {
    pub tag: Option<&'static str>
}
impl std::fmt::Display for Digit
{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        match self.tag {
            Some(tag) => { write!(f, "{}", tag) },
            None => { write!(f, "digit") }
        }
    }
}
impl crate::Parser for Digit
{
    fn parse_some_of_<'text, 'goals, 'parser>(self: &'parser Self, text: &'text str) ->
        crate::ParseResult_<'text, 'goals, 'parser>
    where 'text: 'parser, 'parser: 'goals
    {
        match text.chars().next() {
            Some(c) => {
                if '0' <= c && c <= '9' {
                    return crate::ParseResult_::<'text, 'goals, 'parser>::Ok(
                        crate::AST{ value: crate::ast::Item { tag: self.tag, text: &text[0..1] },
                                    children: vec!() });
                }
                crate::ParseResult_::<'text, 'goals, 'parser>::Err(
                    ( crate::ParseFailed::<'text, 'goals, 'parser>{
                        context: vec!(),
                        cause: not_digit(c) },
                      vec!()))
            },
            None => {
                crate::ParseResult_::<'text, 'goals, 'parser>::Err(
                    ( crate::ParseFailed::<'text, 'goals, 'parser>{
                        context: vec!(),
                        cause: end_of_input() },
                      vec!()))
            }
        }
    }
    fn goal(self: & Self) -> & dyn std::fmt::Display
    {
        self
    }
}

struct NotOctalDigit {
    got: char
}
fn not_octal_digit(got: char) -> Box<dyn std::fmt::Display>
{
    Box::new(NotOctalDigit{got: got})
}

impl std::fmt::Display for NotOctalDigit {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        // REVISIT: need a rust-escape-string formatting function so that e.g. \n
        // formats as '\n'
        write!(f, "expected 0..7 not '{y}'", y=self.got)
    }
}

pub struct OctalDigit {
    pub tag: Option<&'static str>
}
impl std::fmt::Display for OctalDigit
{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        match self.tag {
            Some(tag) => { write!(f, "{}", tag) },
            None => { write!(f, "octal digit") }
        }
    }
}
impl crate::Parser for OctalDigit
{
    fn parse_some_of_<'text, 'goals, 'parser>(self: &'parser Self, text: &'text str) ->
        crate::ParseResult_<'text, 'goals, 'parser>
    where 'text: 'parser, 'parser: 'goals
    {
        match text.chars().next() {
            Some(c) => {
                if '0' <= c && c <= '7' {
                    return crate::ParseResult_::<'text, 'goals, 'parser>::Ok(
                        crate::AST{ value: crate::ast::Item { tag: self.tag, text: &text[0..1] },
                                    children: vec!() });
                }
                crate::ParseResult_::<'text, 'goals, 'parser>::Err(
                    ( crate::ParseFailed::<'text, 'goals, 'parser>{
                        context: vec!(),
                        cause: not_octal_digit(c) },
                      vec!()))
            },
            None => {
                crate::ParseResult_::<'text, 'goals, 'parser>::Err(
                    ( crate::ParseFailed::<'text, 'goals, 'parser>{
                        context: vec!(),
                        cause: end_of_input() },
                      vec!()))
            }
        }
    }
    fn goal(self: & Self) -> & dyn std::fmt::Display
    {
        self
    }
}

struct NotHexDigit {
    got: char
}
fn not_hex_digit(got: char) -> Box<dyn std::fmt::Display>
{
    Box::new(NotHexDigit{got: got})
}

impl std::fmt::Display for NotHexDigit {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        // REVISIT: need a rust-escape-string formatting function so that e.g. \n
        // formats as '\n'
        write!(f, "expected 0..7, a..f or A..F not '{y}'", y=self.got)
    }
}

pub struct HexDigit {
    pub tag: Option<&'static str>
}
impl std::fmt::Display for HexDigit
{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        match self.tag {
            Some(tag) => { write!(f, "{}", tag) },
            None => { write!(f, "hex digit") }
        }
    }
}
impl crate::Parser for HexDigit
{
    fn parse_some_of_<'text, 'goals, 'parser>(self: &'parser Self, text: &'text str) ->
        crate::ParseResult_<'text, 'goals, 'parser>
    where 'text: 'parser, 'parser: 'goals
    {
        match text.chars().next() {
            Some(c) => {
                if ('0' <= c && c <= '7') || ('a'<=c &&  c<='f') || ('A'<=c && c<='F'){
                    return crate::ParseResult_::<'text, 'goals, 'parser>::Ok(
                        crate::AST{ value: crate::ast::Item { tag: self.tag, text: &text[0..1] },
                                    children: vec!() });
                }
                crate::ParseResult_::<'text, 'goals, 'parser>::Err(
                    ( crate::ParseFailed::<'text, 'goals, 'parser>{
                        context: vec!(),
                        cause: not_hex_digit(c) },
                      vec!()))
            },
            None => {
                crate::ParseResult_::<'text, 'goals, 'parser>::Err(
                    ( crate::ParseFailed::<'text, 'goals, 'parser>{
                        context: vec!(),
                        cause: end_of_input() },
                      vec!()))
            }
        }
    }
    fn goal(self: & Self) -> & dyn std::fmt::Display
    {
        self
    }
}

struct NotUsAsciiPrintable {
    got: char
}
fn not_us_ascii_printable(got: char) -> Box<dyn std::fmt::Display>
{
    Box::new(NotUsAsciiPrintable{got: got})
}

impl std::fmt::Display for NotUsAsciiPrintable {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        // REVISIT: need a rust-escape-string formatting function so that e.g. \n
        // formats as '\n'
        write!(f, "expected a..z, A..Z, 0..9, space, one of {various} or delete '{y}'",
               various="!\"#$%&'()*+,-./:;<=>?@[\\]^_{|}~",y=self.got)
    }
}

pub struct UsAsciiPrintable {
    pub tag: Option<&'static str>
}
impl std::fmt::Display for UsAsciiPrintable
{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        match self.tag {
            Some(tag) => { write!(f, "{}", tag) },
            None => { write!(f, "US ASCII printable characcter") }
        }
    }
}
impl crate::Parser for UsAsciiPrintable
{
    fn parse_some_of_<'text, 'goals, 'parser>(self: &'parser Self, text: &'text str) ->
        crate::ParseResult_<'text, 'goals, 'parser>
    where 'text: 'parser, 'parser: 'goals
    {
        match text.chars().next() {
            Some(c) => {
                if char::from_u32(32).unwrap() <= c && c <= char::from_u32(127).unwrap() {
                    return crate::ParseResult_::<'text, 'goals, 'parser>::Ok(
                        crate::AST{ value: crate::ast::Item { tag: self.tag, text: &text[0..1] },
                                    children: vec!() });
                }
                crate::ParseResult_::<'text, 'goals, 'parser>::Err(
                    ( crate::ParseFailed::<'text, 'goals, 'parser>{
                        context: vec!(),
                        cause: not_us_ascii_printable(c) },
                      vec!()))
            },
            None => {
                crate::ParseResult_::<'text, 'goals, 'parser>::Err(
                    ( crate::ParseFailed::<'text, 'goals, 'parser>{
                        context: vec!(),
                        cause: end_of_input() },
                      vec!()))
            }
        }
    }
    fn goal(self: & Self) -> & dyn std::fmt::Display
    {
        self
    }
}


// struct NotOneOfChars {
//     wanted: crate::Chars,
//     got: char
// }
// fn not_us_ascii_printable(got: char) -> Box<dyn std::fmt::Display>
// {
//     Box::new(NotUsAsciiPrintable{got: got})
// }

// impl std::fmt::Display for NotUsAsciiPrintable {
//     fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
//     {
//         // REVISIT: need a rust-escape-string formatting function so that e.g. \n
//         // formats as '\n'
//         write!(f, "expected a..z, A..Z, 0..9, space, one of {various} or delete '{y}'",
//                various="!\"#$%&'()*+,-./:;<=>?@[\\]^_{|}~",y=self.got)
//     }
// }

// pub struct UsAsciiPrintable {
//     pub tag: Option<&'static str>
// }
// impl std::fmt::Display for UsAsciiPrintable
// {
//     fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
//     {
//         match self.tag {
//             Some(tag) => { write!(f, "{}", tag) },
//             None => { write!(f, "US ASCII printable characcter") }
//         }
//     }
// }
// impl crate::Parser for UsAsciiPrintable
// {
//     fn parse_some_of_<'text, 'goals, 'parser>(self: &'parser Self, text: &'text str) ->
//         crate::ParseResult_<'text, 'goals, 'parser>
//     where 'text: 'parser, 'parser: 'goals
//     {
//         match text.chars().next() {
//             Some(c) => {
//                 if char::from_u32(32).unwrap() <= c && c <= char::from_u32(127).unwrap() {
//                     return crate::ParseResult_::<'text, 'goals, 'parser>::Ok(
//                         crate::AST{ value: crate::ast::Item { tag: self.tag, text: &text[0..1] },
//                                     children: vec!() });
//                 }
//                 crate::ParseResult_::<'text, 'goals, 'parser>::Err(
//                     ( crate::ParseFailed::<'text, 'goals, 'parser>{
//                         context: vec!(),
//                         cause: not_us_ascii_printable(c) },
//                       vec!()))
//             },
//             None => {
//                 crate::ParseResult_::<'text, 'goals, 'parser>::Err(
//                     ( crate::ParseFailed::<'text, 'goals, 'parser>{
//                         context: vec!(),
//                         cause: end_of_input() },
//                       vec!()))
//             }
//         }
//     }
//     fn goal(self: & Self) -> & dyn std::fmt::Display
//     {
//         self
//     }
// }

pub struct AnyChar {}
impl std::fmt::Display for AnyChar
{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        write!(f, "any character")
    }
}
impl crate::Parser for AnyChar
{
    fn parse_some_of_<'text, 'goals, 'parser>(self: &'parser Self, text: &'text str) ->
        crate::ParseResult_<'text, 'goals, 'parser>
    where 'text: 'parser, 'parser: 'goals
    {
        match text.chars().next() {
            Some(_) => {
                return crate::ParseResult_::<'text, 'goals, 'parser>::Ok(
                    crate::AST{ value: crate::ast::Item { tag: None, text: &text[0..1] },
                                children: vec!() });
            },
            None => {
                crate::ParseResult_::<'text, 'goals, 'parser>::Err(
                    ( crate::ParseFailed::<'text, 'goals, 'parser>{
                        context: vec!(),
                        cause: end_of_input() },
                      vec!()))
            }
        }
    }
    fn goal(self: & Self) -> & dyn std::fmt::Display
    {
        self
    }
}

pub struct AtLeastOne<'p1>
{
    pub x: std::sync::Arc<dyn crate::Parser+Send+Sync+'p1>,
}

impl<'p1> std::fmt::Display for AtLeastOne<'p1>
{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        write!(f, "at least one {item}", item = self.x.goal())
    }
}
impl<'p1> crate::Parser for AtLeastOne<'p1>
{
    fn parse_some_of_<'text, 'goals, 'parser>(self: &'parser Self, text: &'text str) ->
        crate::ParseResult_<'text, 'goals, 'parser>
    where 'text: 'parser, 'parser: 'goals, 'p1: 'parser
    {
        let mut rest = text;
        match self.x.parse_some_of(rest) {
            crate::ParseResult::Err(e) => crate::ParseResult_::Err( (e, vec!()) ),
            crate::ParseResult::Ok(first_item_ast) => {
                rest = all_of(rest).after(first_item_ast.value.text);
                let mut items = vec!(first_item_ast);
                loop {
                    match self.x.parse_some_of(rest) {
                        crate::ParseResult::Err(_) => {
                            return crate::ParseResult_::Ok(
                                crate::AST{
                                    value: crate::ast::Item{
                                        tag: None,
                                        text: all_of(text).up_to(rest)},
                                    children: items});
                        }
                        crate::ParseResult::Ok(item) => {
                            rest = all_of(rest).after(item.value.text);
                            items.push(item);
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

pub fn parse_charset(chars: &crate::CharSet) -> std::collections::HashSet<char>
{
    let c1 = "c1";
    let c2 = "c2";
    let c = "c";
    let range = "range";
    let parser = crate::at_least_one(
        crate::tagged(range,
                      crate::tagged(c1, crate::any_char()) +
                      crate::literal("-") +
                      crate::tagged(c2, crate::any_char()) ) |
        crate::tagged(c, crate::any_char()));
    let x = parser.parse_some_of(chars.value);
    match x {
        crate::ParseResult::Ok(ast) => {
            let mut result = std::collections::HashSet::new();
            ast.select_by_value(&|v| v.tag == Some(range)).iter().for_each(
                |range_ast| {
                    let c1 = range_ast.select_by_value(&|v| v.tag == Some(c1)).iter().next()
                        .unwrap().value.text.chars().next().unwrap();
                    let c2 = range_ast.select_by_value(&|v| v.tag == Some(c2)).iter().next()
                        .unwrap().value.text.chars().next().unwrap();
                    if c1 <= c2 {
                        for c in c1..=c2 { result.insert(c); }
                        result.insert(c2);
                    }
                    else {
                        result.insert(c1);
                        result.insert('-');
                        result.insert(c2);
                    }
                });
            ast.select_by_value(&|v| v.tag == Some(c)).iter().for_each(
                |ast| { result.insert(ast.value.text.chars().next().unwrap()); });
            result
        },
        // only way to fail is empty pattern
        crate::ParseResult::Err(_) => std::collections::HashSet::new()
    }
}

struct UnexpectedChar {
    wanted: crate::CharSet,
    got: char
}
fn unexpected_char(wanted: crate::CharSet, got: char) -> Box<dyn std::fmt::Display>
{
    Box::new(UnexpectedChar{wanted: wanted, got: got})
}

impl std::fmt::Display for UnexpectedChar {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        // REVISIT: need a rust-escape-string formatting function so that e.g. \n
        // formats as '\n'
        write!(f, "expected one of characters {pattern} not '{y}'", pattern=self.wanted.value, y=self.got)
    }
}

pub struct OneOfChars {
    pub pattern: crate::CharSet,
    pub chars: std::collections::HashSet<char>
}
impl std::fmt::Display for OneOfChars
{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result
    {
        write!(f, "one of characters {pattern}", pattern=self.pattern)
    }
}
impl crate::Parser for OneOfChars
{
    fn parse_some_of_<'text, 'goals, 'parser>(self: &'parser Self, text: &'text str) ->
        crate::ParseResult_<'text, 'goals, 'parser>
    where 'text: 'parser, 'parser: 'goals
    {
        match text.chars().next() {
            Some(c) => {
                if self.chars.contains(&c) {
                    return crate::ParseResult_::<'text, 'goals, 'parser>::Ok(
                        crate::AST{ value: crate::ast::Item { tag: None, text: &text[0..1] },
                                    children: vec!() });
                }
                crate::ParseResult_::<'text, 'goals, 'parser>::Err(
                    ( crate::ParseFailed::<'text, 'goals, 'parser>{
                        context: vec!(),
                        cause: unexpected_char(self.pattern, c) },
                      vec!()))
            },
            None => {
                crate::ParseResult_::<'text, 'goals, 'parser>::Err(
                    ( crate::ParseFailed::<'text, 'goals, 'parser>{
                        context: vec!(),
                        cause: end_of_input() },
                      vec!()))
            }
        }
    }
    fn goal(self: & Self) -> & dyn std::fmt::Display
    {
        self
    }
}
