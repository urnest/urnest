

struct X
{
    x: i32
}

fn xx(x : &X) -> i32
{
    x.x
}

fn a()
{
    let y = X{ x: 5 };   // variable x is immutable

    // cannot make y hold a new value
    // can't y = X{ x: 6 };
    
    // cannot change an attribute in the value held by y
    // can't y.x = 6;

    // cannot make variable yy a mutable reference to y as y is immutable
    // can't let yy : &mut X = &mut y;

    // can still take the value out of y
    let z = y;   // variable z is immutable

    // can no longer use y, it's value has moved into z
    // can't let q = y;

    // cannot move value back into y (can only put a value into y once)
    // can't y = z;

    // can make variable yy an immutable reference to z
    let yy : &X = &z;

    // can make multiple immutable references at the same time
    let yy2 : &X = &z;

    // we can try to move yy2's value (which is a reference) to yy3
    // but it actually derefereces yy2 on the sly, so we actually
    // end up with another reference to y ... you could consider it
    // an inconsistency I guess
    let yy3 : &X = yy2;

    // we can make a reference to yy3, which is itself a reference
    // not that it makes any sense
    let yy4 : &&X = &yy3;

    // we can use a yy4, which is a reference-to-a-reference-to
    // a variable holding an X where we only need a eference-to
    // a variable holding an X, rust automatically dereferences
    // for us (and will do as many dereferences as needs automatically).
    let yy5 = xx(yy4);

    // we can't call xxx with any of those references because that
    // would invalidate other refereces (regardless of whether
    // we subsequently use one of the other references)
    // xxx(**yy4);

    // we can't call xxx with the original either because it would
    // invalidate our references ... but note it is only a problem
    // if we later try to use one of those references... you could
    // consider this inconsistent with previous
    //xxx(z);

    // note each param here needs to be a reference-to-x, but
    // rust will automatically make one from variable holding x
    println!("{} {} {} {} {} {}", z.x, yy.x, yy2.x, yy3.x, yy4.x, yy5);
}

fn b()
{
    let mut y = X{ x: 5 };   // variable x is mutable

    // can change attribute of value held by y
    y.x = 6;

    // can make y hold a new value
    y = X{ x: 7 };

    // can make a mutable reference to y
    let yy : &mut X = &mut y;

    // we can't use both our reference and original as reference is mutable
    // can't println!("{} {}", y.x, yy.x);

    // but we can use one if we've stopped using the other
    println!("{}", y.x); // OR println!("{}", yy.x);

    // rust won't let us make an immutable ref to our mutable ref here
    // ... but only because we used y.x in println!() above. Really it
    // could let us, because the use of y in println is well and truly
    // gone by now ... and it's seemingly inconsistent with c() below
    // which does a similar thing but calling a function, not a macro :-(
    // maybe future version of rust will smarten up
    // can't let yyy : &&mut X = &yy;
    // can't xx(&yy);
}

fn c()
{
    let mut y = X{ x: 5 };   // variable x is mutable

    let yy : &mut X = &mut y;

    // even though we have a mutable ref, we can turn that
    // into an immutable ref to call xx
    xx(&yy);
    // can let rust turn it into an immutable ref too
    xx(yy);

    let yy2 : &&mut X = &yy;
    xx(yy2);
    println!("{}", yy2.x);
    
    // and we can again make use of mutable ref after we're done
    yy.x = 10;

    // but not directory on y  if we later go via our ref
    // can't y.x = 7; yy.x = 12;
}

// so above means we should be able to return
// an immutable ref to an object attribute that is
// a mutable ref, use it for a little while
// then go back to using the object mutable ref

struct D<'a>
{
    d : &'a mut i32
}

impl<'a> D<'a>
{
    fn dref(self: &'a D<'a>) -> &'a i32     
    {   //                       |
        // there is some automatic coersion here I think?
	//                       |
	let result :            &&'a mut i32 = &self.d;
	return result;

	// and in fact rust will do it all for us like:
	// return self.d;
    }
}

// and this is how to return mut reference to self
impl<'a, 'b> D<'a>
    where 'a: 'b
{
    fn adjust_self(self: &'b mut D<'a>) -> &'b mut D<'a>
    {
	*self.d += 1;
	self
    }
}

fn d()
{
    let mut x : i32 = 3;

    let mut d = D{ d: &mut x };

    let y: &i32 = d.dref();
    println!("{}", y);

    // even though the result of d.dref() remains valid while d is valid,
    // rust "drops" it for us when it sees the next line
    *d.d = 8;

    d.adjust_self().adjust_self();
    //d.adjust_self();

    println!("{}",d.d);
}

fn main()
{
    a();
    b();
    c();
    d();
}
