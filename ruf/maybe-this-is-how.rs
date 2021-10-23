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
    fn children(self : &'a mut Selection<'a>)  -> Selection<'a> {
	// this -------^^^^^^^^^^^^^^^^^^^^^
	// says "when calling this method (children), the reference you
	// pass must outlive (i.e. have same or longer lifetime)
	// as the thing it references. (More often, references passed
	// into functions only need live for duration of function - which
	// is what fn f<'a>(x : &mut Section<'a>) implies - but in
	// our case we want this reference to "cancel" any previous
	// references into the tree, because those previous references
	// might be invalidated by operations on the Selection we create.)
	Selection::<'a> { tree: self.tree }
    }
    fn get_x(&'a self) -> i32 {
	self.tree.x
    }
}

fn y<'a>(s1 : &'a mut Selection<'a>){
    let s2 = s1.children();
    println!("{}", s2.get_x());
}

fn main() {
    let mut t = Tree{ x:2 };
    let mut s1 = t.root();
    {
	y(&mut s1);
    }
    println!("{}", s1.get_x());
}
