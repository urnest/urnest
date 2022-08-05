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
	    assert::equal(&result.cause.to_string().as_str(), &"\" w...\" does not start with \"dy\"");
	    assert::equal(&format!("{:#}", result).as_str(), &"1:1: failed to parse \"freddy\" because, having parsed \"fred\",\n1:1: \" w...\" does not start with \"dy\"");
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
	    assert::equal(&e.to_string().as_str(),
			  &"failed to parse jock at line 1 column 1 because failed to parse \"freddy\" at line 1 column 1 because, having parsed \"fred\", \" w...\" does not start with \"dy\"");
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
	    assert::equal(&e.to_string().as_str(), &"failed to parse \"fred\" then \" was\" then \" very\" at line 1 column 1 because, having parsed \"fred\" and parsed \" was\", failed to parse \" very\" at line 1 column 9 because, having parsed \" \", \"real...\" does not start with \"very\"");
	    assert::equal(&format!("{:#}", e).as_str(), &"1:1: failed to parse \"fred\" then \" was\" then \" very\" because, having parsed \"fred\" and parsed \" was\",\n1:9: failed to parse \" very\" because, having parsed \" \",\n1:9: \"real...\" does not start with \"very\"");
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

    let p = parser::list_of(parser::literal("{"),
                            parser::literal("aa") | parser::literal("ab") |
                            parser::literal("ac") | parser::literal("add"),
                            parser::literal(","), parser::literal("}"));
    assert::equal(&p.goal().to_string().as_str(), &"list of \",\"-separated \"aa\" or \"ab\" or \"ac\" or \"add\" items inside \"{\"..\"}\"");
    let x = "{aa,ab,aa}";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: None,
		    text: "{aa,ab,aa}" },
		children: vec!(
                    parser::AST{value: parser::ast::Item{tag: None,text: "{"},children: vec!() },
                    parser::AST{value: parser::ast::Item{tag: None,text: "aa"},children: vec!() },
                    parser::AST{value: parser::ast::Item{tag: None,text: ","},children: vec!() },
                    parser::AST{value: parser::ast::Item{tag: None,text: "ab"},children: vec!() },
                    parser::AST{value: parser::ast::Item{tag: None,text: ","},children: vec!() },
                    parser::AST{value: parser::ast::Item{tag: None,text: "aa"},children: vec!() },
                    parser::AST{value: parser::ast::Item{tag: None,text: "}"},children: vec!() })
            });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }
    let x = "{az,ab,aa}";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse list of \",\"-separated \"aa\" or \"ab\" or \"ac\" or \"add\" items inside \"{\"..\"}\" at line 1 column 1 because, having parsed \"{\", failed to parse \"aa\" or \"ab\" or \"ac\" or \"add\" at line 1 column 2 because failed to parse \"aa\" at line 1 column 2 because, having parsed \"a\", \"z...\" does not start with \"a\"");
	}
    }
    let x = "{aa;ab,aa}";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse list of \",\"-separated \"aa\" or \"ab\" or \"ac\" or \"add\" items inside \"{\"..\"}\" at line 1 column 1 because, having parsed \"{\" and parsed some items, failed to parse \"}\" at line 1 column 4 because \";...\" does not start with \"}\"");
	}
    }
    let x = "{aa,ab;aa}";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse list of \",\"-separated \"aa\" or \"ab\" or \"ac\" or \"add\" items inside \"{\"..\"}\" at line 1 column 1 because, having parsed \"{\" and parsed some items, failed to parse \"}\" at line 1 column 7 because \";...\" does not start with \"}\"");
	}
    }
    let x = "{aa,adq,aa}";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse list of \",\"-separated \"aa\" or \"ab\" or \"ac\" or \"add\" items inside \"{\"..\"}\" at line 1 column 1 because, having parsed \"{\" and parsed some items, failed to parse \"aa\" or \"ab\" or \"ac\" or \"add\" at line 1 column 5 because failed to parse \"add\" at line 1 column 5 because, having parsed \"ad\", \"q...\" does not start with \"d\"");
	}
    }
    let x = "{aa,add,aa,}";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse list of \",\"-separated \"aa\" or \"ab\" or \"ac\" or \"add\" items inside \"{\"..\"}\" at line 1 column 1 because, having parsed \"{\" and parsed some items, failed to parse \"aa\" or \"ab\" or \"ac\" or \"add\" at line 1 column 12 because failed to parse \"aa\" at line 1 column 12 because \"}\" does not start with \"aa\"");
	}
    }


    let p = parser::cr();
    assert::equal(&p.goal().to_string().as_str(), &"carriage-return");
    let x = "\r";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: Some("carriage-return"),
		    text: "\r" },
		children: vec!()
            });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }
    let x = "fred\n";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse carriage-return at line 1 column 1 because expected '\r' not 'f'");
	}
    }


    let p = parser::lf();
    assert::equal(&p.goal().to_string().as_str(), &"line-feed");
    let x = "\n";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: Some("line-feed"),
		    text: "\n" },
		children: vec!()
            });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }
    let x = "fred\r";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse line-feed at line 1 column 1 because expected '\n' not 'f'");
	}
    }


    let p = parser::crlf();
    assert::equal(&p.goal().to_string().as_str(), &"CRLF");
    let x = "\r\n";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: Some(parser::CRLF),
		    text: "\r\n" },
		children: vec!(
                    parser::AST{
		        value: parser::ast::Item{
		            tag: None,
		            text: "\r\n" },
		        children: vec!(
                            parser::AST{
		                value: parser::ast::Item{
		                    tag: Some("carriage-return"),
		                    text: "\r" },
		                children: vec!()},
                            parser::AST{
		                value: parser::ast::Item{
		                    tag: Some("line-feed"),
		                    text: "\n" },
		                children: vec!()})})
            });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }
    let x = "fred\r";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse CRLF at line 1 column 1 because failed to parse carriage-return then line-feed at line 1 column 1 because failed to parse carriage-return at line 1 column 1 because expected '\r' not 'f'");
	}
    }


    let p = parser::digit();
    assert::equal(&p.goal().to_string().as_str(), &"digit");
    for c in "0123456789".chars() {
        let a = format!("{c}x", c=c);
        let x = a.as_str();
        let y = p.parse_some_of(x);
        match y {
	    parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: Some("digit"),
		    text: &x[0..1] },
		children: vec!()
            });
	    },
	    parser::ParseResult::Err(_e) => {
	        assert::equal(&true, &false);
	    }
        }
    }
    let x = "A34";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse digit at line 1 column 1 because expected 0..9 not 'A'");
	}
    }


    let p = parser::octal_digit();
    assert::equal(&p.goal().to_string().as_str(), &"octal digit");
    for c in "01234567".chars() {
        let a = format!("{c}x", c=c);
        let x = a.as_str();
        let y = p.parse_some_of(x);
        match y {
	    parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: Some("octal digit"),
		    text: &x[0..1] },
		children: vec!()
            });
	    },
	    parser::ParseResult::Err(_e) => {
	        assert::equal(&true, &false);
	    }
        }
    }
    let x = "834";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse octal digit at line 1 column 1 because expected 0..7 not '8'");
	}
    }


    let p = parser::hex_digit();
    assert::equal(&p.goal().to_string().as_str(), &"hex digit");
    for c in "01234567abcdefABCDEF".chars() {
        let a = format!("{c}x", c=c);
        let x = a.as_str();
        let y = p.parse_some_of(x);
        match y {
	    parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: Some("hex digit"),
		    text: &x[0..1] },
		children: vec!()
            });
	    },
	    parser::ParseResult::Err(_e) => {
	        assert::equal(&true, &false);
	    }
        }
    }
    let x = "g";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse hex digit at line 1 column 1 because expected 0..7, a..f or A..F not 'g'");
	}
    }


    let p = parser::us_ascii_printable();
    assert::equal(&p.goal().to_string().as_str(), &"US ASCII printable character");
    for i in 32..127 {
        let a = format!("{c}x", c=char::from_u32(i).unwrap());
        let x = a.as_str();
        let y = p.parse_some_of(x);
        match y {
	    parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: Some("US ASCII printable character"),
		    text: &x[0..1] },
		children: vec!()
            });
	    },
	    parser::ParseResult::Err(_e) => {
	        assert::equal(&true, &false);
	    }
        }
    }
    let x = "\t";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse US ASCII printable character at line 1 column 1 because expected a..z, A..Z, 0..9, space, one of !\"#$%&'()*+,-./:;<=>?@[\\]^_{|}~ or delete '\t'");
	}
    }


    let p = parser::any_char();
    assert::equal(&p.goal().to_string().as_str(), &"any character");
    let x = "f";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: None,
		    text: &x[0..1] },
		children: vec!()
            });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }
    let x = "";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse any character at line 1 column 1 because end of input");
	}
    }


    let p = parser::at_least_one(parser::digit());
    assert::equal(&p.goal().to_string().as_str(), &"at least one digit");
    let x = "1234a";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: None,
		    text: &x[0..4] },
		children: vec!(
                    parser::AST{
		        value: parser::ast::Item{
		            tag: Some("digit"),
		            text: "1" },
		        children: vec!()
                    },
                    parser::AST{
		        value: parser::ast::Item{
		            tag: Some("digit"),
		            text: "2" },
		        children: vec!()
                    },
                    parser::AST{
		        value: parser::ast::Item{
		            tag: Some("digit"),
		            text: "3" },
		        children: vec!()
                    },
                    parser::AST{
		        value: parser::ast::Item{
		            tag: Some("digit"),
		            text: "4" },
		        children: vec!()
                    })
            });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }
    let x = "a";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse at least one digit at line 1 column 1 because failed to parse digit at line 1 column 1 because expected 0..9 not 'a'");
	}
    }


    let p = parser::one_of_chars(parser::CharSet{value: "a-z0-9_-"});
    assert::equal(&p.goal().to_string().as_str(), &"one of characters a-z0-9_-");
    for c in "01234567abcdefghijklmnopqrstuvwxyz_-".chars() {
        let a = format!("{c}x", c=c);
        let x = a.as_str();
        let y = p.parse_some_of(x);
        match y {
	    parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: None,
		    text: &x[0..1] },
		children: vec!()
            });
	    },
	    parser::ParseResult::Err(_e) => {
	        assert::equal(&true, &false);
	    }
        }
    }
    let x = "!";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse one of characters a-z0-9_- at line 1 column 1 because expected one of characters a-z0-9_- not '!'");
	}
    }


    let p = parser::one_of_chars(parser::CharSet{value: "f-a"});
    assert::equal(&p.goal().to_string().as_str(), &"one of characters f-a");
    for c in "af-".chars() {
        let a = format!("{c}x", c=c);
        let x = a.as_str();
        let y = p.parse_some_of(x);
        match y {
	    parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: None,
		    text: &x[0..1] },
		children: vec!()
            });
	    },
	    parser::ParseResult::Err(_e) => {
	        assert::equal(&true, &false);
	    }
        }
    }
    let x = "b";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse one of characters f-a at line 1 column 1 because expected one of characters f-a not 'b'");
	}
    }

    let p = parser::one_of_chars(parser::CharSet{value: "-af"});
    assert::equal(&p.goal().to_string().as_str(), &"one of characters -af");
    for c in "af-".chars() {
        let a = format!("{c}x", c=c);
        let x = a.as_str();
        let y = p.parse_some_of(x);
        match y {
	    parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: None,
		    text: &x[0..1] },
		children: vec!()
            });
	    },
	    parser::ParseResult::Err(_e) => {
	        assert::equal(&true, &false);
	    }
        }
    }
    let x = "b";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse one of characters -af at line 1 column 1 because expected one of characters -af not 'b'");
	}
    }

    let p = parser::one_of_chars(parser::CharSet{value: "af-"});
    assert::equal(&p.goal().to_string().as_str(), &"one of characters af-");
    for c in "af-".chars() {
        let a = format!("{c}x", c=c);
        let x = a.as_str();
        let y = p.parse_some_of(x);
        match y {
	    parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: None,
		    text: &x[0..1] },
		children: vec!()
            });
	    },
	    parser::ParseResult::Err(_e) => {
	        assert::equal(&true, &false);
	    }
        }
    }
    let x = "b";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse one of characters af- at line 1 column 1 because expected one of characters af- not 'b'");
	}
    }

    let p = parser::any_char_except(parser::CharSet{value: "a-z0-9_-"});
    assert::equal(&p.goal().to_string().as_str(), &"any character except a-z0-9_-");
    for c in "?/.,".chars() {
        let a = format!("{c}x", c=c);
        let x = a.as_str();
        let y = p.parse_some_of(x);
        match y {
	    parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: None,
		    text: &x[0..1] },
		children: vec!()
            });
	    },
	    parser::ParseResult::Err(_e) => {
	        assert::equal(&true, &false);
	    }
        }
    }
    let x = "-";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse any character except a-z0-9_- at line 1 column 1 because '-' is one of characters a-z0-9_-");
	}
    }

    let p = parser::whitespace_char();
    assert::equal(&p.goal().to_string().as_str(), &"whitespace character");
    for c in " \t\n\r".chars() {
        let a = format!("{c}x", c=c);
        let x = a.as_str();
        let y = p.parse_some_of(x);
        match y {
	    parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: None,
		    text: &x[0..1] },
		children: vec!()
            });
	    },
	    parser::ParseResult::Err(_e) => {
	        assert::equal(&true, &false);
	    }
        }
    }
    let x = "b";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse whitespace character at line 1 column 1 because expected space, tab, newline or carriage-return not 'b'");
	}
    }

    let p = parser::parse_x_until_y(parser::digit(), parser::literal("."));
    let x = "1234.";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: None,
		    text: &x[0..5] },
		children: vec!(
                    parser::AST{
		        value: parser::ast::Item{
		            tag: Some("digit"),
		            text: &x[0..1] },
		        children: vec!()
                    },
                    parser::AST{
		        value: parser::ast::Item{
		            tag: Some("digit"),
		            text: &x[1..2] },
		        children: vec!()
                    },
                    parser::AST{
		        value: parser::ast::Item{
		            tag: Some("digit"),
		            text: &x[2..3] },
		        children: vec!()
                    },
                    parser::AST{
		        value: parser::ast::Item{
		            tag: Some("digit"),
		            text: &x[3..4] },
		        children: vec!()
                    },
                    parser::AST{
		        value: parser::ast::Item{
		            tag: None,
		            text: &x[4..5] },
		        children: vec!()
                    })
            });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }
    let x = "1234b";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse digit repeatedly until \".\" at line 1 column 1 because, having parsed some items, failed to parse \".\" at line 1 column 5 because \"b\" does not start with \".\"");
	}
    }

    let p = parser::zero_or_more(parser::digit());
    let x = "1234";
    for i in 0..x.len() {
        let y = p.parse_some_of(&x[0..i]);
        match y {
	    parser::ParseResult::Ok( ast ) => {
                let mut expected_children = vec!();
                for j in 0..i {
                    expected_children.push(
                        parser::AST{
		            value: parser::ast::Item{
		                tag: Some("digit"),
		                text: &x[j..j+1] },
		            children: vec!()
                        });
                }
	        assert::equal(&ast, &parser::AST{
		    value: parser::ast::Item{
		        tag: None,
		        text: &x[0..i] },
		    children: expected_children
                });
	    },
	    parser::ParseResult::Err(_e) => {
	        assert::equal(&true, &false);
	    }
        }
    }

    let p = parser::some_space();
    let x = " \tb";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	        assert::equal(&ast, &parser::AST{
		    value: parser::ast::Item{
		        tag: None,
		        text: &x[0..2] },
		    children: vec!()
                });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }
    let x = "1";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse some space at line 1 column 1 because failed to parse whitespace character at line 1 column 1 because expected space, tab, newline or carriage-return not '1'");
	}
    }

    let p = parser::any_space();
    assert::equal(&p.goal().to_string().as_str(), &"any space");
    let x = "   \t\n  b";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: None,
		    text: &x[0..7]},
		children: vec!()
            });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }

    let x = "";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: None,
		    text: ""},
		children: vec!()
            });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }

    let p = parser::optional(parser::digit());
    assert::equal(&p.goal().to_string().as_str(), &"optional digit");
    let x = "1c";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: None,
		    text: &x[0..1]},
		children: vec!(
                    parser::AST{
		        value: parser::ast::Item{
		            tag: Some("digit"),
		            text: &x[0..1]},
		        children: vec!()
                    })
            });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }

    let x = "";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	    assert::equal(&ast, &parser::AST{
		value: parser::ast::Item{
		    tag: None,
		    text: ""},
		children: vec!()
            });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }

    let p = parser::select(
        (parser::literal("22"),
         parser::literal("22a")),
        vec!(
            (parser::literal("2")+parser::digit(),
             parser::literal("2")+parser::digit()+parser::literal("b")),
            (parser::literal("3"),
             parser::literal("3")+parser::digit())));
    let x = "22ax";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	        assert::equal(&ast, &parser::AST{
		    value: parser::ast::Item{
		        tag: None,
		        text: &"22a" },
		    children: vec!()
                });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }
    let x = "27bx";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	        assert::equal(&ast, &parser::AST{
		    value: parser::ast::Item{
		        tag: None,
		        text: &x[0..3] },
		    children: vec!(
                        parser::AST{
		            value: parser::ast::Item{
		                tag: None,
		                text: &"2" },
		            children: vec!()
                        },
                        parser::AST{
		            value: parser::ast::Item{
		                tag: Some("digit"),
		                text: &"7" },
		            children: vec!()
                        },
                        parser::AST{
		            value: parser::ast::Item{
		                tag: None,
		                text: &"b" },
		            children: vec!()
                        })
                });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }
    let x = "39";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( ast ) => {
	        assert::equal(&ast, &parser::AST{
		    value: parser::ast::Item{
		        tag: None,
		        text: &x[0..2] },
		    children: vec!(
                        parser::AST{
		            value: parser::ast::Item{
		                tag: None,
		                text: &"3" },
		            children: vec!()
                        },
                        parser::AST{
		            value: parser::ast::Item{
		                tag: Some("digit"),
		                text: &"9" },
		            children: vec!()
                        })
                });
	},
	parser::ParseResult::Err(_e) => {
	    assert::equal(&true, &false);
	}
    }
    let x = "22b";
    let y = p.parse_some_of(x);
    match y {
	parser::ParseResult::Ok( _ast ) => {
	    assert::equal(&true, &false);
	},
	parser::ParseResult::Err(e) => {
	    assert::equal(&e.to_string().as_str(), &"failed to parse \"22\" => \"22a\" or \"2\" then digit => \"2\" then digit then \"b\" or \"3\" => \"3\" then digit at line 1 column 1 because failed to parse \"22a\" at line 1 column 1 because, having parsed \"22\", \"b\" does not start with \"a\"");
	}
    }

    let x = r###"
{
  "a": 1,
  "b": {
    "c": 2
  }
}
"###;
    let ___ = parser::any_space();
    let comma = parser::literal(",");
    let name_p = parser::tagged(
        "name", parser::literal(r#"""#)+parser::one_of_chars("a-z")+parser::literal(r#"""#));
    let s = parser::back_ref();
    let p = parser::select(
        (parser::literal("{"), (
            parser::tagger("object",
                           parser::list_of(parser::literal("{")+___,
                                           name_p+___+s+___+comma+___,
                                           parser::literal("}")+___)))),
        vec!((parser::digit(), (parser::tagger("int",parser::digit()+___)))));
    s.recurse_to(p);
    @@@;
}
