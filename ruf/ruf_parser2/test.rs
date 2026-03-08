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
}
