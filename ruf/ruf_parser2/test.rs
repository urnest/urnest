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

use ruf_parser2::{
    AST,
    ParseResult,
    LeafResult,
    CompositeResult,
    Unexpected,
    Goal,
    literal,
    tagged,
    end_of_input,
    get_ast,
    ParseFailed,
};
use ruf_parser2::ast::Item;
use ruf_parser2::all_of::all_of;

use ruf_assert as assert;

fn main() {
    let root="root";

    // end_of_input
    let x = "";
    let p = end_of_input();
    let r = p.parse(&x);
    assert::equal(&r,
                  &ParseResult::Leaf(LeafResult{
                      matched: &x,
                      then: None,  // success (end of input)
                  }));
    assert::equal(&get_ast(&x, &r, root),
                  &Ok(AST{ value:Item{ tag: root, text: &x }, children: vec!()}));
    
    let x = "left over";
    let r = p.parse(&x);
    assert::equal(&r,
                  &ParseResult::Leaf(LeafResult{
                      matched: &x[0..0],
                      then: Some(Unexpected::Char),  // not at end of input!
                  }));
    assert::equal(&get_ast(&x, &r, root),
                  &Err(ParseFailed{
                      at: all_of(x).after(&x[..0]),
                      why: Unexpected::Char,
                      context: vec!()
                  }));

    // literal
    let x = "freddy was very good";
    let p = literal("freddy");
    let y = p.parse(x);
    assert::equal(&y, &ParseResult::Leaf(LeafResult{
        matched: &x[0..6],
        then: None
    }));

    let x = "fredy was very good";
    let y = p.parse(x);
    assert::equal(&y, &ParseResult::Leaf(LeafResult{
        matched: &x[0..4],
        then: Some(Unexpected::Char)
    }));

    let x = "fredd";
    let y = p.parse(x);
    assert::equal(&y, &ParseResult::Leaf(LeafResult{
        matched: &x[0..5],
        then: Some(Unexpected::EndOfInput)
    }));

    // p1 + p2 i.e. parse p1 then p2
    let x = "freddy was very good";
    let p1 = literal("freddy");
    let p2 = literal(" was");
    let p = p1.clone() + p2.clone();
    let y = p.parse(x);
    assert::equal(&y, &ParseResult::Composite(
        CompositeResult{
            matched: Some(&x[0..10]),  // success, matched "freddy was"
            components: vec!(
                (Goal{parser: &*p1, text: x},
                 ParseResult::Leaf(
                     LeafResult{
                         matched: &x[0..6],
                         then: None})),
                (Goal{ parser: &*p2, text: &x[6..]},
                 ParseResult::Leaf(
                     LeafResult{
                         matched: &x[6..10],
                         then: None})),
            )}));

    let x = "fred was very good";
    let y = p.parse(x);
    assert::equal(&y, &ParseResult::Composite(
        CompositeResult{
            matched: None,  // failed, reason is captured in components 
            components: vec!(                         //              |
                (Goal{parser: &*p1, text: x},         //              |
                 ParseResult::Leaf(                   //              |
                     LeafResult{                      //              |
                         matched: &x[0..4],           // "fred" looked good...
                         then: Some(Unexpected::Char) // ... but no "dy" etc
                     })),
            )}));

    let x = "freddy wis very good";
    let y = p.parse(x);
    assert::equal(&y, &ParseResult::Composite(
        CompositeResult{
            matched: None,
            components: vec!(
                (Goal{parser: &*p1, text: x},
                 ParseResult::Leaf(
                     LeafResult{
                         matched: &x[0..6],
                         then: None})),
                (Goal{ parser: &*p2, text: &x[6..]},
                 ParseResult::Leaf(
                     LeafResult{
                         matched: &x[6..8],
                         then: Some(Unexpected::Char)})),
            )}));

    let x = "freddy wa";
    let y = p.parse(x);
    assert::equal(&y, &ParseResult::Composite(
        CompositeResult{
            matched: None,
            components: vec!(
                (Goal{parser: &*p1, text: x},
                 ParseResult::Leaf(
                     LeafResult{
                         matched: &x[0..6],
                         then: None})),
                (Goal{ parser: &*p2, text: &x[6..]},
                 ParseResult::Leaf(
                     LeafResult{
                         matched: &x[6..9],
                         then: Some(Unexpected::EndOfInput)})),
            )}));

    // p3 | p4 i.e. parse either p3 or p4 (trying p3 first)
    let x = "freddy was very good";
    let p1 = literal("freddy");
    let p2 = literal(" was");
    let p3 = literal(" is");
    let p4 = p2.clone() | p3.clone();
    let p = p1.clone() + p4.clone();
    let y = p.parse(x);
    assert::equal(&y, &ParseResult::Composite(
        CompositeResult{
            matched: Some(&x[0..10]),
            components: vec!(
                (Goal{parser: &*p1, text: x},
                 ParseResult::Leaf(
                     LeafResult{
                         matched: &x[0..6],
                         then: None})),
                (Goal{parser: &*p4, text: &x[6..]},
                 ParseResult::Composite(
                     CompositeResult{
                         matched: Some(&x[6..10]),
                         components: vec!( (Goal{ parser: &*p2, text: &x[6..]},
                                            ParseResult::Leaf(
                                                LeafResult{
                                                    matched: &x[6..10],
                                                    then: None})) )})),
            )}));

    let x = "freddy is very good";
    let p1 = literal("freddy");
    let p2 = literal(" was");
    let p3 = literal(" is");
    let p4 = p2.clone() | p3.clone();
    let p = p1.clone() + p4.clone();
    let y = p.parse(x);
    assert::equal(&y, &ParseResult::Composite(
        CompositeResult{
            matched: Some(&x[0..9]),
            components: vec!(
                (Goal{parser: &*p1, text: x},
                 ParseResult::Leaf(
                     LeafResult{
                         matched: &x[0..6],
                         then: None})),
                (Goal{parser: &*p4, text: &x[6..]},
                 ParseResult::Composite(
                     CompositeResult{
                         matched: Some(&x[6..9]),
                         components: vec!(
                             (Goal{ parser: &*p2, text: &x[6..]},
                              ParseResult::Leaf(
                                  LeafResult{
                                      matched: &x[6..7],
                                      then: Some(Unexpected::Char)})),
                             (Goal{ parser: &*p3, text: &x[6..]},
                              ParseResult::Leaf(
                                  LeafResult{
                                      matched: &x[6..9],
                                      then: None})) )})),
            )}));

    let x = "freddy not very good";
    let p1 = literal("freddy");
    let p2 = literal(" was");
    let p3 = literal(" is");
    let p4 = p2.clone() | p3.clone();
    let p = p1.clone() + p4.clone();
    let y = p.parse(x);
    assert::equal(&y, &ParseResult::Composite(
        CompositeResult{
            matched: None,
            components: vec!(
                (Goal{parser: &*p1, text: x},
                 ParseResult::Leaf(
                     LeafResult{
                         matched: &x[0..6],
                         then: None})),
                (Goal{parser: &*p4, text: &x[6..]},
                 ParseResult::Composite(
                     CompositeResult{
                         matched: None,
                         components: vec!(
                             (Goal{ parser: &*p2, text: &x[6..]},
                              ParseResult::Leaf(
                                  LeafResult{
                                      matched: &x[6..7],
                                      then: Some(Unexpected::Char)})),
                             (Goal{ parser: &*p3, text: &x[6..]},
                              ParseResult::Leaf(
                                  LeafResult{
                                      matched: &x[6..7],
                                      then: Some(Unexpected::Char)})) )})),
            )}));

    // tagged
    let x = "freddy was very good";
    let p1 = literal("freddy");
    let p = tagged(&"tag1", p1.clone());
    let y = p.parse(x);
    assert::equal(&y, &ParseResult::Composite(
        CompositeResult{
            matched: Some(&x[0..6]),
            components: vec!(
                (Goal{ parser: &*p1, text: &x},
                 ParseResult::Leaf(LeafResult{
                     matched: &x[0..6],
                     then: None
                 })),)}));

    let x = "freddy was very good";
    let p1 = literal("freddy");
    let p2 = literal(" was");
    let p3 = p1.clone() + p2.clone();
    let p = tagged(&"tag1", p3.clone());
    let y = p.parse(x);
    assert::equal(&y, &ParseResult::Composite(
        CompositeResult{
            matched: Some(&x[0..10]),
            components: vec!(
                (Goal{ parser: &*p3, text: &x},
                 ParseResult::Composite(
                     CompositeResult{
                         matched: Some(&x[0..10]),
                         components: vec!(
                             (Goal{parser: &*p1, text: x},
                              ParseResult::Leaf(
                                  LeafResult{
                                      matched: &x[0..6],
                                      then: None})),
                             (Goal{ parser: &*p2, text: &x[6..]},
                              ParseResult::Leaf(
                                  LeafResult{
                                      matched: &x[6..10],
                                      then: None})),
                         )})),)}));

    let x = "freddy was very good";
    let p1 = literal("jenny");
    let p = tagged(&"tag1", p1.clone());
    let y = p.parse(x);
    assert::equal(&y, &ParseResult::Composite(
        CompositeResult{
            matched: None,
            components: vec!(
                (Goal{ parser: &*p1, text: &x},
                 ParseResult::Leaf(LeafResult{
                     matched: &x[0..0],
                     then: Some(Unexpected::Char)
                 })),)}));

    let x = "freddy was very good";
    let p1 = literal("freddy");
    let p2 = literal(" wasn't");
    let p3 = p1.clone() + p2.clone();
    let p = tagged(&"tag1", p3.clone());
    let y = p.parse(x);
    assert::equal(&y, &ParseResult::Composite(
        CompositeResult{
            matched: None,
            components: vec!(
                (Goal{ parser: &*p3, text: &x},
                 ParseResult::Composite(
                     CompositeResult{
                         matched: None,
                         components: vec!(
                             (Goal{parser: &*p1, text: x},
                              ParseResult::Leaf(
                                  LeafResult{
                                      matched: &x[0..6],
                                      then: None})),
                             (Goal{ parser: &*p2, text: &x[6..]},
                              ParseResult::Leaf(
                                  LeafResult{
                                      matched: &x[6..10],
                                      then: Some(Unexpected::Char)})),
                         )})),)}));
}
