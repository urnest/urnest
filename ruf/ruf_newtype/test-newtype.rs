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

struct I1_; impl ruf_newtype::Tag for I1_ { type BaseType = i32; }
struct I2_; impl ruf_newtype::Tag for I2_ { type BaseType = f64; }

type I1 = ruf_newtype::T<I1_>;
type I2 = ruf_newtype::T<I2_>;

// REVISIT: macro so that can write ruf_newtype::NewType!(I1,i32);

fn main() {
    let a = I1 {value: 22};
    let b = I1 {value: -6};

    let c = a + b;
    assert::equal(&c, &I1{value:16});

    assert::equal(&(I2{value:0.0} + I2::new(66.0)), &I2::new(66.0));
}
