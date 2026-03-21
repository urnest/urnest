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
    at_least_one,
    zero_or_more,
    CharSet,
    one_of_chars,
};
use ruf_parser2::ast::Item;
use ruf_parser2::all_of::all_of;
use ruf_assert as assert;

/*
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
    let left_handed = "lefty";
    let right_handedness = "righty";

    let left = "left";
    let right = "right";
    let parse_left = tagged(left, literal(left));
    let parse_right = tagged(right, literal(right));
    let ws = whitespace();
    let parser =
        tagged(full_name, // nesting makes it easier to search unambiguously
               tagged(first_name, parse_until(ws)) + ws +
               tagged(last_name, parse_until(ws))) + ws +
        tagged(eye_colour, parse_until(ws)) + ws +
        literal("eyes and is") + ws +
        tagged(handedness, parse_left | parse_right) +
        literal(" handed and ") + ws +
        (parse_left | parse_right) + ws +
        literal("footed.")+end_of_input();

    // parsing into an tagged tree is two stages:
    // - parse, which produces a tree of all parser outcomes, including failures
    // - extracting tagged tree, which has one node for each successful tagged()
    //   ... or has summary of the "best" match in case parsing failed.
    // if we're only interested in the final tree, combine the two stages:
    let ast = parser.parse(some_text).get_ast();

    // REVISIT: then can pull the relevant info out of the tree:
    
}
 */

fn main() {
    //turorial();

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
}
