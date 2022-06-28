// Copyright (c) 2021 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

extern crate ruf_parser;
extern crate ruf_assert;

use ruf_parser as parser;
use ruf_assert as assert;

// This is just to demonstrate that we can combine parsers of different
// lifetime (can't just create a static parser::Ref as it doesn't meet
// the "const" function limitations of static variables).
// In practice there's no significant advantage creating lazy static parsers as they
// are cheap to create anyway and unlikely to ever lead to optimisation.
fn static_parse_very() -> parser::Ref<'static>
{
    unsafe{ // replace with std::lazy::Lazy when it becomes main stream
        static mut P: Option<parser::Ref> = None;
        static O: std::sync::Once = std::sync::Once::new();
        O.call_once(|| {
            P = Some(parser::literal(" very"));
        });
        return P.as_ref().unwrap().clone();
    }
}

        
fn main() {
    let x = "fred was very good";
    let p = parser::literal("fred");
    let y = p.parse_some_of(x);
    match y.ok() {
	None => {
	    assert::equal(&true, &false);
	},
	Some(result) => {
	    assert::equal(&result,
			  &parser::AST{
			      value: parser::ast::Item{
				  tag: None,
				  text: x.get(0..4).unwrap() },
			      children: vec!() } );
	}
    }
    let p = parser::literal("freddy");
    let y = p.parse_some_of(x);
    match y.err() {
	None => {
	    assert::equal(&true, &false);
	},
	Some(result) => {
	    assert::equal(&result.cause.to_string().as_str(), &"\"fred w...\" does not start with \"freddy\"");
	    assert::equal(&format!("{:#}", result).as_str(), &"1:1: failed to parse \"freddy\" because\n1:1: \"fred w...\" does not start with \"freddy\"");
	}
    }
    let p = parser::tagged("jock", parser::literal("fred"));
    
    let y = p.parse_some_of(x);
    
    match y {
	parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: Some("jock"),
		    text: "fred" },
		children: vec!( parser::AST{
		    value: parser::ast::Item{
			tag: None,
			text: x.get(0..4).unwrap() },
		    children: vec!() } ) });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }

    let p = parser::tagged("jock", parser::literal("freddy"));
    
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok(_x) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.cause.to_string().as_str(),
			  &"\"fred w...\" does not start with \"freddy\"");
	    assert::equal(&e.to_string().as_str(),
			  &"failed to parse jock at line 1 column 1 because failed to parse \"freddy\" at line 1 column 1 because \"fred w...\" does not start with \"freddy\"");
	}
    }

    let mut lc = parser::LineCol{ line:1, col:1 };
    lc.advance_through("fred");
    assert::equal(&lc, &parser::LineCol{ line:1, col:5 });
    lc.advance_through("dy\njon");
    assert::equal(&lc, &parser::LineCol{ line:2, col:4 });
    lc.advance_through("dy\rallen");
    assert::equal(&lc, &parser::LineCol{ line:2, col:6 });
    lc.advance_through("\rak\tb");
    assert::equal(&lc, &parser::LineCol{ line:2, col:9 });
    lc.advance_through("\n\tb");
    assert::equal(&lc, &parser::LineCol{ line:3, col:9 });
    lc.advance_through("\n123456\tb");
    assert::equal(&lc, &parser::LineCol{ line:4, col:9 });
    lc.advance_through("\n1234567\tb");
    assert::equal(&lc, &parser::LineCol{ line:5, col:17 });
    lc.advance_through("\n12345678\tb");
    assert::equal(&lc, &parser::LineCol{ line:6, col:17 });
    lc.advance_through("\n\t\tb");
    assert::equal(&lc, &parser::LineCol{ line:7, col:17 });
    lc.advance_through("\n\t");
    assert::equal(&lc, &parser::LineCol{ line:8, col:8 });

    let p = parser::literal("fred").clone() + parser::literal(" was").clone();
    assert::equal(&p.goal().to_string().as_str(), &"\"fred\" then \" was\"");
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: None,
		    text: "fred was" },
		children: vec!(
                    parser::AST{
		        value: parser::ast::Item{
			    tag: None,
			    text: "fred"},
		        children: vec!() },
                    parser::AST{
		        value: parser::ast::Item{
			    tag: None,
			    text: " was"},
		        children: vec!() }
                ),
            });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }
    let p = parser::literal("fred") + parser::literal(" was") + parser::literal(" very");
    assert::equal(&p.goal().to_string().as_str(), &"\"fred\" then \" was\" then \" very\"");
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: None,
		    text: "fred was very" },
		children: vec!(
                    parser::AST{
		        value: parser::ast::Item{
			    tag: None,
			    text: "fred"},
		        children: vec!() },
                    parser::AST{
		        value: parser::ast::Item{
			    tag: None,
			    text: " was"},
		        children: vec!() },
                    parser::AST{
		        value: parser::ast::Item{
			    tag: None,
			    text: " very"},
		        children: vec!() }
                ),
            });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }

    let p = parser::literal("fred") + parser::literal(" was") + static_parse_very();
    assert::equal(&p.goal().to_string().as_str(), &"\"fred\" then \" was\" then \" very\"");
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: None,
		    text: "fred was very" },
		children: vec!(
                    parser::AST{
		        value: parser::ast::Item{
			    tag: None,
			    text: "fred"},
		        children: vec!() },
                    parser::AST{
		        value: parser::ast::Item{
			    tag: None,
			    text: " was"},
		        children: vec!() },
                    parser::AST{
		        value: parser::ast::Item{
			    tag: None,
			    text: " very"},
		        children: vec!() }
                ),
            });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }

    let x = "fred was really good";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse \"fred\" then \" was\" then \" very\" at line 1 column 1 because, having parsed fred and parsed \" was\", failed to parse \" very\" at line 1 column 9 because \" real...\" does not start with \" very\"");
	    assert::equal(&format!("{:#}", e).as_str(), &"1:1: failed to parse \"fred\" then \" was\" then \" very\" because, having parsed fred and parsed \" was\",\n1:9: failed to parse \" very\" because\n1:9: \" real...\" does not start with \" very\"");
	}
    }

    let p = parser::literal("fred") | parser::literal(" was") | static_parse_very();
    assert::equal(&p.goal().to_string().as_str(), &"\"fred\" or \" was\" or \" very\"");
    let x = "fred was very good";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: None,
		    text: "fred" },
		children: vec!(),
            });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }

    let p = parser::literal("fred") | parser::literal(" was") | static_parse_very();
    assert::equal(&p.goal().to_string().as_str(), &"\"fred\" or \" was\" or \" very\"");
    let x = " was very good";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: None,
		    text: " was" },
		children: vec!(),
            });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }


    let p = parser::literal("fred") + (parser::literal(" was") | static_parse_very());
    assert::equal(&p.goal().to_string().as_str(), &"\"fred\" then \" was\" or \" very\"");
    let x = "fred very good";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: None,
		    text: "fred very" },
		children: vec!(
                    parser::AST{
		        value: parser::ast::Item{
			    tag: None,
			    text: "fred"},
		        children: vec!() },
                    parser::AST{
		        value: parser::ast::Item{
			    tag: None,
			    text: " very"},
		        children: vec!() }),
            });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }

    let p = parser::literal("fred") + parser::none()+(parser::literal(" was") | static_parse_very());
    assert::equal(&p.goal().to_string().as_str(), &"\"fred\" then nothing then \" was\" or \" very\"");
    let x = "fred very good";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: None,
		    text: "fred very" },
		children: vec!(
                    parser::AST{
		        value: parser::ast::Item{
			    tag: None,
			    text: "fred"},
		        children: vec!() },
                    parser::AST{
		        value: parser::ast::Item{
			    tag: None,
			    text: ""},
		        children: vec!() },
                    parser::AST{
		        value: parser::ast::Item{
			    tag: None,
			    text: " very"},
		        children: vec!() }),
            });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }
}