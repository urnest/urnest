// Copyright (c) 2021 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

extern crate ruf_parser;
extern crate ruf_assert;

use ruf_assert as assert;
use ruf_parser::separated_by::IteratorSeparatedBy;

fn main() {
    let x : Vec<i32> = vec!(1);
    let y: Vec<i32> = x.iter().ruf_parser_separated_by(&0).map(|x: &i32| *x).collect();
    assert::equal(&vec!(1), &y);

    let x : Vec<i32> = vec!(1, 2);
    let y: Vec<i32> = x.iter().ruf_parser_separated_by(&0).map(|x| *x).collect();
    assert::equal(&vec!(1, 0, 2), &y);

    let x : Vec<i32> = vec!();
    let y: Vec<i32> = x.iter().ruf_parser_separated_by(&0).map(|&x| x).collect();
    assert::equal(&vec!(), &y);

    let x : Vec<i32> = vec!(1, 2, 3);
    let y: Vec<i32> = x.iter().ruf_parser_separated_by(&0).map(|&x| x).collect();
    assert::equal(&vec!(1, 0, 2, 0, 3), &y);
}
