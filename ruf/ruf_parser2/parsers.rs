// crate::Parser implementations

extern crate ruf_assert;

use std::ops::Deref;
use std::sync::Arc;

use crate::all_of::all_of;

pub struct And<'parser>
{
    pub first_term: Arc<dyn crate::Parser+Send+Sync+'parser>,
    pub other_terms: Vec<Arc<dyn crate::Parser+Send+Sync+'parser>>
}
impl<'parser> crate::Parser for And<'parser>
{
    fn parse_some_of_<'text, 'parser_ref, 'backrefs, 'result>(
        self: &'parser_ref Self,
        text: &'text str,
        cache: &mut [crate::Cache<'text, 'parser_ref>],
        backrefs: &'parser_ref [crate::BackReffable<'backrefs>]
    ) -> crate::Outcome<'text, 'result>
    where 'parser: 'parser_ref, 'text: 'parser_ref, 'backrefs: 'parser_ref, 'backrefs: 'result, 'parser_ref: 'result
    {
        let result_of_first_term = self.first_term.parse_some_of(text, cache, backrefs);
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
                    let result_of_term = term.parse_some_of(rest, cache, backrefs);
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
    pub first_term: Arc<dyn crate::Parser+Send+Sync+'parser>,
    pub other_terms: Vec<Arc<dyn crate::Parser+Send+Sync+'parser>>
}
impl<'or> crate::Parser for Or<'or>
{
    fn parse_some_of_<'text, 'parser_ref, 'backrefs, 'result>(
        self: &'parser_ref Self,
        text: &'text str,
        cache: &mut [crate::Cache<'text, 'parser_ref>],
        backrefs: &'parser_ref [crate::BackReffable<'backrefs>]
    ) -> crate::Outcome<'text, 'result>
    where 'text: 'parser_ref, 'or: 'parser_ref, 'backrefs: 'parser_ref, 'backrefs: 'result, 'parser_ref: 'result
    {
        let result_of_first_term = self.first_term.parse_some_of(text, cache, backrefs);
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
                    let result_of_term = term.parse_some_of(text, cache, backrefs);
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
    fn parse_some_of_<'text, 'parser_ref, 'backrefs, 'result>(
        self: &'parser_ref Self,
        text: &'text str,
        _cache: &mut [crate::Cache<'text, 'parser_ref>],
        _backrefs: &[crate::BackReffable<'backrefs>]
    ) -> crate::Outcome<'text, 'result>
    where 'text: 'parser_ref, 'backrefs: 'result, 'parser_ref: 'result
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
    fn parse_some_of_<'text, 'parser_ref, 'backrefs, 'result>(
        self: &'parser_ref Self,
        text: &'text str,
        _cache: &mut [crate::Cache<'text, 'parser_ref>],
        _backrefs: &[crate::BackReffable<'backrefs>]
    ) -> crate::Outcome<'text, 'result>
    where 'parser: 'parser_ref, 'text: 'parser_ref, 'backrefs: 'result, 'parser_ref: 'result
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
    fn parse_some_of_<'text, 'parser_ref, 'backrefs, 'result>(
        self: &'parser_ref Self,
        text: &'text str,
        _cache: &mut [crate::Cache],
        _backrefs: &[crate::BackReffable<'backrefs>]
    ) -> crate::Outcome<'text, 'result>
    where 'text: 'parser_ref, 'backrefs: 'result, 'parser_ref: 'result
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
    pub content: Arc<dyn crate::Parser+Send+Sync+'parser>
}
impl<'parser> crate::Parser for Tagged<'parser>
{
    fn tag(&self) -> Option<&'static str>
    {
        Some(self.tag)
    }
    fn parse_some_of_<'text, 'parser_ref, 'backrefs, 'result>(
        self: &'parser_ref Self,
        text: &'text str,
        cache: &mut [crate::Cache<'text, 'parser_ref>],
        backrefs: &'parser_ref [crate::BackReffable<'backrefs>]
    ) -> crate::Outcome<'text, 'result>
    where 'parser: 'parser_ref, 'text: 'parser_ref, 'backrefs: 'parser_ref, 'backrefs: 'result, 'parser_ref: 'result
    {
        let result = self.content.parse_some_of(text, cache, backrefs);
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
    fn parse_some_of_<'text, 'parser_ref, 'backrefs, 'result>(
        self: &'parser_ref Self,
        text: &'text str,
        _cache: &mut [crate::Cache<'text, 'parser_ref>],
        _backrefs: &'parser_ref [crate::BackReffable<'backrefs>]
    ) -> crate::Outcome<'text, 'result>
    where 'text: 'parser_ref, 'backrefs: 'result, 'parser_ref: 'result
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
    fn parse_some_of_<'text, 'parser_ref, 'backrefs, 'result>(
        self: &'parser_ref Self,
        text: &'text str,
        _cache: &mut [crate::Cache<'text, 'parser_ref>],
        _backrefs: &'parser_ref [crate::BackReffable<'backrefs>]
    ) -> crate::Outcome<'text, 'result>
    where 'text: 'parser_ref, 'backrefs: 'result, 'parser_ref: 'result
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
    fn parse_some_of_<'text, 'parser_ref, 'backrefs, 'result>(
        self: &'parser_ref Self,
        text: &'text str,
        _cache: &mut [crate::Cache<'text, 'parser_ref>],
        _backrefs: &'parser_ref [crate::BackReffable<'backrefs>]
    ) -> crate::Outcome<'text, 'result>
    where 'text: 'parser_ref, 'backrefs: 'result, 'parser_ref: 'result
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

pub struct AtLeastOne<'x>
{
    pub x: Arc<dyn crate::Parser+Send+Sync+'x>,
}

impl<'parser> crate::Parser for AtLeastOne<'parser>
{
    fn parse_some_of_<'text, 'parser_ref, 'backrefs, 'result>(
        self: &'parser_ref Self,
        text: &'text str,
        cache: &mut [crate::Cache<'text, 'parser_ref>],
        backrefs: &'parser_ref [crate::BackReffable<'backrefs>]
    ) -> crate::Outcome<'text, 'result>
    where 'text: 'parser_ref, 'backrefs: 'parser_ref, 'backrefs: 'result, 'parser_ref: 'result
    {
        let result = self.x.parse_some_of(text, cache, backrefs);
            match result {
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
                            (crate::Goal{parser: self.x.deref(), text: text}, result),)},

                crate::Outcome::Leaf{
                    matched,
                    then: None
                } |
                crate::Outcome::Composite{
                    matched: Some(matched),
                    components: _
                } => {
                    let mut components = vec!(
                        (crate::Goal{parser: self.x.deref(), text: text}, result) );
                    let mut rest: &str = all_of(text).after(matched);
                    let mut cache = &mut cache[matched.len()..];
                    loop {
                        let result = self.x.parse_some_of(rest, cache, backrefs);
                        match result {
                            crate::Outcome::Leaf{
                                matched: _,
                                then: Some(_)
                            } |
                            crate::Outcome::Composite{
                                matched: None,
                                components: _
                            } =>
                                return crate::Outcome::Composite{
                                    matched: Some(all_of(text).up_to(rest)),
                                    components: components
                                },
                            
                            crate::Outcome::Leaf{
                                matched,
                                then: None
                            } |
                            crate::Outcome::Composite{
                                matched: Some(matched),
                                components: _
                            } => {
                                components.push(
                                    (crate::Goal{parser: self.x.deref(), text: rest}, result) );
                                rest = all_of(rest).after(matched);
                                cache = &mut cache[matched.len()..];
                            }
                        }
                    }
                }
            }
    }
}

pub struct ZeroOrMore<'x>
{
    pub x: Arc<dyn crate::Parser+Send+Sync+'x>,
}

impl<'parser> crate::Parser for ZeroOrMore<'parser>
{
    fn parse_some_of_<'text, 'parser_ref, 'backrefs, 'result>(
        self: &'parser_ref Self,
        text: &'text str,
        cache: &mut [crate::Cache<'text, 'parser_ref>],
        backrefs: &'parser_ref [crate::BackReffable<'backrefs>]
    ) -> crate::Outcome<'text, 'result>
    where 'text: 'parser_ref, 'backrefs: 'parser_ref, 'backrefs: 'result, 'parser_ref: 'result
    {
        let mut components = vec!();
        let mut rest = text;
        let mut cache = cache;
        loop {
            let result = self.x.parse_some_of(rest, cache, backrefs);
            match result {
                crate::Outcome::Leaf{
                    matched: _,
                    then: Some(_)
                } |
                crate::Outcome::Composite{
                    matched: None,
                    components: _
                } =>
                    return crate::Outcome::Composite{
                        matched: Some(all_of(text).up_to(rest)),
                        components: components
                    },
                
                crate::Outcome::Leaf{
                    matched,
                    then: None
                } |
                crate::Outcome::Composite{
                    matched: Some(matched),
                    components: _
                } => {
                    components.push(
                        (crate::Goal{parser: self.x.deref(), text: rest}, result) );
                    rest = all_of(rest).after(matched);
                    cache = &mut cache[matched.len()..];
                }
            }
        }
    }
}


pub fn parse_charset(chars: &crate::CharSet) -> std::collections::HashSet<char>
{
    let c1 = "c1";
    let c2 = "c2";
    let c = "c";
    let range = "range";
    let parser = crate::zero_or_more(
        crate::tagged(range,
                      crate::tagged(c1, crate::any_char()) +
                      crate::char('-') +
                      crate::tagged(c2, crate::any_char()) ) |
        crate::tagged(c, crate::any_char())) + crate::end_of_input();
    let x = parser.parse(chars.value).get_ast("root");
    match x {
        Ok(ast) => {
            let mut result = std::collections::HashSet::new();
            ast.select_by_value(&|v| v.tag == range).iter().for_each(
                |range_ast| {
                    let c1 = range_ast.select_by_value(&|v| v.tag == c1).iter().next()
                        .unwrap().value.text.chars().next().unwrap();
                    let c2 = range_ast.select_by_value(&|v| v.tag == c2).iter().next()
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
            ast.select_by_value(&|v| v.tag == c).iter().for_each(
                |ast| { result.insert(ast.value.text.chars().next().unwrap()); });
            result
        },
        // cannot fail
        Err(x) => { ruf_assert::never_reached(x); }
    }
}

pub struct OneOfChars {
    pub chars: std::collections::HashSet<char>
}

impl crate::Parser for OneOfChars
{
    fn parse_some_of_<'text, 'parser_ref, 'backrefs, 'result>(
        self: &'parser_ref Self,
        text: &'text str,
        _cache: &mut [crate::Cache<'text, 'parser_ref>],
        _backrefs: &'parser_ref [crate::BackReffable<'backrefs>]
    ) -> crate::Outcome<'text, 'result>
    where 'text: 'parser_ref, 'backrefs: 'result, 'parser_ref: 'result
    {
        match text.chars().next() {
            Some(c) => {
                if self.chars.contains(&c) {
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

pub struct AnyCharExcept {
    pub chars: std::collections::HashSet<char>
}

impl crate::Parser for AnyCharExcept
{
    fn parse_some_of_<'text, 'parser_ref, 'backrefs, 'result>(
        self: &'parser_ref Self,
        text: &'text str,
        _cache: &mut [crate::Cache<'text, 'parser_ref>],
        _backrefs: &'parser_ref [crate::BackReffable<'backrefs>]
    ) -> crate::Outcome<'text, 'result>
    where 'text: 'parser_ref, 'backrefs: 'result, 'parser_ref: 'result
    {
        match text.chars().next() {
            Some(c) => {
                if self.chars.contains(&c) {
                    return crate::Outcome::Leaf{
                        matched: &text[0..0],
                        then: Some(crate::Unexpected::Char)
                    };
                }
                crate::Outcome::Leaf{
                    matched: &text[0..1],
                    then: None
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

pub struct ParseXUntilY<'x, 'y> {
    pub x: Arc<dyn crate::Parser+Send+Sync+'x>,
    pub y: Arc<dyn crate::Parser+Send+Sync+'y>,
}

impl<'x, 'y> crate::Parser for ParseXUntilY<'x, 'y>
{
    fn parse_some_of_<'text, 'parser_ref, 'backrefs, 'result>(
        self: &'parser_ref Self,
        text: &'text str,
        cache: &mut [crate::Cache<'text, 'parser_ref>],
        backrefs: &'parser_ref [crate::BackReffable<'backrefs>]
    ) -> crate::Outcome<'text, 'result>
    where 'text: 'parser_ref, 'x: 'parser_ref, 'y: 'parser_ref, 'backrefs: 'parser_ref, 'backrefs: 'result, 'parser_ref: 'result
    {
        let mut components = vec!();
        let mut rest = text;
        let mut cache = cache;
        loop {
            let close = self.y.parse_some_of(rest, cache, backrefs);
            match close {
                crate::Outcome::Leaf{ matched: _, then: None } |
                crate::Outcome::Composite{ matched: Some(_), components: _ } => {
                    return crate::Outcome::Composite{
                        matched: Some(all_of(text).up_to(rest)),
                        components: components
                    }
                },
                crate::Outcome::Leaf{ matched: _, then: Some(_) } |
                crate::Outcome::Composite{ matched: None, components: _ } => {
                
                    let item = self.x.parse_some_of(rest, cache, backrefs);
                    match item {
                        crate::Outcome::Leaf{ matched: _, then: Some(_) } |
                        crate::Outcome::Composite{ matched: None, components: _ } => {
                            components.push((crate::Goal{parser: self.y.deref(), text: rest}, close));
                            components.push((crate::Goal{parser: self.x.deref(), text: rest}, item));
                            return crate::Outcome::Composite{
                                matched: None,
                                components: components
                            };
                        },
                
                        crate::Outcome::Leaf{ matched, then: None } |
                        crate::Outcome::Composite{ matched: Some(matched), components: _ } => {
                            components.push((crate::Goal{parser: self.x.deref(), text: rest}, item) );
                            rest = all_of(rest).after(matched);
                            cache = &mut cache[matched.len()..];
                        }
                    }
                }
            }
        }
    }
}

pub struct ParseBalancedUntilY<'x, 'y, 'v> {
    pub balance_pairs: Vec<(crate::Ref<'v>, crate::Ref<'v>)>,
    pub content: crate::Ref<'x>,
    pub y: crate::Ref<'y>,
}

struct CompositeResult<'text, 'parser> {
    pub matched: Option<&'text str>,  // None if parse failed, otherwise the match
    pub components: Vec< (crate::Goal<'text, 'parser>, crate::Outcome<'text, 'parser>) >
}

impl<'x, 'y, 'v> crate::Parser for ParseBalancedUntilY<'x, 'y, 'v>
{
    // does not consume self.y
    fn parse_some_of_<'text, 'parser_ref, 'backrefs, 'result>(
        self: &'parser_ref Self,
        text: &'text str,
        cache: &mut [crate::Cache<'text, 'parser_ref>],
        backrefs: &'parser_ref [crate::BackReffable<'backrefs>]
    ) -> crate::Outcome<'text, 'result>
    where 'text: 'parser_ref, 'x: 'parser_ref, 'y: 'parser_ref, 'backrefs: 'parser_ref, 'backrefs: 'result, 'parser_ref: 'result
    {
        let result = self.parse_to_y(self.y.deref(), text, cache, backrefs);
        crate::Outcome::Composite{
            matched: result.matched,
            components: result.components
        }
    }
}

impl<'x, 'y, 'v> ParseBalancedUntilY<'x, 'y, 'v>
{
    // does not consume y
    fn parse_to_y<'text, 'parser_ref, 'backrefs, 'result>(
        self: &'parser_ref Self,
        y: &'y dyn crate::Parser,
        text: &'text str,
        cache: &mut [crate::Cache<'text, 'parser_ref>],
        backrefs: &'parser_ref [crate::BackReffable<'backrefs>]
    ) -> CompositeResult<'text, 'result>
    where 'text: 'parser_ref, 'x: 'parser_ref, 'y: 'parser_ref, 'backrefs: 'parser_ref, 'backrefs: 'result, 'parser_ref: 'result
    {
        let mut components = vec!();
        let mut rest = text;
        let mut cache = cache;
        loop {
            let end = y.parse_some_of(rest, cache, backrefs);
            match end {
                crate::Outcome::Leaf{ matched: _, then: None } |
                crate::Outcome::Composite{ matched: Some(_), components: _ } => {
                    // do not consume y
                    return CompositeResult{
                        matched: Some(all_of(text).up_to(rest)),
                        components
                    }
                },
                crate::Outcome::Leaf{ matched: _, then: Some(_) } |
                crate::Outcome::Composite{ matched: None, components: _ } => {
                    let mut nested = vec!();
                    for (ref p_open, ref p_close) in &self.balance_pairs {
                        let open = p_open.x.parse_some_of(rest, cache, backrefs);
                        match open {
                            crate::Outcome::Leaf{ matched: _, then: Some(_) } |
                            crate::Outcome::Composite{ matched: None, components: _ } =>
                                (),
                            
                            crate::Outcome::Leaf{ matched, then: None } |
                            crate::Outcome::Composite{ matched: Some(matched), components: _ } => {
                                nested.push((crate::Goal{parser: p_open.deref(), text: rest}, open));
                                rest = all_of(rest).after(matched);
                                cache = &mut cache[matched.len()..];
                                match self.parse_to_y(p_close.x.deref(), rest, cache, backrefs) {
                                    CompositeResult{ matched: None, components: nested_components } => {
                                        return CompositeResult{
                                            matched: None,
                                            components: [components, nested, nested_components].concat()
                                        };
                                    },
                                    
                                    CompositeResult{ matched: Some(matched), components: _ } => {
                                        rest = all_of(rest).after(matched);
                                        cache = &mut cache[matched.len()..];
                                        // parse_to does not consume close
                                        let close = p_close.parse_some_of(rest, cache, backrefs);
                                        match close {
                                            crate::Outcome::Leaf{ matched: _, then: Some(_) } |
                                            crate::Outcome::Composite{ matched: None, components: _ } =>
                                                ruf_assert::never_reached(&close),
                                            crate::Outcome::Leaf{ matched, then: None } |
                                            crate::Outcome::Composite{
                                                matched: Some(matched), components: _
                                            } => {
                                                rest = all_of(rest).after(matched);
                                                cache = &mut cache[matched.len()..];
                                                nested.push(
                                                    (crate::Goal{parser: p_close.deref(), text: rest},
                                                     close));
                                            }
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    if nested.len() == 0 {
                        let content = self.content.x.parse_some_of(rest, cache, backrefs);
                        match content {
                            crate::Outcome::Leaf{ matched: _, then: Some(_) } |
                            crate::Outcome::Composite{ matched: None, .. } => {
                                components.push(
                                    (crate::Goal{parser: self.content.deref(), text: rest}, content));
                                return CompositeResult{
                                    matched: None,
                                    components
                                };
                            },
                            crate::Outcome::Leaf{ matched, then: None } |
                            crate::Outcome::Composite{ matched: Some(matched), .. } => {
                                nested.push(
                                    (crate::Goal{parser: self.content.deref(), text: rest}, content));
                                rest = all_of(rest).after(matched);
                                cache = &mut cache[matched.len()..];
                            }
                        }
                    }
                    components = [components, nested].concat();
                }
            }
        }
    }
}

pub struct BackRefs<'parser, 'backrefs> {
    pub backrefs: Vec<crate::BackReffable<'backrefs>>,
    pub parser: crate::Ref<'parser>
}
    
impl<'parser, 'backrefs1> crate::Parser for BackRefs<'parser, 'backrefs1>
{
    fn parse_some_of_<'text, 'parser_ref, 'backrefs, 'result>(
        self: &'parser_ref Self,
        text: &'text str,
        cache: &mut [crate::Cache<'text, 'parser_ref>],
        _backrefs: &'parser_ref [crate::BackReffable<'backrefs>]
    ) -> crate::Outcome<'text, 'result>
    where 'text: 'parser_ref, 'parser: 'parser_ref, 'backrefs: 'parser_ref, 'backrefs: 'result, 'parser_ref: 'result
    {
        self.parser.parse_some_of(
            text,
            cache,
            self.backrefs.as_slice())
    }
}
