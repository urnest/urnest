// Copyright (c) 2021 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

extern crate ruf_newtype;
extern crate ruf_assert;

use ruf_assert as assert;

struct I1Tag{}
struct I2Tag{}

type I1 = ruf_newtype::T<i32, I1Tag>;
type I2 = ruf_newtype::T<f64, I2Tag>;

fn main() {
    let a = I1::new(22);
    let b = I1::new(-6);

    let c = a + b;
    assert::equal(&c, &I1::new(16));

    assert::equal(&(I2::new(0.0) + I2::new(66.0)), &I2::new(66.0));
}
