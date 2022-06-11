// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

pub struct SeparatedBy<'i, I, U>
    where I: Iterator<Item=U>, U: Copy
{
    sep: U,
    seen_first: bool,
    next: Option<U>,
    i: &'i mut I
}

impl<'i, I, U> std::iter::Iterator for SeparatedBy<'i, I, U>
    where I: std::iter::Iterator<Item=U>, U: Copy
{
    type Item = U;
    fn next(&mut self) -> Option<Self::Item>
    {
        match self.seen_first {
            false => {
                self.seen_first = true;
                self.i.next()
            },
            true => match self.next {
                None => match self.i.next() {
                    None => None,
                    Some(u) => {
                        self.next = Some(u);
                        Some(self.sep)
                    }
                },
                Some(u) => {
                        self.next = None;
                    Some(u)
                }
            }
        }
    }
}

pub trait IteratorSeparatedBy<I, U>
    where I: Iterator<Item=U>, U: Copy
{
    fn ruf_parser_separated_by(&mut self, sep: U) -> SeparatedBy<I, U>;
}

impl<I, U> IteratorSeparatedBy<I, U> for I
    where I: Iterator<Item=U>, U: Copy
{
    fn ruf_parser_separated_by(self: &mut Self, sep: U) -> SeparatedBy<Self, U>
    {
        SeparatedBy{ sep: sep, seen_first: false, next: None, i: self}
    }
}
