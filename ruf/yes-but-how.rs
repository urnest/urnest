struct Tree
{
    x : i32,
}

struct Selection<'a>
{
    tree: &'a mut Tree,
}

impl Tree {
    fn root<'a>(self: &'a mut Tree) -> Selection<'a> {
	Selection::<'a> { tree: self }
    }
}

impl<'a> Selection<'a>
{
    fn children(self : & mut Selection<'a>) -> Selection<'a> {
	Selection::<'a> { tree: self.tree }
    }
    fn get_x(&'a self) -> i32 {
	self.tree.x
    }
}

fn main() {
    let mut t = Tree{ x:2 };
    let mut s1 = t.root();
    {
	let s2 = s1.children();
	println!("{}", s2.get_x());
    }
    println!("{}", s1.get_x());
}
