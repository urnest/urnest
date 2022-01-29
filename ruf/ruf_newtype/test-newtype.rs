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

use std::hash::Hasher;
use std::hash::Hash;

fn main() {
    let a = I1 {value: 22};
    let b = I1 {value: -6};

    let c = a + b;
    assert::equal(&c, &I1{value:16});
    assert::less(&c, &I1{value:17});
    assert::less_equal(&c, &I1{value:17});
    assert::less_equal(&c, &I1{value:16});

    let mut d = I1 {value: 3};
    d += I1{value:4};
    assert::greater_equal(&d, &I1{value:7});
    assert::greater(&d, &I1{value:6});
    assert::equal(&d, &I1{value:7});
    
    assert::equal(&(I2{value:0.0} + I2::new(66.0)), &I2::new(66.0));

    assert::equal(&format!("{:b}", I1{value:8}).as_str(), &"1000"); // Binary
    assert::equal(&(I1{value:2} & I1::new(3)), &I1::new(2));
    let mut d = I1 {value: 3};
    d &= I1{value:2};
    assert::equal(&d, &I1::new(2));

    assert::equal(&(I1{value:6} | I1::new(3)), &I1::new(7));
    let mut d = I1 {value: 6};
    d |= I1{value:3};
    assert::equal(&d, &I1::new(7));

    assert::equal(&(I1{value:6} ^ I1::new(3)), &I1::new(5));
    let mut d = I1 {value: 6};
    d ^= I1{value:3};
    assert::equal(&d, &I1::new(5));

    let mut d = I1 {value: 6}.clone();
    assert::equal(&d, &I1::new(6));

    d.clone_from(&I1::new(7));
    assert::equal(&d, &I1::new(7));

    let c = I1 {value: 6};
    let d = c; // copy
    assert::equal(&c, &d);

    assert::equal(&format!("{:?}", I1{value:8}).as_str(), &"8"); // Debug

    let c = I1::default();
    assert::equal(&c, &I1{value: Default::default()});
    
    assert::equal(&format!("{}", I1{value:8}).as_str(), &"8"); // Display

    let a = I1 {value: 22};
    let b = I1 {value: 11};
    let c = a / b;
    assert::equal(&c, &I1{value:2});

    let mut d = I1 {value: 8};
    d /= I1{value:2};
    assert::equal(&d, &I1{value:4});

    //let d = NonZeroI32{ 6 };
    //REVISIT: awaits implementation assert::equal(&I1::from(d), &I1{value:6});

    let mut d = I1 {value: 8};
    let mut h1 = std::collections::hash_map::DefaultHasher::new();
    d.hash(&mut h1);
    
    let mut h2 = std::collections::hash_map::DefaultHasher::new();
    let f : i32 = 8;
    f.hash(&mut h2);
    assert::equal(&h1.finish(), &h2.finish());
}
