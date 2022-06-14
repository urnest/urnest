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
        crate::ParseResult<'text, 'goals, 'parser>
    where 'text: 'parser, 'parser: 'goals, 'and: 'parser
    {
        let (result, mut rest) = self.first_term.parse_some_of(text)?;
        let mut items = vec!( result );
        for term in self.other_terms.iter() {
            match term.parse_some_of(rest) {
                crate::ParseResult::Ok(x) => {
                    items.push(x.0);
                    rest = x.1;
                },
                crate::ParseResult::Err(e) => {
                    return crate::ParseResult::Err(e);
                }
            }
        }
        return crate::ParseResult::Ok( (
            crate::AST::<'text>{
                value: crate::ast::Item::<'text>{
                    tag: None,
                    text: text.split_at(text.len() - rest.len()).0
                },
                children: items },
            rest )
        );
    }
    fn goal(self: & Self) -> & dyn std::fmt::Display
    {
        self
    }
}
