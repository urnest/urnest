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
    // end_of_input
    let x = "";
    assert::equal(&parser::parse_some_of(&*parser::end_of_input(), &x),
                  &parser::ParseResult::Leaf(parser::LeafResult{
                      matched: &x,
                      then: None,  // success (end of input)
                  }));

    let x = "left over";
    assert::equal(&parser::parse_some_of(&*parser::end_of_input(), &x),
                  &parser::ParseResult::Leaf(parser::LeafResult{
                      matched: &x[0..0],
                      then: Some(parser::Unexpected::Char),  // not at end of input!
                  }));

    // literal
    let x = "freddy was very good";
    let p = parser::literal("freddy");
    let y = parser::parse_some_of(&*p, x);
    assert::equal(&y, &parser::ParseResult::Leaf(parser::LeafResult{
        matched: &x[0..6],
        then: None
    }));

    let x = "fredy was very good";
    let y = parser::parse_some_of(&*p, x);
    assert::equal(&y, &parser::ParseResult::Leaf(parser::LeafResult{
        matched: &x[0..4],
        then: Some(parser::Unexpected::Char)
    }));

    let x = "fredd";
    let y = parser::parse_some_of(&*p, x);
    assert::equal(&y, &parser::ParseResult::Leaf(parser::LeafResult{
        matched: &x[0..5],
        then: Some(parser::Unexpected::EndOfInput)
    }));

    // p1 + p2 i.e. parse p1 then p2
    let x = "freddy was very good";
    let p1 = parser::literal("freddy");
    let p2 = parser::literal(" was");
    let p = p1.clone() + p2.clone();
    let y = parser::parse_some_of(&*p, x);
    assert::equal(&y, &parser::ParseResult::Composite(
        parser::CompositeResult{
            matched: Some(&x[0..10]),  // success, matched "freddy was"
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
    let y = parser::parse_some_of(&*p, x);
    assert::equal(&y, &parser::ParseResult::Composite(
        parser::CompositeResult{
            matched: None,  // failed, reason is captured in components 
            components: vec!(                                 //      |
                (parser::Goal{parser: &*p1, text: x},         //      |
                 parser::ParseResult::Leaf(                   //      |
                     parser::LeafResult{                      //      |
                         matched: &x[0..4],                   // "fred" looked good...
                         then: Some(parser::Unexpected::Char) // ... but no "dy" etc
                     })),
            )}));

    let x = "freddy wis very good";
    let y = parser::parse_some_of(&*p, x);
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
    let y = parser::parse_some_of(&*p, x);
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

    // p3 | p4 i.e. parse either p3 or p4 (trying p3 first)
    let x = "freddy was very good";
    let p1 = parser::literal("freddy");
    let p2 = parser::literal(" was");
    let p3 = parser::literal(" is");
    let p4 = p2.clone() | p3.clone();
    let p = p1.clone() + p4.clone();
    let y = parser::parse_some_of(&*p, x);
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
    let y = parser::parse_some_of(&*p, x);
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
    let y = parser::parse_some_of(&*p, x);
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
