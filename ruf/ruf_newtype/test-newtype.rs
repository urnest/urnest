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

struct I1_; impl ruf_newtype::Tag for I1_ { type BaseType = i32;}
struct I2_; impl ruf_newtype::Tag for I2_ { type BaseType = f64;}
struct B1_; impl ruf_newtype::Tag for B1_ { type BaseType = bool;}
struct S1_; impl ruf_newtype::Tag for S1_ { type BaseType = String;}

type I1 = ruf_newtype::T<I1_>;
type I2 = ruf_newtype::T<I2_>;
type B1 = ruf_newtype::T<B1_>;
type S1 = ruf_newtype::T<S1_>;

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

    /* 
    ** does not work see "two strings added together" comment in mod.rs

    assert::equal(& (S1{value: String::from("fred")}+S1{value: String::from(" jones")}),
		  & S1{value: String::from("fred jones")});
     */
    
    let mut d = I1 {value: 3};
    d += I1{value:4};
    assert::greater_equal(&d, &I1{value:7});
    assert::greater(&d, &I1{value:6});
    assert::equal(&d, &I1{value:7});

    /* REVISIT: wtf?
    let mut d = S1{value: String::from("fred")};
    d += S1{value: String::from(" jones")};
    assert::equal(&d, & S1{value: String::from("fred jones")});
     */
    
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

    let d = I1 {value: 8};
    let mut h1 = std::collections::hash_map::DefaultHasher::new();
    d.hash(&mut h1);
    
    let mut h2 = std::collections::hash_map::DefaultHasher::new();
    let f : i32 = 8;
    f.hash(&mut h2);
    assert::equal(&h1.finish(), &h2.finish());

    assert::equal(&format!("{:e}", I1{value:42}).as_str(), &"4.2e1"); // LowerExp
    assert::equal(&format!("{:x}", I1{value:42}).as_str(), &"2a"); // LowerHex

    assert::equal(&(I2{value:2.0} * I2::new(66.0)), &I2::new(132.0));

    let mut d = I1 {value: 3};
    d *= I1{value:4};
    assert::equal(&d, &I1{value:12});
    
    assert::equal(& -I2{value:2.0}, &I2::new(-2.0));

    assert::equal(& !B1{value:true}, &B1::new(false));

    assert::equal(&format!("{:o}", I1{value:42}).as_str(), &"52"); // Octal

    let d = [ I1{value:2}, I1{value:3} ];
    let e = d.iter().product();
    assert::equal(&e, &I1{value:6});

    assert::equal(& (I1{value: 10} % I1{value: 3}), &I1{value: 1});

    let mut d = I1{value: 10};
    d %= I1{value: 3};
    assert::equal(&d, &I1{value: 1});

    assert::equal(& (I1{value: 1} << 2), &I1{value: 4});

    let mut d = I1{value: 1};
    d <<= 2;
    assert::equal(&d, &I1{value: 4});

    assert::equal(& (I1{value: 4} >> 2), &I1{value: 1});

    let mut d = I1{value: 4};
    d >>= 2;
    assert::equal(&d, &I1{value: 1});

    assert::equal(& (I1{value: 4} - I1{value: 2}), &I1{value: 2});

    let mut d = I1{value: 4};
    d -= I1{value:2};
    assert::equal(&d, &I1{value: 2});

    let d = [ I1{value:2}, I1{value:3} ];
    let e = d.iter().sum();
    assert::equal(&e, &I1{value:5});

    assert::equal(&format!("{:E}", I1{value:42}).as_str(), &"4.2E1"); // LowerExp
    assert::equal(&format!("{:X}", I1{value:42}).as_str(), &"2A"); // LowerHex

    let d = S1{value: String::from("fred")};
    let mut e = d.clone();

    assert::equal(&I1 {value: -6}.abs(), &I1{value:6});
}
