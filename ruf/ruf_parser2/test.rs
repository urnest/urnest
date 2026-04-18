// Copyright (c) 2021 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

extern crate ruf_parser2;
extern crate ruf_assert;

use ruf_parser2::{
    AST,
    Outcome,
    Unexpected,
    Goal,
    none,
    char,
    literal,
    tagged,
    end_of_input,
    ParseFailed,
    Parsed,
    Context,
    any_char,
    digit,
    octal_digit,
    hex_digit,
    at_least_one,
    zero_or_more,
    CharSet,
    one_of_chars,
    any_char_except,
    CR, LF, CRLF,
    cr, lf, crlf,
    trailing_space,
    some_inline_space,
    any_inline_space,
    rest_of_line_blank,
    some_space,
    any_space,
    eat_white,
    TRAILING_SPACE,
    REST_OF_LINE_BLANK,
    INLINE_SPACE,
    WHITESPACE,
    parse_x_until_y,
    us_ascii_printable,
    list_of,
    parse_balanced_until_y,
    backref,
    backrefs,
    switch,
};

use ruf_parser2::ast::Item;
use ruf_parser2::all_of::all_of;
use ruf_assert as assert;

fn tutorial() {
    // build a parser like a grammar, marking interesting bits via tagged()
    // parser; tags are always 'static

    // example:
    let some_text = "Fred Jones has red eyes and is left handed and right footed.";
    // interested in ^^^^^^^^^^     ^^^             ^^^^

    // tags for the interesting bits:
    let full_name = "full name";
    let first_name = "first name";
    let last_name = "last name";
    let eye_colour = "eye colour";
    let handedness = "handedness";
    let lefty = "lefty";
    let righty = "righty";

    // parsers for components, defined as functions so we don't litter the
    // following definition with clone() calls
    let eyes = || literal("eyes");
    let is = || literal("is");
    let has = || literal("has");
    let and = || literal("and");
    let handed = || literal("handed");
    let footed = || literal("footed.");
    let left = || literal("left");
    let right = || literal("right");
    let parse_left = || tagged(lefty, left());
    let parse_right = || tagged(righty, right());
    let ws = || some_space();
    let parser =
        tagged(full_name, // nesting makes it easier to search unambiguously
               tagged(first_name, parse_x_until_y(any_char(), ws())) + ws() +
               tagged(last_name, parse_x_until_y(any_char(), ws() + has()))) + ws() + has() + ws() +
        tagged(eye_colour, parse_x_until_y(any_char(), ws()+eyes())) +
        ws()+eyes()+ws()+and()+ws()+is()+ws()+
        tagged(handedness, parse_left() | parse_right()) + ws() + handed() + ws() + and() + ws() +
        (parse_left() | parse_right()) + ws() + footed() + end_of_input();

    // parsing into an tagged tree is two stages:
    // - parse, which produces a tree of all parser outcomes, including failures
    // - extracting tagged tree, which has one node for each successful tagged()
    //   ... or has summary of the "best" match in case parsing failed.
    // if we're only interested in the final tree, combine the two stages:
    let ast = parser.parse(some_text).get_ast("root").ok().unwrap();
    
    // then can pull the relevant info out of the tree:
    assert::equal(
        &vec!(
            ast.select_by_value(&|v| v.tag == first_name)
                .copy_selected_values().first().unwrap().text.to_string(),
            ast.select_by_value(&|v| v.tag == last_name)
                .copy_selected_values().first().unwrap().text.to_string(),
            ast.select_by_value(&|v| v.tag == eye_colour)
                .copy_selected_values().first().unwrap().text.to_string(),
            ast.select_by_value(&|v| v.tag == handedness)
                .copy_selected_values().first().unwrap().text.to_string(),
        ),
        &vec!(
            "Fred".to_string(),
            "Jones".to_string(),
            "red".to_string(),
            "left".to_string(),
        )
    )
}

fn main() {
    tutorial();

    // below are test cases... not as easy to read as tutorial but
    // comprehensive.
    
    let root="root";
    
    // end_of_input
    let x = "";
    let p = end_of_input();
    let r = p.parse(&x);
    assert::equal(&r,
                  &Parsed {
                      goal: Goal{ parser: &*p, text: &x },
                      outcome: Outcome::Leaf{
                          matched: &x,
                          then: None,  // success (end of input)
                      }
                  });
    assert::equal(&r.get_ast(root),
                  &Ok(AST{ value:Item{ tag: root, text: &x }, children: vec!()}));
    
    let x = "left over";
    let r = p.parse(&x);
    assert::equal(&r,
                  &Parsed {
                      goal: Goal{ parser: &*p, text: &x },
                      outcome: Outcome::Leaf{
                          matched: &x[0..0],
                          then: Some(Unexpected::Char),  // not at end of input!
                      }
                  });
    assert::equal(&r.get_ast(root),
                  &Err(ParseFailed{
                      at: all_of(x).after(&x[..0]),
                      why: Unexpected::Char,
                      context: vec!(Context{tag: root, text: &x})
                  }));
    
    // none
    let x = "f";
    let p = none();
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..0],
            then: None
        }});
    assert::equal(&r.get_ast(root),
                  &Ok(AST{ value:Item{ tag: root, text: &x[0..0] }, children: vec!()}));
    
    let x = "";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..0],
            then: None
        }});
    assert::equal(&r.get_ast(root),
                  &Ok(AST{ value:Item{ tag: root, text: &x[0..0] }, children: vec!()}));


    // char
    let x = "fr";
    let p = char('f');
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..1],
            then: None
        }});
    
    let x = "gr";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..0],
            then: Some(Unexpected::Char)
        }
    });
    
    let x = "";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..0],
            then: Some(Unexpected::EndOfInput)
        }});
    
    // literal
    let x = "freddy was very good";
    let p = literal("freddy");
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..6],
            then: None
        }});
    assert::equal(&r.get_ast(root),
                  &Ok(AST{ value:Item{ tag: root, text: &x[0..6] }, children: vec!()}));
    
    let x = "fredy was very good";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..4],
            then: Some(Unexpected::Char)
        }
    });
    assert::equal(&r.get_ast(root),
                  &Err(ParseFailed{
                      at: all_of(x).after(&x[0..4]),
                      why: Unexpected::Char,
                      context: vec!(Context{tag: root, text: &x})
                  }));
    
    let x = "fredd";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..5],
            then: Some(Unexpected::EndOfInput)
        }});
    assert::equal(&r.get_ast(root),
                  &Err(ParseFailed{
                      at: all_of(x).after(&x[0..5]),
                      why: Unexpected::EndOfInput,
                      context: vec!(Context{tag: root, text: &x})
                  }));
    
    // p1 + p2 i.e. parse p1 then p2
    let x = "freddy was very good";
    let p1 = literal("freddy");
    let p2 = literal(" was");
    let p = p1.clone() + p2.clone();
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Composite{
            matched: Some(&x[0..10]),  // success, matched "freddy was"
            components: vec!(
                (Goal{parser: &*p1, text: x},
                 Outcome::Leaf{
                     matched: &x[0..6],
                     then: None}),
                (Goal{ parser: &*p2, text: &x[6..]},
                 Outcome::Leaf{
                     matched: &x[6..10],
                     then: None}),
            )}});
    assert::equal(&r.get_ast(root),
                  &Ok(AST{ value:Item{ tag: root, text: &x[0..10] }, children: vec!()}));
    
    let x = "fred was very good";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Composite{
            matched: None,  // failed, reason is captured in components 
            components: vec!(                         //              |
                (Goal{parser: &*p1, text: x},         //              |
                 Outcome::Leaf{                       //              |
                     matched: &x[0..4],               // "fred" looked good...
                     then: Some(Unexpected::Char)     // ... but no "dy" etc
                 }),
            )}});
    assert::equal(&r.get_ast(root),
                  &Err(ParseFailed{
                      at: all_of(x).after(&x[0..4]),
                      why: Unexpected::Char,
                      context: vec!(Context{tag: root, text: &x})
                  }));
                  
    let x = "freddy wis very good";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Composite{
            matched: None,
            components: vec!(
                (Goal{parser: &*p1, text: x},
                 Outcome::Leaf{
                     matched: &x[0..6],
                     then: None}),
                (Goal{ parser: &*p2, text: &x[6..]},
                 Outcome::Leaf{
                     matched: &x[6..8],
                     then: Some(Unexpected::Char)}),
            )}});
    assert::equal(&r.get_ast(root),
                  &Err(ParseFailed{
                      at: all_of(x).after(&x[0..8]),
                      why: Unexpected::Char,
                      context: vec!(Context{tag: root, text: &x})
                  }));
    
    let x = "freddy wa";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Composite{
            matched: None,
            components: vec!(
                (Goal{parser: &*p1, text: x},
                 Outcome::Leaf{
                     matched: &x[0..6],
                     then: None}),
                (Goal{ parser: &*p2, text: &x[6..]},
                 Outcome::Leaf{
                     matched: &x[6..9],
                     then: Some(Unexpected::EndOfInput)}),
            )}});
    assert::equal(&r.get_ast(root),
                  &Err(ParseFailed{
                      at: all_of(x).after(&x[0..9]),
                      why: Unexpected::EndOfInput,
                      context: vec!(Context{tag: root, text: &x})
                  }));
    
    // p1 + (p3 | p4) i.e. parse p1 and either p3 or p4 (trying p3 first)
    let x = "freddy was very good";
    let p1 = literal("freddy");
    let p2 = literal(" was");
    let p3 = literal(" is");
    let p4 = p2.clone() | p3.clone();
    let p = p1.clone() + p4.clone();
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Composite{
            matched: Some(&x[0..10]),
            components: vec!(
                (Goal{parser: &*p1, text: x},
                 Outcome::Leaf{
                     matched: &x[0..6],
                     then: None}),
                (Goal{parser: &*p4, text: &x[6..]},
                 Outcome::Composite{
                     matched: Some(&x[6..10]),
                     components: vec!( (Goal{ parser: &*p2, text: &x[6..]},
                                        Outcome::Leaf{
                                            matched: &x[6..10],
                                            then: None}) )}),
            )}});
    assert::equal(&r.get_ast(root),
                  &Ok(AST{ value:Item{ tag: root, text: &x[0..10] }, children: vec!()}));
    
    let x = "freddy is very good";
    let p1 = literal("freddy");
    let p2 = literal(" was");
    let p3 = literal(" is");
    let p4 = p2.clone() | p3.clone();
    let p = p1.clone() + p4.clone();
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Composite{
            matched: Some(&x[0..9]),
            components: vec!(
                (Goal{parser: &*p1, text: x},
                 Outcome::Leaf{
                     matched: &x[0..6],
                     then: None}),
                (Goal{parser: &*p4, text: &x[6..]},
                 Outcome::Composite{
                     matched: Some(&x[6..9]),
                     components: vec!(
                         (Goal{ parser: &*p2, text: &x[6..]},
                          Outcome::Leaf{
                              matched: &x[6..7],
                              then: Some(Unexpected::Char)}),
                         (Goal{ parser: &*p3, text: &x[6..]},
                          Outcome::Leaf{
                              matched: &x[6..9],
                              then: None}) )}),
            )}});
    assert::equal(&r.get_ast(root),
                  &Ok(AST{ value:Item{ tag: root, text: &x[0..9] }, children: vec!()}));
    
    let x = "freddy were not very good";
    let p1 = literal("freddy");
    let p2 = literal(" was");
    let p3 = literal(" is");
    let p4 = p2.clone() | p3.clone();
    let p = p1.clone() + p4.clone();
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Composite{
            matched: None,
            components: vec!(
                (Goal{parser: &*p1, text: x},
                 Outcome::Leaf{
                     matched: &x[0..6],
                     then: None}),
                (Goal{parser: &*p4, text: &x[6..]},
                 Outcome::Composite{
                     matched: None,
                     components: vec!(
                         (Goal{ parser: &*p2, text: &x[6..]},
                          Outcome::Leaf{
                              matched: &x[6..8],
                              then: Some(Unexpected::Char)}),
                         (Goal{ parser: &*p3, text: &x[6..]},
                          Outcome::Leaf{
                              matched: &x[6..7],
                              then: Some(Unexpected::Char)}) )}),
            )}});
    assert::equal(&r.get_ast(root),
                  &Err(ParseFailed{
                      at: all_of(x).after(&x[0..8]),
                      why: Unexpected::Char,
                      context: vec!(Context{tag: root, text: &x})
                  }));
    
    // tagged
    let x = "freddy was very good";
    let p1 = literal("freddy");
    let tag1 = "tag1";
    let p = tagged(tag1, p1.clone());
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Composite{
            matched: Some(&x[0..6]),
            components: vec!(
                (Goal{ parser: &*p1, text: &x},
                 Outcome::Leaf{
                     matched: &x[0..6],
                     then: None
                 }),)}});
    assert::equal(&r.get_ast(root),
                  &Ok(AST{ value:Item{ tag: root, text: &x[0..6] },
                           children: vec!(
                               AST{ value:Item{ tag: tag1, text: &x[0..6] }, children: vec!()})}));

    
    let x = "freddy was very good";
    let p0 = literal("freddy");
    let tag0 = "tag0";
    let p1 = tagged(tag0, p0.clone());
    let p2 = literal(" was");
    let p3 = p1.clone() + p2.clone();
    let tag3 = "tag3";
    let p = tagged(tag3, p3.clone());
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Composite{
            matched: Some(&x[0..10]),
            components: vec!(
                (Goal{ parser: &*p3, text: &x},
                 Outcome::Composite{
                     matched: Some(&x[0..10]),
                     components: vec!(
                         (Goal{parser: &*p1, text: x},
                          Outcome::Composite{
                              matched: Some(&x[0..6]),
                              components: vec!(
                                  (Goal{parser: &*p0, text: x},
                                   Outcome::Leaf{
                              matched: &x[0..6],
                                       then: None}))}),
                         (Goal{ parser: &*p2, text: &x[6..]},
                          Outcome::Leaf{
                              matched: &x[6..10],
                              then: None}),
                     )}),)}});
    assert::equal(&r.get_ast(root),
                  &Ok(AST{ value:Item{ tag: root, text: &x[0..10] },
                           children: vec!(
                               AST{ value:Item{ tag: tag3, text: &x[0..10] },
                                    children: vec!(AST{ value:Item{ tag: tag0, text: &x[0..6] },
                                                        children: vec!()})})}));

    let x = "freddy was very good";
    let p1 = literal("jenny");
    let p = tagged(tag1, p1.clone());
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Composite{
            matched: None,
            components: vec!(
                (Goal{ parser: &*p1, text: &x},
                 Outcome::Leaf{
                     matched: &x[0..0],
                     then: Some(Unexpected::Char)
                 }),)}});
    assert::equal(&r.get_ast(root),
                  &Err(ParseFailed{
                      at: all_of(x).after(&x[0..0]),
                      why: Unexpected::Char,
                      context: vec!(Context{tag: tag1, text: &x},
                                    Context{tag: root, text: &x})
                  }));
    
    let x = "freddy was very good";
    let p0 = literal("freddy");
    let p1 = tagged(tag0, p0.clone());
    let p2 = literal(" wasn't");
    let p3 = p1.clone() + p2.clone();
    let p = tagged(tag3, p3.clone());
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Composite{
            matched: None,
            components: vec!(
                (Goal{ parser: &*p3, text: &x},
                 Outcome::Composite{
                     matched: None,
                     components: vec!(
                         (Goal{parser: &*p1, text: x},
                          Outcome::Composite{
                              matched: Some(&x[0..6]),
                              components: vec!(
                                  (Goal{parser: &*p0, text: x},
                                   Outcome::Leaf{
                                       matched: &x[0..6],
                                       then: None}) )}),
                         (Goal{ parser: &*p2, text: &x[6..]},
                          Outcome::Leaf{
                              matched: &x[6..10],
                              then: Some(Unexpected::Char)}),
                     )}),)}});
    assert::equal(&r.get_ast(root),
                  &Err(ParseFailed{
                      at: all_of(x).after(&x[0..10]),
                      why: Unexpected::Char,
                      context: vec!(Context{tag: tag3, text: &x},
                                    Context{tag: root, text: &x})
                  }));

    let x = "freddy was very good";
    let p0 = literal("freddy");
    let p1 = tagged(tag0, p0.clone());
    let p2 = literal(" wasn't");
    let p3 = tagged(tag3, p2.clone());
    let p4 = p1.clone() + p3.clone();
    let tag4 = "tag4";
    let p = tagged(tag4, p4.clone());
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Composite{
            matched: None,
            components: vec!(
                (Goal{ parser: &*p4, text: &x},
                 Outcome::Composite{
                     matched: None,
                     components: vec!(
                         (Goal{parser: &*p1, text: x},
                          Outcome::Composite{
                              matched: Some(&x[0..6]),
                              components: vec!(
                                  (Goal{parser: &*p0, text: x},
                                   Outcome::Leaf{
                                       matched: &x[0..6],
                                       then: None}) )}),
                         (Goal{parser: &*p3, text: &x[6..]},
                          Outcome::Composite{
                              matched: None,
                              components: vec!(
                                  (Goal{ parser: &*p2, text: &x[6..]},
                                   Outcome::Leaf{
                                       matched: &x[6..10],
                                       then: Some(Unexpected::Char)}))}),
                     )}),)}});
    assert::equal(&r.get_ast(root),
                  &Err(ParseFailed{
                      at: all_of(x).after(&x[0..10]),
                      why: Unexpected::Char,
                      context: vec!(Context{tag: tag3, text: &x[6..]},
                                    Context{tag: tag4, text: &x},
                                    Context{tag: root, text: &x})
                  }));

    // any_char
    let x = "fr";
    let p = any_char();
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..1],
            then: None
        }});
    
    let x = "\n";
    let p = any_char();
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..1],
            then: None
        }});
    
    let x = "";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..0],
            then: Some(Unexpected::EndOfInput)
        }});
    
    // digit
    let x = "0123456789";
    let p = digit();
    for (i, _c) in x.chars().enumerate() {
        let test_str = &x[i..];
        let r = p.parse(test_str);
        assert::equal(&r, &Parsed {
            goal: Goal{ parser: &*p, text: &test_str },
            outcome: Outcome::Leaf{
                matched: &test_str[0..1],
                then: None
            }});
    }
    
    let x = "A34";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..0],
            then: Some(Unexpected::Char)
        }});
    
    let x = "";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..0],
            then: Some(Unexpected::EndOfInput)
        }});


    // octal_digit
    let x = "01234567";
    let p = octal_digit();
    for (i, _c) in x.chars().enumerate() {
        let test_str = &x[i..];
        let r = p.parse(test_str);
        assert::equal(&r, &Parsed {
            goal: Goal{ parser: &*p, text: &test_str },
            outcome: Outcome::Leaf{
                matched: &test_str[0..1],
                then: None
            }});
    }
    
    let x = "8c";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..0],
            then: Some(Unexpected::Char)
        }});
    
    let x = "";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..0],
            then: Some(Unexpected::EndOfInput)
        }});


    // hex_digit
    let x = "0123456789abcdefABCDEF";
    let p = hex_digit();
    for (i, _c) in x.chars().enumerate() {
        let test_str = &x[i..];
        let r = p.parse(test_str);
        assert::equal(&r, &Parsed {
            goal: Goal{ parser: &*p, text: &test_str },
            outcome: Outcome::Leaf{
                matched: &test_str[0..1],
                then: None
            }});
    }
    
    let x = "g4";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..0],
            then: Some(Unexpected::Char)
        }});
    
    let x = "";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..0],
            then: Some(Unexpected::EndOfInput)
        }});


    // at_least_one
    let x = "1234a";
    let p1 = digit();
    let p = at_least_one(p1.clone());
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Composite{
            matched: Some(&x[0..4]),
            components: vec!(
                (Goal{parser: &*p1, text: x},
                 Outcome::Leaf{
                     matched: &x[0..1],
                     then: None}),
                (Goal{parser: &*p1, text: &x[1..]},
                 Outcome::Leaf{
                     matched: &x[1..2],
                     then: None}),
                (Goal{parser: &*p1, text: &x[2..]},
                 Outcome::Leaf{
                     matched: &x[2..3],
                     then: None}),
                (Goal{parser: &*p1, text: &x[3..]},
                 Outcome::Leaf{
                     matched: &x[3..4],
                     then: None}),
            )}});
    
    let x = "a";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Composite{
            matched: None,
            components: vec!(
                (Goal{parser: &*p1, text: x},
                 Outcome::Leaf{
                     matched: &x[0..0],
                     then: Some(Unexpected::Char)}),
            )}});
    
    let x = "";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Composite{
            matched: None,
            components: vec!(
                (Goal{parser: &*p1, text: x},
                 Outcome::Leaf{
                     matched: &x[0..0],
                     then: Some(Unexpected::EndOfInput)}),
            )}});
    
    // zero_or_more
    let x = "1234a";
    let p1 = digit();
    let p = zero_or_more(p1.clone());
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Composite{
            matched: Some(&x[0..4]),
            components: vec!(
                (Goal{parser: &*p1, text: x},
                 Outcome::Leaf{
                     matched: &x[0..1],
                     then: None}),
                (Goal{parser: &*p1, text: &x[1..]},
                 Outcome::Leaf{
                     matched: &x[1..2],
                     then: None}),
                (Goal{parser: &*p1, text: &x[2..]},
                 Outcome::Leaf{
                     matched: &x[2..3],
                     then: None}),
                (Goal{parser: &*p1, text: &x[3..]},
                 Outcome::Leaf{
                     matched: &x[3..4],
                     then: None}),
            )}});
    
    let x = "a";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Composite{
            matched: Some(&x[0..0]),
            components: vec!(
            )}});
    
    let x = "";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Composite{
            matched: Some(&x[0..0]),
            components: vec!(
            )}});

    // one_of_chars
    let x = "axy";
    let p = one_of_chars(CharSet{ value: "a-z0-9_-" });
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..1],
            then: None
        }});
    
    let x = "!";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..0],
            then: Some(Unexpected::Char)
        }});
    
    let p = one_of_chars(CharSet{ value: "f-a" });
    for c in "af-".chars() {
        let a = format!("{c}x", c=c);
        let x = a.as_str();
        let r = p.parse(x);
        assert::equal(&r, &Parsed {
            goal: Goal{ parser: &*p, text: &x },
            outcome: Outcome::Leaf{
                matched: &x[0..1],
                then: None
            }});
    }
    let x = "b";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..0],
            then: Some(Unexpected::Char)
        }});
    
    let p = one_of_chars(CharSet{ value: "-af" });
    for c in "af-".chars() {
        let a = format!("{c}x", c=c);
        let x = a.as_str();
        let r = p.parse(x);
        assert::equal(&r, &Parsed {
            goal: Goal{ parser: &*p, text: &x },
            outcome: Outcome::Leaf{
                matched: &x[0..1],
                then: None
            }});
    }
    let x = "b";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..0],
            then: Some(Unexpected::Char)
        }});
    
    let p = one_of_chars(CharSet{ value: "af-" });
    for c in "af-".chars() {
        let a = format!("{c}x", c=c);
        let x = a.as_str();
        let r = p.parse(x);
        assert::equal(&r, &Parsed {
            goal: Goal{ parser: &*p, text: &x },
            outcome: Outcome::Leaf{
                matched: &x[0..1],
                then: None
            }});
    }
    let x = "b";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..0],
            then: Some(Unexpected::Char)
        }});


    // any_char_except
    let x = "!xy";
    let p = any_char_except(crate::CharSet{ value: "a-z0-9_-" });
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..1],
            then: None
        }});
    
    let x = "a";
    let r = p.parse(x);
    assert::equal(&r, &Parsed {
        goal: Goal{ parser: &*p, text: &x },
        outcome: Outcome::Leaf{
            matched: &x[0..0],
            then: Some(Unexpected::Char)
        }});
    
    let p = any_char_except(crate::CharSet{ value: "f-a" });
    for c in "bce".chars() {
        let a = format!("{c}x", c=c);
        let x = a.as_str();
        let r = p.parse(x);
        assert::equal(&r, &Parsed {
            goal: Goal{ parser: &*p, text: &x },
            outcome: Outcome::Leaf{
                matched: &x[0..1],
                then: None
            }});
    }
    for c in "af-".chars() {
        let a = format!("{c}x", c=c);
        let x = a.as_str();
        let r = p.parse(x);
        assert::equal(&r, &Parsed {
            goal: Goal{ parser: &*p, text: &x },
            outcome: Outcome::Leaf{
                matched: &x[0..0],
                then: Some(Unexpected::Char)
            }});
    }
    
    let p = any_char_except(crate::CharSet{ value: "-af" });
    for c in "bce".chars() {
        let a = format!("{c}x", c=c);
        let x = a.as_str();
        let r = p.parse(x);
        assert::equal(&r, &Parsed {
            goal: Goal{ parser: &*p, text: &x },
            outcome: Outcome::Leaf{
                matched: &x[0..1],
                then: None
            }});
    }
    for c in "af-".chars() {
        let a = format!("{c}x", c=c);
        let x = a.as_str();
        let r = p.parse(x);
        assert::equal(&r, &Parsed {
            goal: Goal{ parser: &*p, text: &x },
            outcome: Outcome::Leaf{
                matched: &x[0..0],
                then: Some(Unexpected::Char)
            }});
    }
    
    let p = any_char_except(crate::CharSet{ value: "af-" });
    for c in "bce".chars() {
        let a = format!("{c}x", c=c);
        let x = a.as_str();
        let r = p.parse(x);
        assert::equal(&r, &Parsed {
            goal: Goal{ parser: &*p, text: &x },
            outcome: Outcome::Leaf{
                matched: &x[0..1],
                then: None
            }});
    }
    for c in "af-".chars() {
        let a = format!("{c}x", c=c);
        let x = a.as_str();
        let r = p.parse(x);
        assert::equal(&r, &Parsed {
            goal: Goal{ parser: &*p, text: &x },
            outcome: Outcome::Leaf{
                matched: &x[0..0],
                then: Some(Unexpected::Char)
            }});
    }

    // cr / lf / crlf
    let x = "\r\n\r\n";
    let p = cr() + lf() + crlf();
    let r = p.parse(x);
    assert::equal(&r.get_ast(root),
                  &Ok(AST{ value:Item{ tag: root, text: &x[0..4] },
                           children: vec!(
                               AST{ value:Item{ tag: CR, text: &x[0..1] }, children: vec!() },
                               AST{ value:Item{ tag: LF, text: &x[1..2] }, children: vec!() },
                               AST{ value:Item{ tag: CRLF, text: &x[2..4] }, children: vec!() },
                           )}));

    // trailing_space, some_inline_space, any_inline_space, rest_of_line_blank,
    // some_space, any_space, eat_white
    let x = r###"
{
  "a": 1,
  8: a 
}
"###;
    let a = "a";
    let p = trailing_space()+
        any_inline_space()+char('{')+any_space()+
        tagged(a,literal("\"a\""))+eat_white()+char(':')+some_space()+char('1')+any_space()+
        char(',')+rest_of_line_blank()+
        some_inline_space()+literal("8:")+any_inline_space()+char('a')+eat_white()+char('}')+trailing_space()+
        end_of_input();
    let r = p.parse(x);
    assert::equal(
        &r.get_ast(root),
        &Ok(
            AST{ value:Item{ tag: root, text: x },
                 children: vec!(
                     AST{ value:Item{ tag: TRAILING_SPACE, text: &x[0..1] }, children: vec!(
                         AST{ value:Item{ tag: REST_OF_LINE_BLANK, text: &x[0..1] }, children: vec!(
                             AST{ value:Item{ tag: INLINE_SPACE, text: &x[0..0] }, children: vec!()},
                             AST{ value:Item{ tag: LF, text: &x[0..1] }, children: vec!()},
                         )}
                     )},
                     AST{ value:Item{ tag: INLINE_SPACE, text: &x[1..1] }, children: vec!()},
                     AST{ value:Item{ tag: WHITESPACE, text: &x[2..5] }, children: vec!()},
                     AST{ value:Item{ tag: a, text: &x[5..8] }, children: vec!()}, // "a"
                     AST{ value:Item{ tag: WHITESPACE, text: &x[9..10] }, children: vec!()},
                     // 1
                     AST{ value:Item{ tag: WHITESPACE, text: &x[11..11] }, children: vec!()},
                     // ,
                     AST{ value:Item{ tag: REST_OF_LINE_BLANK, text: &x[12..13] }, children: vec!(
                             AST{ value:Item{ tag: INLINE_SPACE, text: &x[12..12] }, children: vec!()},
                             AST{ value:Item{ tag: LF, text: &x[12..13] }, children: vec!()},
                     )},
                     AST{ value:Item{ tag: INLINE_SPACE, text: &x[13..15] }, children: vec!()},
                     // 8:  [16..18]
                     AST{ value:Item{ tag: INLINE_SPACE, text: &x[19..20] }, children: vec!()},
                     // a + eat white + }
                     AST{ value:Item{ tag: TRAILING_SPACE, text: &x[22..23] }, children: vec!(
                         AST{ value:Item{ tag: REST_OF_LINE_BLANK, text: &x[22..23] }, children: vec!(
                             AST{ value:Item{ tag: INLINE_SPACE, text: &x[22..22] }, children: vec!()},
                             AST{ value:Item{ tag: LF, text: &x[22..23] }, children: vec!()},
                         )}
                     )})}));

    let x = "y";
    let p1 = char('x');
    let p2 = char('y');
    let p = parse_x_until_y(p1, p2);
    let r = p.parse(x);
    assert::equal(&r.get_ast(root), 
        &Ok(
            AST{ value:Item{ tag: root, text: &x[0..0] },
                 children: vec!()
            }));

    let x = "xxxy";
    let r = p.parse(x);
    assert::equal(&r.get_ast(root), 
        &Ok(
            AST{ value:Item{ tag: root, text: &x[0..3] },
                 children: vec!()
            }));

    let x = "xby";
    let r = p.parse(x);
    assert::equal(&r.get_ast(root), 
                  &Err(
                      ParseFailed{
                          at: all_of(x).after(&x[0..1]),
                          why: Unexpected::Char,
                          context: vec!(Context{tag: root, text: &x})
                      }));

    // us_ascii_printable
    let x = " g~";
    let p = parse_x_until_y(us_ascii_printable(), end_of_input());
    let r = p.parse(x);
    assert::equal(&r.get_ast(root), 
        &Ok(
            AST{ value:Item{ tag: root, text: x },
                 children: vec!()
            }));

    // list_of
    let x = "{ a, b, c }";
    let p = list_of(char('{')+eat_white(),
                    one_of_chars(CharSet{ value: "abc" })+eat_white(),
                    char(',')+eat_white(),
                    char('}')+eat_white());
    let r = p.parse(x);
    assert::equal(&r.get_ast(root), 
        &Ok(
            AST{ value:Item{ tag: root, text: x },
                 children: vec!()
            }));
    let x = "{ a, b,, }";
    let r = p.parse(x);
    assert::equal(&r.get_ast(root), 
                  &Err(
                      ParseFailed{
                          at: all_of(x).after(&x[0..7]),
                          why: Unexpected::Char,
                          context: vec!(Context{tag: root, text: &x})
                      }));

    // parse_balanced_until_y
    let p = parse_balanced_until_y(
        [
            (char('['), char(']')),
            (char('('), char(')')),
        ].to_vec(),
        digit() | char('+'),
        char(']')
    );
    let x = "123]"; // no nesting
    let r = p.parse(x);
    assert::equal(&r.get_ast(root), 
        &Ok(
            AST{ value:Item{ tag: root, text: &x[..x.len()-1] },
                 children: vec!()
            }));
    let x = "1[23]7([8]9)0]"; // nesting
    let r = p.parse(x);
    assert::equal(&r.get_ast(root), 
        &Ok(
            AST{ value:Item{ tag: root, text: &x[..x.len()-1] },
                 children: vec!()
            }));
    let x = "123)]"; // unbalanced )
    let r = p.parse(x);
    assert::equal(&r.get_ast(root), 
                  &Err(
                      ParseFailed{
                          at: all_of(x).after(&x[0..3]),
                          why: Unexpected::Char,
                          context: vec!(Context{tag: root, text: &x})
                      }));
    let x = "1[23)]a]"; // unbalanced )
    let r = p.parse(x);
    assert::equal(&r.get_ast(root), 
                  &Err(
                      ParseFailed{
                          at: all_of(x).after(&x[0..4]),
                          why: Unexpected::Char,
                          context: vec!(Context{tag: root, text: &x})
                      }));
    let x = "123x"; // missing close
    let r = p.parse(x);
    assert::equal(&r.get_ast(root), 
                  &Err(
                      ParseFailed{
                          at: all_of(x).after(&x[0..3]),
                          why: Unexpected::Char,
                          context: vec!(Context{tag: root, text: &x})
                      }));
    let x = "1(23"; // missing nested close
    let r = p.parse(x);
    assert::equal(&r.get_ast(root), 
                  &Err(
                      ParseFailed{
                          at: all_of(x).after(&x[0..4]),
                          why: Unexpected::EndOfInput,
                          context: vec!(Context{tag: root, text: &x})
                      }));
    let x = "1(23]"; // missing nested close
    let r = p.parse(x);
    assert::equal(&r.get_ast(root), 
                  &Err(
                      ParseFailed{
                          at: all_of(x).after(&x[0..4]),
                          why: Unexpected::Char,
                          context: vec!(Context{tag: root, text: &x})
                      }));

    // backref (recursion)
    let b1id = "b1";
    let b2id = "b2";

    let b1 = digit() | (char('(') + backref(&b2id) + char(')'));
    let b2 = digit() | (char('[') + (backref(&b1id) | backref(&b2id)) + char(']'));
    let p  = backrefs([(b1id, b1.clone()),(b2id, b2.clone())].to_vec(), b1 | b2);
    let x = "1";
    let r = p.parse(x);
    assert::equal(&r.get_ast(root), 
        &Ok(
            AST{ value:Item{ tag: root, text: x },
                 children: vec!()
            }));
    let x = "(1)";
    let r = p.parse(x);
    assert::equal(&r.get_ast(root), 
        &Ok(
            AST{ value:Item{ tag: root, text: x },
                 children: vec!()
            }));
    let x = "([(1)])";
    let r = p.parse(x);
    assert::equal(&r.get_ast(root), 
        &Ok(
            AST{ value:Item{ tag: root, text: x },
                 children: vec!()
            }));
    let x = "[[([1])]]";
    let r = p.parse(x);
    assert::equal(&r.get_ast(root), 
        &Ok(
            AST{ value:Item{ tag: root, text: x },
                 children: vec!()
            }));

    // switch
    // where there is a constraint that says
    //   if it starts with x then it must x y and nothing else
    let owl = "owl";
    let fox_cub = "fox cub";
    let bird = "bird";
    let p = switch(
        (literal("an"), tagged(owl, literal(" owl"))),
        [(literal("a"), tagged(fox_cub, literal(" fox cub"))),
         (literal("the"), tagged(bird, literal(" bird")))].to_vec());
    let x = "an owl";
    let r = p.parse(x);
    assert::equal(
        &r.get_ast(root),
        &Ok(
            AST{ value:Item{ tag: root, text: x },
                 children: vec!(
                     AST{ value:Item{ tag: owl, text: &x[2..6] }, children: vec!()})}));
    let x = "a fox cub";
    let r = p.parse(x);
    assert::equal(
        &r.get_ast(root),
        &Ok(
            AST{ value:Item{ tag: root, text: x },
                 children: vec!(
                     AST{ value:Item{ tag: fox_cub, text: &x[1..9] }, children: vec!()})}));
    let x = "the bird";
    let r = p.parse(x);
    assert::equal(
        &r.get_ast(root),
        &Ok(
            AST{ value:Item{ tag: root, text: x },
                 children: vec!(
                     AST{ value:Item{ tag: bird, text: &x[3..8] }, children: vec!()})}));

    let x = "an fox cub";
    let r = p.parse(x);
    assert::equal(&r.get_ast(root), 
                  &Err(
                      ParseFailed{
                          at: all_of(x).after(&x[0..3]),
                          why: Unexpected::Char,
                          context: vec!(Context{tag: owl, text: &x[2..]},
                                        Context{tag: root, text: &x})
                      }));
    
}
