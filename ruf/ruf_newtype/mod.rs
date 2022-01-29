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

