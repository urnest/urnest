pub fn equal<T>(a: &T, b: &T)
    where T: std::cmp::PartialEq
{
    if !(*a == *b){
	::std::process::abort();
    }
}
