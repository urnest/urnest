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

impl<U: Tag> std::ops::Add for T<U>
where U::BaseType : std::ops::Add<Output = U::BaseType>
{
    type Output = Self;
    
    fn add(self, other: Self) -> Self { Self::new(self.value + other.value) }
}
