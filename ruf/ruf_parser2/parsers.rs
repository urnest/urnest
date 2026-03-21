// crate::Parser implementations

extern crate ruf_assert;

use std::ops::Deref;

use crate::all_of::all_of;

pub struct And<'parser>
{
    pub first_term: std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>,
    pub other_terms: Vec<std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>>
}
impl<'parser> crate::Parser for And<'parser>
{
    fn parse_some_of_<'text, 'parser_ref>(
        self: &'parser_ref Self,
        text: &'text str,
        cache: &mut [crate::Cache<'text, 'parser_ref>]
    ) -> crate::Outcome<'text, 'parser_ref>
    where 'parser: 'parser_ref, 'text: 'parser_ref
    {
        let result_of_first_term = self.first_term.parse_some_of(text, cache);
        match result_of_first_term {
            crate::Outcome::Leaf{
                matched: _,
                then: Some(_)
            } |
            crate::Outcome::Composite{
                matched: None,
                components: _
            } =>
                crate::Outcome::Composite{
                    matched: None,
                    components: vec!( (crate::Goal{parser: self.first_term.deref(), text: text},
                                       result_of_first_term) )},
            crate::Outcome::Leaf{ matched: first_term_matched, then: None} |
            crate::Outcome::Composite{
                matched: Some(first_term_matched),
                components: _
            } => {
                let mut components = vec!( (crate::Goal{parser: self.first_term.deref(), text: text},
                                            result_of_first_term) );
                let mut rest: &str = all_of(text).after(first_term_matched);
                let mut cache = &mut cache[first_term_matched.len()..];
                for term in self.other_terms.iter() {
                    let result_of_term = term.parse_some_of(rest, cache);
                    match result_of_term {
                        crate::Outcome::Leaf{
                            matched: _,
                            then: Some(_)
                        } |
                        crate::Outcome::Composite{
                            matched: None,
                            components: _
                        } =>
                        {
                            components.push( (crate::Goal{parser: term.deref(), text: rest},result_of_term) );
                            return crate::Outcome::Composite{
                                matched: None,
                                components: components};
                        },
                        crate::Outcome::Leaf{
                            matched,
                            then: None
                        } |
                        crate::Outcome::Composite{
                            matched: Some(matched),
                            components: _
                        } => {
                            let t = all_of(rest).after(matched);
                            let c = &mut cache[matched.len()..];
                            components.push( (crate::Goal{parser: term.deref(), text: rest},result_of_term) );
                            rest = t;
                            cache = c;
                        }
                    }
                }
                crate::Outcome::Composite{
                    matched: Some(all_of(text).up_to(rest)),
                    components: components}
            }
        }
    }
}

pub struct Or<'parser>
{
    pub first_term: std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>,
    pub other_terms: Vec<std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>>
}
impl<'or> crate::Parser for Or<'or>
{
    fn parse_some_of_<'text, 'parser_ref>(
        self: &'parser_ref Self,
        text: &'text str,
        cache: &mut [crate::Cache<'text, 'parser_ref>]
    ) -> crate::Outcome<'text, 'parser_ref>
    where 'text: 'parser_ref, 'or: 'parser_ref
    {
        let result_of_first_term = self.first_term.parse_some_of(text, cache);
        match result_of_first_term {
            crate::Outcome::Leaf{
                matched,
                then: None
            } |
            crate::Outcome::Composite{
                matched: Some(matched),
                components: _
            } =>
                crate::Outcome::Composite{
                    matched: Some(matched),
                    components: vec!( (crate::Goal{parser: self.first_term.deref(), text: text},
                                       result_of_first_term) )},

            crate::Outcome::Leaf{ matched: _, then: Some(_)} |
            crate::Outcome::Composite{
                matched: None,
                components: _
            } => {
                let mut components = vec!( (crate::Goal{parser: self.first_term.deref(), text: text}, result_of_first_term) );
                for term in self.other_terms.iter() {
                    let result_of_term = term.parse_some_of(text, cache);
                    match result_of_term {
                        crate::Outcome::Leaf{
                            matched,
                            then: None
                        } |
                        crate::Outcome::Composite{
                            matched: Some(matched),
                            components: _
                        } =>
                        {
                            components.push( (crate::Goal{parser: term.deref(), text: text}, result_of_term) );
                            return crate::Outcome::Composite{
                                matched: Some(matched),
                                components: components};
                        },
                        crate::Outcome::Leaf{
                            matched: _,
                            then: Some(_)
                        } |
                        crate::Outcome::Composite{
                            matched: None,
                            components: _
                        } =>
                            components.push( (crate::Goal{parser: term.deref(), text: text}, result_of_term) )
                    }
                }
                crate::Outcome::Composite{
                    matched: None,
                    components: components}
            }
        }
    }
}

pub struct None {}

impl crate::Parser for None
{
    fn parse_some_of_<'text, 'parser_ref>(
        self: &'parser_ref Self,
        text: &'text str,
        _cache: &mut [crate::Cache<'text, 'parser_ref>]
    ) -> crate::Outcome<'text, 'parser_ref>
    where 'text: 'parser_ref
    {
        crate::Outcome::Leaf{
            matched: &text[0..0],
            then: None
        }
    }
}

pub struct Literal<'parser> {
    pub x: &'parser str
}

impl<'parser> crate::Parser for Literal<'parser>
{
    fn parse_some_of_<'text, 'parser_ref>(
        self: &'parser_ref Self,
        text: &'text str,
        _cache: &mut [crate::Cache<'text, 'parser_ref>]
    ) -> crate::Outcome<'text, 'parser_ref>
    where 'parser: 'parser_ref, 'text: 'parser_ref
    {
        // we would use starts_with but when it doesn't start with we want the longest
        // having_parsed we can in error, so we use our own loop.
        // REVISIT: this also ignores unicode grapheme clusters
        let mut i = self.x.char_indices();
        let mut j = text.char_indices();
        loop {
            match (i.next(), j.next()) {
                (None, None) => {
                    return crate::Outcome::Leaf
                        { matched: text, then: None };
                },
                (None, Some((n, _))) => {
                    return crate::Outcome::Leaf
                        { matched: &text[0..n] , then: None };
                },
                (Some( (n, c1)), b) => {
                    match b {
                        Some( (_, c2) ) => {
                            if c1 == c2 { continue; }
                            return crate::Outcome::Leaf
                                { matched: &text[0..n],
                                  then: Some(crate::Unexpected::Char)};
                        },
                        None => return crate::Leaf
                            { matched: &text[0..n],
                              then: Some(crate::Unexpected::EndOfInput)}
                    }
                }
            }
        }
    }
}

pub struct EndOfInput {
}

impl crate::Parser for EndOfInput
{
    fn parse_some_of_<'text, 'parser_ref>(
        self: &'parser_ref Self,
        text: &'text str,
        _cache: &mut [crate::Cache]
    ) -> crate::Outcome<'text, 'parser_ref>
    where 'text: 'parser_ref
    {
        match text {
            "" => {
                return crate::Outcome::Leaf
                    { matched: text, then: None };
            },
            _ => {
                return crate::Outcome::Leaf
                    { matched: &text[0..0],
                      then: Some(crate::Unexpected::Char)};
            }
        }
    }
}

pub struct Tagged<'parser> {
    pub tag: &'static str,
    pub content: std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>
}
impl<'parser> crate::Parser for Tagged<'parser>
{
    fn tag(&self) -> Option<&'static str>
    {
        Some(self.tag)
    }
    fn parse_some_of_<'text, 'parser_ref>(
        self: &'parser_ref Self,
        text: &'text str,
        cache: &mut [crate::Cache<'text, 'parser_ref>]
    ) -> crate::Outcome<'text, 'parser_ref>
    where 'text: 'parser_ref, 'parser: 'parser_ref
    {
        let result = self.content.parse_some_of(text, cache);
        match result {
            crate::Outcome::Leaf{
                matched,
                then: None
            } |
            crate::Outcome::Composite{
                matched: Some(matched),
                components: _
            } =>
                crate::Outcome::Composite{
                    matched: Some(matched),
                    components: vec!(
                        (crate::Goal{parser: self.content.deref(), text: text}, result),)},

            crate::Outcome::Leaf{
                matched: _,
                then: Some(_)
            } |
            crate::Outcome::Composite{
                matched: None,
                components: _
            } =>
                crate::Outcome::Composite{
                    matched: None,
                    components: vec!(
                        (crate::Goal{parser: self.content.deref(), text: text}, result),)},
        }
    }
}


pub struct AnyChar {}
impl crate::Parser for AnyChar
{
    fn parse_some_of_<'text, 'parser_ref>(
        self: &'parser_ref Self,
        text: &'text str,
        _cache: &mut [crate::Cache<'text, 'parser_ref>]
    ) -> crate::Outcome<'text, 'parser_ref>
    where 'text: 'parser_ref
    {
        match text.chars().next() {
            Some(_c) => {
                return crate::Outcome::Leaf{
                    matched: &text[0..1],
                    then: None
                };
            },
            None => {
                crate::Outcome::Leaf{
                    matched: &text[0..0],
                    then: Some(crate::Unexpected::EndOfInput)
                }
            }
        }
    }
}

pub struct Digit {}

impl crate::Parser for Digit
{
    fn parse_some_of_<'text, 'parser_ref>(
        self: &'parser_ref Self,
        text: &'text str,
        _cache: &mut [crate::Cache<'text, 'parser_ref>]
    ) -> crate::Outcome<'text, 'parser_ref>
    where 'text: 'parser_ref
    {
        match text.chars().next() {
            Some(c) => {
                if '0' <= c && c <= '9' {
                    return crate::Outcome::Leaf{
                        matched: &text[0..1],
                        then: None
                    };
                }
                crate::Outcome::Leaf{
                    matched: &text[0..0],
                    then: Some(crate::Unexpected::Char)
                }
            },
            None => {
                crate::Outcome::Leaf{
                    matched: &text[0..0],
                    then: Some(crate::Unexpected::EndOfInput)
                }
            }
        }
    }
}

pub struct Char {
    pub x: char
}

impl crate::Parser for Char
{
    fn parse_some_of_<'text, 'parser_ref>(
        self: &'parser_ref Self,
        text: &'text str,
        _cache: &mut [crate::Cache<'text, 'parser_ref>]
    ) -> crate::Outcome<'text, 'parser_ref>
    where 'text: 'parser_ref
    {
        match text.chars().next() {
            Some(c) => {
                if c == self.x {
                    return crate::Outcome::Leaf{
                        matched: &text[0..1],
                        then: None
                    };
                }
                crate::Outcome::Leaf{
                    matched: &text[0..0],
                    then: Some(crate::Unexpected::Char)
                }
            },
            None => {
                crate::Outcome::Leaf{
                    matched: &text[0..0],
                    then: Some(crate::Unexpected::EndOfInput)
                }
            }
        }
    }
}
