// crate::Parser implementations

extern crate ruf_assert;

use ruf_assert as assert;
use std::ops::Deref;

pub struct And<'parser>
{
    pub first_term: std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>,
    pub other_terms: Vec<std::sync::Arc<dyn crate::Parser+Send+Sync+'parser>>
}
impl<'and> crate::Parser for And<'and>
{
    fn parse_some_of<'text, 'parser>(self: &'parser Self, text: &'text str) ->
        crate::ParseResult<'text, 'parser>
    where 'and: 'parser, 'text: 'parser
    {
        let result_of_first_term = self.first_term.parse_some_of(text);
        match result_of_first_term {
            crate::ParseResult::Leaf(crate::LeafResult{
                matched: _,
                then: Some(_)}
            ) |
            crate::ParseResult::Composite(crate::CompositeResult{
                matched: None,
                components: _
            }) =>
                crate::ParseResult::Composite(
                    crate::CompositeResult{
                        matched: None,
                        components: vec!( (crate::Goal{parser: self.first_term.deref(), text: text},
                                           result_of_first_term) )}),
            crate::ParseResult::Leaf(crate::LeafResult{ matched: first_term_matched, then: None}) |
            crate::ParseResult::Composite(crate::CompositeResult{
                matched: Some(first_term_matched),
                components: _
            }) => {
                let mut components = vec!( (crate::Goal{parser: self.first_term.deref(), text: text},
                                            result_of_first_term) );
                let mut rest: &str = all_of(text).after(first_term_matched);
                for term in self.other_terms.iter() {
                    let result_of_term = term.parse_some_of(rest);
                    match result_of_term {
                        crate::ParseResult::Leaf(crate::LeafResult{
                            matched: _,
                            then: Some(_)}
                        ) |
                        crate::ParseResult::Composite(crate::CompositeResult{
                            matched: None,
                            components: _
                        }) =>
                        {
                            components.push( (crate::Goal{parser: term.deref(), text: rest},result_of_term) );
                            return crate::ParseResult::Composite(
                                crate::CompositeResult{
                                    matched: None,
                                    components: components});
                        },
                        crate::ParseResult::Leaf(crate::LeafResult{
                            matched,
                            then: None}
                        ) |
                        crate::ParseResult::Composite(crate::CompositeResult{
                            matched: Some(matched),
                            components: _
                        }) => {
                            rest = all_of(rest).after(matched);
                            components.push( (crate::Goal{parser: term.deref(), text: rest},result_of_term) );
                        }
                    }
                }
                crate::ParseResult::Composite(
                    crate::CompositeResult{
                        matched: Some(all_of(text).up_to(rest)),
                        components: components})
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
    fn parse_some_of<'text, 'parser>(self: &'parser Self, text: &'text str) ->
        crate::ParseResult<'text, 'parser>
    where 'text: 'parser, 'or: 'parser
    {
        let result_of_first_term = self.first_term.parse_some_of(text);
        match result_of_first_term {
            crate::ParseResult::Leaf(crate::LeafResult{
                matched,
                then: None}
            ) |
            crate::ParseResult::Composite(crate::CompositeResult{
                matched: Some(matched),
                components: _
            }) =>
                crate::ParseResult::Composite(
                    crate::CompositeResult{
                        matched: Some(matched),
                        components: vec!( (crate::Goal{parser: self.first_term.deref(), text: text},
                                           result_of_first_term) )}),

            crate::ParseResult::Leaf(crate::LeafResult{ matched: _, then: Some(_)}) |
            crate::ParseResult::Composite(crate::CompositeResult{
                matched: None,
                components: _
            }) => {
                let mut components = vec!( (crate::Goal{parser: self.first_term.deref(), text: text}, result_of_first_term) );
                for term in self.other_terms.iter() {
                    let result_of_term = term.parse_some_of(text);
                    match result_of_term {
                        crate::ParseResult::Leaf(crate::LeafResult{
                            matched,
                            then: None}
                        ) |
                        crate::ParseResult::Composite(crate::CompositeResult{
                            matched: Some(matched),
                            components: _
                        }) =>
                        {
                            components.push( (crate::Goal{parser: term.deref(), text: text}, result_of_term) );
                            return crate::ParseResult::Composite(
                                crate::CompositeResult{
                                    matched: Some(matched),
                                    components: components});
                        },
                        crate::ParseResult::Leaf(crate::LeafResult{
                            matched: _,
                            then: Some(_)}
                        ) |
                        crate::ParseResult::Composite(crate::CompositeResult{
                            matched: None,
                            components: _
                        }) =>
                            components.push( (crate::Goal{parser: term.deref(), text: text}, result_of_term) )
                    }
                }
                crate::ParseResult::Composite(
                    crate::CompositeResult{
                        matched: None,
                        components: components})
            }
        }
    }
}

pub struct Literal<'literal> {
    pub x: &'literal str
}

impl<'literal> crate::Parser for Literal<'literal>
{
    // REVISIT: we should not need separate 'literal and 'parser? should be just 'literal?
    fn parse_some_of<'text, 'parser>(self: &'parser Self, text: &'text str) ->
        crate::ParseResult<'text, 'parser>
    where 'literal: 'parser, 'text: 'parser
    {
        // we would use starts_with but when it doesn't start with we want the longest
        // having_parsed we can in error, so we use our own loop.
        // REVISIT: this also ignores unicode grapheme clusters
        let mut i = self.x.char_indices();
        let mut j = text.char_indices();
        loop {
            match (i.next(), j.next()) {
                (None, None) => {
                    return crate::ParseResult::<'text, 'parser>::Leaf(
                        crate::LeafResult{ matched: text, then: None });
                },
                (None, Some((n, _))) => {
                    return crate::ParseResult::<'text, 'parser>::Leaf(
                        crate::LeafResult{ matched: &text[0..n] , then: None });
                },
                (Some( (n, c1)), b) => {
                    match b {
                        Some( (_, c2) ) => {
                            if c1 == c2 { continue; }
                            return crate::ParseResult::<'text, 'parser>::Leaf(
                                crate::LeafResult{ matched: &text[0..n],
                                                   then: Some(crate::Unexpected::Char)});
                        },
                        None => return crate::ParseResult::<'text, 'parser>::Leaf(
                            crate::LeafResult{ matched: &text[0..n],
                                               then: Some(crate::Unexpected::EndOfInput)})
                    }
                }
            }
        }
    }
}

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
