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
use parser::Parser;

fn main() {
    let x = "fred was very good";
    let y = parser::parse::Literal{ x: "fred" }.parse_some_of(x);
    match y.ok() {
	None => {
	    assert::equal(&true, &false);
	},
	Some(result) => {
	    assert::equal(&result,
			  &(
			      parser::AST{
				  value: parser::ast::Item{
				      tag: None,
				      text: x.get(0..4).unwrap() },
				  children: vec!() },
			      x.get(4..).unwrap() ) );
	}
    }
    let y = parser::parse::Literal{ x: "freddy" }.parse_some_of(x);
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
	parser::ParseResult::Ok( (ast, rest) ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: Some("jock"),
		    text: "fred" },
		children: vec!( parser::AST{
		    value: parser::ast::Item{
			tag: None,
			text: x.get(0..4).unwrap() },
		    children: vec!() } ) });
	    assert::equal(&rest, &x.get(4..).unwrap());
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

    let p = parser::literal("fred") + parser::literal(" was");
    assert::equal(&p.goal().to_string().as_str(), &"parse \"fred\" then parse \" was\"");
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( (ast, rest) ) => {
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
	    assert::equal(&rest, &" very good");
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }
}
