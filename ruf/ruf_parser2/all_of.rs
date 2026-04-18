use ruf_assert as assert;

// make some common slicing patterns a bit clearer, e.g.
//   all_of(x).after(y)  for x[y.len()..]
//   all_of(x).up_to(y)  for x[..y.len()]
pub fn all_of<'x>(x: &'x str) -> AllOf<'x>{ AllOf {x:x} }

pub struct AllOf<'x>
{
    x: &'x str
}

impl<'x> AllOf<'x>
{
    // return remainder of self after removing leading y
    pub fn after(&self, y: &str) -> &'x str
    {
        assert::equal(&self.x.as_ptr(), &y.as_ptr());
        assert::less_equal(&y.len(), &self.x.len());
        return &self.x[y.len()..];
    }
    // return self up to trailing y
    pub fn up_to(&self, y: &str) -> &'x str
    {
        assert::less_equal(&y.len(), &self.x.len());
        assert::less_equal(&self.x.as_ptr(), &y.as_ptr());
        return &self.x[0..self.x.len()-y.len()]
    }
    // return self through to the end of y
    pub fn through(&self, y: &str) -> &'x str
    {
        return &self.x[0..self.up_to(y).len()+y.len()];
    }
}
