// new-type trait class

pub trait Tag
{
    type BaseType;
}

pub struct T<U: Tag>
{
    pub value : U::BaseType,
}

impl<U: Tag> T<U>
{
    pub fn new(value : U::BaseType) -> T<U> {
	T::<U>{ value: value }
    }
}

pub trait Value
{
    type ValueType;
}
impl<U: Tag> Value for T<U>
{
    type ValueType = U::BaseType;
}


impl<U: Tag> std::cmp::PartialEq for T<U>
where U::BaseType : std::cmp::PartialEq
{
    fn eq(&self, other: &Self) -> bool { self.value == other.value }
}

impl<U: Tag> std::cmp::Eq for T<U>
where U::BaseType : std::cmp::Eq
{
}

impl<U: Tag> std::cmp::PartialOrd for T<U>
where U::BaseType : std::cmp::PartialOrd
{
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
	self.value.partial_cmp(&other.value)
    }
}

impl<U: Tag> std::cmp::Ord for T<U>
where U::BaseType : std::cmp::Ord
{
    fn cmp(&self, other: &Self) -> std::cmp::Ordering { self.value.cmp(&other.value) }
}

impl<U: Tag> std::ops::Add for T<U>
where U::BaseType : std::ops::Add<Output = U::BaseType>
{
    type Output = Self;
    
    fn add(self, other: Self) -> Self { Self::new(self.value + other.value) }
}

impl<U: Tag> std::ops::AddAssign for T<U>
where U::BaseType : std::ops::AddAssign
{
    fn add_assign(&mut self, other: Self) { self.value += other.value }
}

impl<U: Tag> std::fmt::Binary for T<U>
where U::BaseType : std::fmt::Binary
{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> Result<(), std::fmt::Error> { self.value.fmt(f) }
}

impl<U: Tag> std::ops::BitAnd for T<U>
where U::BaseType : std::ops::BitAnd<Output = U::BaseType>
{
    type Output = Self;
    
    fn bitand(self, other: Self) -> Self { Self::new(self.value & other.value) }
}

impl<U: Tag> std::ops::BitAndAssign for T<U>
where U::BaseType : std::ops::BitAndAssign
{
    fn bitand_assign(&mut self, other: Self) { self.value &= other.value }
}

impl<U: Tag> std::ops::BitOr for T<U>
where U::BaseType : std::ops::BitOr<Output = U::BaseType>
{
    type Output = Self;
    
    fn bitor(self, other: Self) -> Self { Self::new(self.value | other.value) }
}

impl<U: Tag> std::ops::BitOrAssign for T<U>
where U::BaseType : std::ops::BitOrAssign
{
    fn bitor_assign(&mut self, other: Self) { self.value |= other.value }
}

impl<U: Tag> std::ops::BitXor for T<U>
where U::BaseType : std::ops::BitXor<Output = U::BaseType>
{
    type Output = Self;
    
    fn bitxor(self, other: Self) -> Self { Self::new(self.value ^ other.value) }
}

impl<U: Tag> std::ops::BitXorAssign for T<U>
where U::BaseType : std::ops::BitXorAssign
{
    fn bitxor_assign(&mut self, other: Self) { self.value ^= other.value }
}

impl<U: Tag> std::clone::Clone for T<U>
where U::BaseType : std::clone::Clone {
    fn clone(&self) -> Self { Self { value: self.value.clone() } }

    fn clone_from(&mut self, source: &Self) { self.value.clone_from(&source.value) }
}

impl<U: Tag> std::marker::Copy for T<U>
where U::BaseType : std::marker::Copy {
}

impl<U: Tag> std::fmt::Debug for T<U>
where U::BaseType : std::fmt::Debug
{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> Result<(), std::fmt::Error> { self.value.fmt(f) }
}

impl<U: Tag> std::default::Default for T<U>
where U::BaseType : std::default::Default {
    fn default() -> Self { Self { value: Default::default() } }
}

impl<U: Tag> std::fmt::Display for T<U>
where U::BaseType : std::fmt::Display
{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> Result<(), std::fmt::Error> { self.value.fmt(f) }
}

impl<U: Tag> std::ops::Div for T<U>
where U::BaseType : std::ops::Div<Output = U::BaseType>
{
    type Output = Self;
    
    fn div(self, other: Self) -> Self { Self::new(self.value / other.value) }
}

impl<U: Tag> std::ops::DivAssign for T<U>
where U::BaseType : std::ops::DivAssign
{
    fn div_assign(&mut self, other: Self) { self.value /= other.value }
}

// no useful implementation of FloatToInt

//REVISIT: how to avoid clash with std impl<T> From<T> for T?
//         can we somehow constraint such that V != T<U>?
//impl<U: Tag, V> std::convert::From<V> for T<U>
//where U::BaseType : std::convert::From<V>
//{
//    fn from(x: V) -> Self { Self { value: U::BaseType::from(x) } }
//}

impl<U: Tag> std::hash::Hash for T<U>
where U::BaseType : std::hash::Hash
{
    fn hash<H>(&self, state: &mut H)
    where
        H: std::hash::Hasher
    {
	self.value.hash(state);
    }
}

impl<U: Tag> std::fmt::LowerExp for T<U>
where U::BaseType : std::fmt::LowerExp {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> Result<(), std::fmt::Error> { self.value.fmt(f) }
}

impl<U: Tag> std::fmt::LowerHex for T<U>
where U::BaseType : std::fmt::LowerHex {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> Result<(), std::fmt::Error> { self.value.fmt(f) }
}

impl<U: Tag> std::ops::Mul for T<U>
where U::BaseType : std::ops::Mul<Output = U::BaseType>
{
    type Output = Self;
    
    fn mul(self, other: Self) -> Self { Self::new(self.value * other.value) }
}

impl<U: Tag> std::ops::MulAssign for T<U>
where U::BaseType : std::ops::MulAssign
{
    fn mul_assign(&mut self, other: Self) { self.value *= other.value }
}

impl<U: Tag> std::ops::Neg for T<U>
where U::BaseType : std::ops::Neg<Output = U::BaseType>
{
    type Output = Self;
    
    fn neg(self) -> Self { Self::new(- self.value) }
}


impl<U: Tag> std::ops::Not for T<U>
where U::BaseType : std::ops::Not<Output = U::BaseType>
{
    type Output = Self;
    
    fn not(self) -> Self { Self::new(!self.value) }
}
impl<U: Tag> std::fmt::Octal for T<U>
where U::BaseType : std::fmt::Octal {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> Result<(), std::fmt::Error> { self.value.fmt(f) }
}

struct IA<I: Iterator, N>
{
    i : I,
    phantom: std::marker::PhantomData<N>
}

impl<'a, U:'a + Tag, I> Iterator for IA<I, T<U>>
    where I: Iterator<Item=&'a T<U> >
{
    type Item = &'a U::BaseType;
    fn next(&mut self) -> Option<Self::Item> {
	match self.i.next() {
	    Some(v) => { Some(&v.value) },
	    None => { None }
	}
    }
}

impl<'a, U:'a> std::iter::Product<&'a T<U>> for T<U>
where
    U: Tag, <U as Tag>::BaseType:std::iter::Product<&'a <U as Tag>::BaseType>
{
    fn product<I>(iter: I) -> T<U>
    where
        I: Iterator<Item = &'a T<U> >
    {
	T::<U>{ value: <U::BaseType as std::iter::Product<&'a U::BaseType>>::product(IA { i: iter, phantom: std::marker::PhantomData }) }
    }
}

