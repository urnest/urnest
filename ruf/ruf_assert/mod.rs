// Copyright (c) 2021 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//    Assertion functions.
//
//    Notes:
//
//    [1] These are preferrable over a simple logical
//        assertion (e.g. assert_(a != b)), since on failure
//        they produce a core file containing the evaluated
//        values of a and b, which can otherwise be difficult
//        to deduce.
//
pub fn equal<T>(a: &T, b: &T)
    where T: std::cmp::PartialEq
{
    if !(*a == *b){
	::std::process::abort();
    }
}

pub fn not_equal<T>(a: &T, b: &T)
    where T: std::cmp::PartialEq
{
    if *a == *b {
	::std::process::abort();
    }
}

pub fn less<T>(a: &T, b: &T)
    where T: std::cmp::PartialOrd
{
    if !(*a < *b) {
	::std::process::abort();
    }
}

pub fn less_equal<T>(a: &T, b: &T)
    where T: std::cmp::PartialOrd
{
    if !(*a <= *b) {
	::std::process::abort();
    }
}

pub fn greater<T>(a: &T, b: &T)
    where T: std::cmp::PartialOrd
{
    if !(*a > *b) {
	::std::process::abort();
    }
}

pub fn greater_equal<T>(a: &T, b: &T)
    where T: std::cmp::PartialOrd
{
    if !(*a >= *b) {
	::std::process::abort();
    }
}

pub fn slice_equal<T>(a: &[T], b: &[T])
    where T: std::cmp::PartialEq
{
    equal(&a.len(), &b.len());
    for (x, y) in a.iter().zip(b.iter()) {
	equal(&x, &y);
    }
}


/// a starts with prefix
pub fn slice_starts_with<T>(a: &[T], prefix: &[T])
    where T: std::cmp::PartialEq
{
    greater_equal(&a.len(), &prefix.len());
    if a[0..prefix.len()] != *prefix
    {
	::std::process::abort();
    }
}

/// a starts with prefix
pub fn str_starts_with(a: &str, prefix: &str)
{
    equal(&a.starts_with(prefix), &true);
}
