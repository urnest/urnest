// Copyright (c) 2021 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

extern crate ruf_parser2;
extern crate ruf_assert;

use ruf_parser2 as parser;
use ruf_assert as assert;

fn main() {
    let x = "freddy was very good";
    let p = parser::literal("freddy");
    let y = p.parse_some_of(x);
    assert::equal(&y, &parser::ParseResult::Leaf(parser::LeafResult{
        matched: &x[0..6],
        then: None
    }));

    let x = "fredy was very good";
    let y = p.parse_some_of(x);
    assert::equal(&y, &parser::ParseResult::Leaf(parser::LeafResult{
        matched: &x[0..4],
        then: Some(parser::Unexpected::Char)
    }));

    let x = "fredd";
    let y = p.parse_some_of(x);
    assert::equal(&y, &parser::ParseResult::Leaf(parser::LeafResult{
        matched: &x[0..5],
        then: Some(parser::Unexpected::EndOfInput)
    }));

    let x = "freddy was very good";
    let p1 = parser::literal("freddy");
    let p2 = parser::literal(" was");
    let p = p1.clone() + p2.clone();
    let y = p.parse_some_of(x);
    assert::equal(&y, &parser::ParseResult::Composite(
        parser::CompositeResult{
            matched: Some(&x[0..10]),
            components: vec!(
                (parser::Goal{parser: &*p1, text: x},
                 parser::ParseResult::Leaf(
                     parser::LeafResult{
                         matched: &x[0..6],
                         then: None})),
                (parser::Goal{ parser: &*p2, text: &x[6..]},
                 parser::ParseResult::Leaf(
                     parser::LeafResult{
                         matched: &x[6..10],
                         then: None})),
            )}));

    let x = "fred was very good";
    let y = p.parse_some_of(x);
    assert::equal(&y, &parser::ParseResult::Composite(
        parser::CompositeResult{
            matched: None,
            components: vec!(
                (parser::Goal{parser: &*p1, text: x},
                 parser::ParseResult::Leaf(
                     parser::LeafResult{
                         matched: &x[0..4],
                         then: Some(parser::Unexpected::Char)})),
            )}));

    let x = "freddy wis very good";
    let y = p.parse_some_of(x);
    assert::equal(&y, &parser::ParseResult::Composite(
        parser::CompositeResult{
            matched: None,
            components: vec!(
                (parser::Goal{parser: &*p1, text: x},
                 parser::ParseResult::Leaf(
                     parser::LeafResult{
                         matched: &x[0..6],
                         then: None})),
                (parser::Goal{ parser: &*p2, text: &x[6..]},
                 parser::ParseResult::Leaf(
                     parser::LeafResult{
                         matched: &x[6..8],
                         then: Some(parser::Unexpected::Char)})),
            )}));

    let x = "freddy wa";
    let y = p.parse_some_of(x);
    assert::equal(&y, &parser::ParseResult::Composite(
        parser::CompositeResult{
            matched: None,
            components: vec!(
                (parser::Goal{parser: &*p1, text: x},
                 parser::ParseResult::Leaf(
                     parser::LeafResult{
                         matched: &x[0..6],
                         then: None})),
                (parser::Goal{ parser: &*p2, text: &x[6..]},
                 parser::ParseResult::Leaf(
                     parser::LeafResult{
                         matched: &x[6..9],
                         then: Some(parser::Unexpected::EndOfInput)})),
            )}));

    let x = "freddy was very good";
    let p1 = parser::literal("freddy");
    let p2 = parser::literal(" was");
    let p3 = parser::literal(" is");
    let p4 = p2.clone() | p3.clone();
    let p = p1.clone() + p4.clone();
    let y = p.parse_some_of(x);
    assert::equal(&y, &parser::ParseResult::Composite(
        parser::CompositeResult{
            matched: Some(&x[0..10]),
            components: vec!(
                (parser::Goal{parser: &*p1, text: x},
                 parser::ParseResult::Leaf(
                     parser::LeafResult{
                         matched: &x[0..6],
                         then: None})),
                (parser::Goal{parser: &*p4, text: &x[6..]},
                 parser::ParseResult::Composite(
                     parser::CompositeResult{
                         matched: Some(&x[6..10]),
                         components: vec!( (parser::Goal{ parser: &*p2, text: &x[6..]},
                                            parser::ParseResult::Leaf(
                                                parser::LeafResult{
                                                    matched: &x[6..10],
                                                    then: None})) )})),
            )}));

    let x = "freddy is very good";
    let p1 = parser::literal("freddy");
    let p2 = parser::literal(" was");
    let p3 = parser::literal(" is");
    let p4 = p2.clone() | p3.clone();
    let p = p1.clone() + p4.clone();
    let y = p.parse_some_of(x);
    assert::equal(&y, &parser::ParseResult::Composite(
        parser::CompositeResult{
            matched: Some(&x[0..9]),
            components: vec!(
                (parser::Goal{parser: &*p1, text: x},
                 parser::ParseResult::Leaf(
                     parser::LeafResult{
                         matched: &x[0..6],
                         then: None})),
                (parser::Goal{parser: &*p4, text: &x[6..]},
                 parser::ParseResult::Composite(
                     parser::CompositeResult{
                         matched: Some(&x[6..9]),
                         components: vec!(
                             (parser::Goal{ parser: &*p2, text: &x[6..]},
                              parser::ParseResult::Leaf(
                                  parser::LeafResult{
                                      matched: &x[6..7],
                                      then: Some(parser::Unexpected::Char)})),
                             (parser::Goal{ parser: &*p3, text: &x[6..]},
                              parser::ParseResult::Leaf(
                                  parser::LeafResult{
                                      matched: &x[6..9],
                                      then: None})) )})),
            )}));

    let x = "freddy not very good";
    let p1 = parser::literal("freddy");
    let p2 = parser::literal(" was");
    let p3 = parser::literal(" is");
    let p4 = p2.clone() | p3.clone();
    let p = p1.clone() + p4.clone();
    let y = p.parse_some_of(x);
    assert::equal(&y, &parser::ParseResult::Composite(
        parser::CompositeResult{
            matched: None,
            components: vec!(
                (parser::Goal{parser: &*p1, text: x},
                 parser::ParseResult::Leaf(
                     parser::LeafResult{
                         matched: &x[0..6],
                         then: None})),
                (parser::Goal{parser: &*p4, text: &x[6..]},
                 parser::ParseResult::Composite(
                     parser::CompositeResult{
                         matched: None,
                         components: vec!(
                             (parser::Goal{ parser: &*p2, text: &x[6..]},
                              parser::ParseResult::Leaf(
                                  parser::LeafResult{
                                      matched: &x[6..7],
                                      then: Some(parser::Unexpected::Char)})),
                             (parser::Goal{ parser: &*p3, text: &x[6..]},
                              parser::ParseResult::Leaf(
                                  parser::LeafResult{
                                      matched: &x[6..7],
                                      then: Some(parser::Unexpected::Char)})) )})),
            )}));
}
