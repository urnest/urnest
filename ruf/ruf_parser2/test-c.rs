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
    Unexpected,
    ParseFailed,
    Context,
};
use ruf_parser2::c::{
    eat_white,
    string_literal,
    c_comment,
    cxx_comment,
    S_CHARS,
    identifier,
    IDENTIFIER,
};
use ruf_parser2::ast::Item;
use ruf_assert as assert;

fn main() {
    let root="root";

    // cxx_comment
    let p = cxx_comment();
    let x = "// comment \n   // continued\n    ";
    let r = p.parse(x);
    assert::equal(
        &r.get_ast(root),
        &Ok(
            AST{ value:Item{ tag: root, text: &x[0..28] },
                 children: vec!(
                     AST{ value:Item{ tag: "c++ comment", text: &x[0..28] }, children: vec!(
                         AST{ value:Item{ tag: "inline whitespace", text: &x[0..0] }, children: vec!()},
                         AST{ value:Item{ tag: "inline whitespace", text: &x[12..15] }, children: vec!()}
                     )})}));
    
    let x = "/ / comment \n   ";
    let r = p.parse(x);
    assert::equal(
        &r.get_ast(root),
        &Err(
            ParseFailed{
                at: &x[1..],
                why: Unexpected::Char,
                context: vec!(Context {tag: "c++ comment", text: "/ / comment \n   "},
                              Context{tag: root, text: &x})
            }));

    // c_comment
    let p = c_comment();
    let x = "/*\n  comment\n  /*continued\n*/    ";
    let r = p.parse(x);
    assert::equal(
        &r.get_ast(root),
        &Ok(
            AST{ value:Item{ tag: root, text: &x[0..29] },
                 children: vec!(
                     AST{ value:Item{ tag: "c comment", text: &x[0..29] }, children: vec!(
                         AST{ value:Item{ tag: "inline whitespace", text: &x[0..0] }, children: vec!()})}
                     )}));
    
    let x = "/* comment \n   ";
    let r = p.parse(x);
    assert::equal(
        &r.get_ast(root),
        &Err(
            ParseFailed{
                at: "",
                why: Unexpected::EndOfInput,
                context: vec!(Context {tag: "c comment", text: x},
                              Context{tag: root, text: x})
            }));

    // eat_white - does not fail
    let p = eat_white();
    let x = " // comment \n// ctd\n/* ... \n*/  abc";
    let r = p.parse(x);
    assert::equal(&r.get_ast(root).err(), &None);
    assert::equal(
        &r.get_ast(root).ok().unwrap().value,
        &Item{ tag: root, text: &x[0..32] });

    let x = "abc";
    let r = p.parse(x);
    assert::equal(&r.get_ast(root).err(), &None);
    assert::equal(
        &r.get_ast(root).ok().unwrap().value,
        &Item{ tag: root, text: &x[0..0] });

    // string_literal
    let p = string_literal();
    let x = "\"abcd\"";
    let r = p.parse(x);
    let a = r.get_ast(root);
    assert::equal(&a.as_ref().err(), &None);
    assert::equal(
        &a.as_ref().ok().unwrap().value,
        &Item{ tag: root, text: x });
    assert::equal(
        &a.ok().unwrap().select_by_value(&|v| v.tag == S_CHARS)
            .copy_selected_values().iter()
            .map(|v| v.text).collect(),
        &vec!("abcd"));
            
    let x = "\"\"";
    let r = p.parse(x);
    let a = r.get_ast(root);
    assert::equal(&a.as_ref().err(), &None);
    assert::equal(
        &a.as_ref().ok().unwrap().value,
        &Item{ tag: root, text: x });
    assert::equal(
        &a.ok().unwrap().select_by_value(&|v| v.tag == S_CHARS)
            .copy_selected_values().iter()
            .map(|v| v.text).collect(),
        &vec!(""));
            
    let x = "\"\\\"\"";
    let r = p.parse(x);
    let a = r.get_ast(root);
    assert::equal(&a.as_ref().err(), &None);
    assert::equal(
        &a.as_ref().ok().unwrap().value,
        &Item{ tag: root, text: x });
    assert::equal(
        &a.ok().unwrap().select_by_value(&|v| v.tag == S_CHARS)
            .copy_selected_values().iter()
            .map(|v| v.text).collect(),
        &vec!("\\\""));
            
    let x = "\"\\\"de\"";
    let r = p.parse(x);
    let a = r.get_ast(root);
    assert::equal(&a.as_ref().err(), &None);
    assert::equal(
        &a.as_ref().ok().unwrap().value,
        &Item{ tag: root, text: x });
    assert::equal(
        &a.ok().unwrap().select_by_value(&|v| v.tag == S_CHARS)
            .copy_selected_values().iter()
            .map(|v| v.text).collect(),
        &vec!("\\\"de"));
            
    let x = "\"ab\\\"de\"";
    let r = p.parse(x);
    let a = r.get_ast(root);
    assert::equal(&a.as_ref().err(), &None);
    assert::equal(
        &a.as_ref().ok().unwrap().value,
        &Item{ tag: root, text: x });
    assert::equal(
        &a.ok().unwrap().select_by_value(&|v| v.tag == S_CHARS)
            .copy_selected_values().iter()
            .map(|v| v.text).collect(),
        &vec!("ab\\\"de"));
            
    let x = "\"ab\\\"d\\ne\"";
    let r = p.parse(x);
    let a = r.get_ast(root);
    assert::equal(&a.as_ref().err(), &None);
    assert::equal(
        &a.as_ref().ok().unwrap().value,
        &Item{ tag: root, text: x });
    assert::equal(
        &a.ok().unwrap().select_by_value(&|v| v.tag == S_CHARS)
            .copy_selected_values().iter()
            .map(|v| v.text).collect(),
        &vec!("ab\\\"d\\ne"));
            
    let x = "\"ab\\\\d\\ne\"   ";
    let r = p.parse(x);
    let a = r.get_ast(root);
    assert::equal(&a.as_ref().err(), &None);
    assert::equal(
        &a.as_ref().ok().unwrap().value,
        &Item{ tag: root, text: x });
    assert::equal(
        &a.ok().unwrap().select_by_value(&|v| v.tag == S_CHARS)
            .copy_selected_values().iter()
            .map(|v| v.text).collect(),
        &vec!("ab\\\\d\\ne"));
            
    let x = "\"abcde\"  \n \"fred\"  ";
    let r = p.parse(x);
    let a = r.get_ast(root);
    assert::equal(&a.as_ref().err(), &None);
    assert::equal(
        &a.as_ref().ok().unwrap().value,
        &Item{ tag: root, text: x });
    assert::equal(
        &a.ok().unwrap().select_by_value(&|v| v.tag == S_CHARS)
            .copy_selected_values().iter()
            .map(|v| v.text).collect(),
        &vec!("abcde", "fred"));
            
    let x = "\"abcde\"  \n \"fred\"\n\"jock\"  ";
    let r = p.parse(x);
    let a = r.get_ast(root);
    assert::equal(&a.as_ref().err(), &None);
    assert::equal(
        &a.as_ref().ok().unwrap().value,
        &Item{ tag: root, text: x });
    assert::equal(
        &a.ok().unwrap().select_by_value(&|v| v.tag == S_CHARS)
            .copy_selected_values().iter()
            .map(|v| v.text).collect(),
        &vec!("abcde", "fred", "jock"));
            
    let x = "\"abcde\" \"fred\"";
    let r = p.parse(x);
    let a = r.get_ast(root);
    assert::equal(&a.as_ref().err(), &None);
    assert::equal(
        &a.as_ref().ok().unwrap().value,
        &Item{ tag: root, text: x });
    assert::equal(
        &a.ok().unwrap().select_by_value(&|v| v.tag == S_CHARS)
            .copy_selected_values().iter()
            .map(|v| v.text).collect(),
        &vec!("abcde", "fred"));
            
    let x = "\"abcde";
    let r = p.parse(x);
    let a = r.get_ast(root);
    assert::equal(&a.as_ref().ok(), &None);
    assert::equal(
        a.as_ref().err().unwrap(),
        &ParseFailed{
            at: "",
            why: Unexpected::EndOfInput,
            context: vec!(Context {tag: "string chars", text: "abcde"}, 
                          Context {tag: "string literal including quotes", text: "\"abcde"},
                          Context {tag: root, text: x})
        });
            
    let x = "\"abc\\999";
    let r = p.parse(x);
    let a = r.get_ast(root);
    assert::equal(&a.as_ref().ok(), &None);
    assert::equal(
        a.as_ref().err().unwrap(),
        &ParseFailed{
            at: "999",
            why: Unexpected::Char,
            context: vec!(Context {tag: "string chars", text: "abc\\999"}, 
                          Context {tag: "string literal including quotes", text: "\"abc\\999"},
                          Context {tag: root, text: x})
        });

    // identifier
    let x = "_Fred_34-";
    let p = identifier();
    let r = p.parse(x);
    let a = r.get_ast(root);
    assert::equal(&a.as_ref().err(), &None);
    assert::equal(
        &a.as_ref().ok().unwrap().value,
        &Item{ tag: root, text: &x[0..8] });
    assert::equal(
        &a.ok().unwrap().select_by_value(&|v| v.tag == IDENTIFIER)
            .copy_selected_values().iter()
            .map(|v| v.text).collect(),
        &vec!("_Fred_34"));
    
}
