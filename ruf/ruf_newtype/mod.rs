// new-type trait class

pub struct T<BaseType, TagType>
{
    value : BaseType,
    phantom : std::marker::PhantomData<TagType>
}

impl<BaseType, TagType> T<BaseType, TagType>
{
    pub fn new(value : BaseType) -> T<BaseType, TagType> {
	T::<BaseType, TagType>{ value: value, phantom: std::marker::PhantomData{} }
    }
}

impl<BaseType, TagType> std::cmp::PartialEq for T<BaseType, TagType>
where BaseType : std::cmp::PartialEq
{
    fn eq(&self, other: &Self) -> bool { self.value == other.value }
}

impl<BaseType, TagType> std::cmp::Eq for T<BaseType, TagType>
where BaseType : std::cmp::Eq
{
}

impl<BaseType, TagType> std::ops::Add for T<BaseType, TagType>
where BaseType : std::ops::Add<Output = BaseType>
{
    type Output = Self;
    
    fn add(self, other: Self) -> Self { Self::new(self.value + other.value) }
}
