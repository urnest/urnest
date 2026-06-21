// Copyright (c) 2026 Trevor Taylor
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

use ruf_parser2::typescript::{
    from_module,
    MODULE_NAME,
    include_namespace,
    INCLUDE_NAMESPACE,
    named_import,
    namespace_import,
    import,
    export_aggregate,
    ts_intro,
};

use ruf_parser2::c::{
    S_CHARS,
};

use ruf_parser2::ast::Item;
use ruf_assert as assert;

fn main() {
    let root="root";

    // from_module
    let p = from_module();
    let x = "from \"a/b\"";
    let r = p.parse(x);
    let a = r.get_ast(root);
    assert::equal(&a.as_ref().err(), &None);
    assert::equal(
        &a.as_ref().ok().unwrap().value,
        &Item{ tag: root, text: x });
    assert::equal(
        &a.ok().unwrap().select_by_value(&|v| v.tag == MODULE_NAME)
            .refine_by_value(&|v| v.tag == S_CHARS)
            .copy_selected_values().iter()
            .map(|v| v.text).collect(),
        &vec!("a/b"));

    // include_namespace
    let p = include_namespace();
    let x = "/// <reference path=\"a/b\">";
    let r = p.parse(x);
    let a = r.get_ast(root);
    assert::equal(&a.as_ref().err(), &None);
    assert::equal(
        &a.as_ref().ok().unwrap().value,
        &Item{ tag: root, text: x });
    assert::equal(
        &a.ok().unwrap().select_by_value(&|v| v.tag == INCLUDE_NAMESPACE)
            .refine_by_value(&|v| v.tag == S_CHARS)
            .copy_selected_values().iter()
            .map(|v| v.text).collect(),
        &vec!("a/b"));
    
    // named_import
    let p = named_import();
    let x = "{ a, c as d, \"e-f\" as g } ";
    let r = p.parse(x);
    let a = r.get_ast(root);
    assert::equal(&a.as_ref().err(), &None);
    assert::equal(
        &a.as_ref().ok().unwrap().value,
        &Item{ tag: root, text: x });
    
    // namespace_import
    let p = namespace_import();
    let x = "* as x ";
    let r = p.parse(x);
    let a = r.get_ast(root);
    assert::equal(&a.as_ref().err(), &None);
    assert::equal(
        &a.as_ref().ok().unwrap().value,
        &Item{ tag: root, text: x });
    
    // import
    let p = import();
    for x in [
        "import dflt, { a as b } from \"c/d\" ; ",
        "import dflt, * as d from \"c/d\" ; ",
        "import { a as b } from \"c/d\" ; ",
        "import * as d from \"c/d\" ; ",
        "import \"c/d\" ; ",
    ] {
        let r = p.parse(x);
        let a = r.get_ast(root);
        assert::equal(&a.as_ref().err(), &None);
        assert::equal(
            &a.as_ref().ok().unwrap().value,
            &Item{ tag: root, text: x });
        assert::equal(
            &a.ok().unwrap().select_by_value(&|v| v.tag == MODULE_NAME)
                .refine_by_value(&|v| v.tag == S_CHARS)
                .copy_selected_values().iter()
                .map(|v| v.text).collect(),
            &vec!("c/d"));
    }    
    
    // export_aggregate
    let p = export_aggregate();
    for x in [
        "export { a as b } from \"c/d\" ; ",
        "export * as d from \"c/d\" ; ",
        "export * from \"c/d\" ; ",
    ] {
        let r = p.parse(x);
        let a = r.get_ast(root);
        assert::equal(&a.as_ref().err(), &None);
        assert::equal(
            &a.as_ref().ok().unwrap().value,
            &Item{ tag: root, text: x });
        assert::equal(
            &a.ok().unwrap().select_by_value(&|v| v.tag == MODULE_NAME)
                .refine_by_value(&|v| v.tag == S_CHARS)
                .copy_selected_values().iter()
                .map(|v| v.text).collect(),
            &vec!("c/d"));
    }

    
    // ts_intro
    let p = ts_intro();
    let x = r###"
// xxxx
//
/// <reference path="a">
/// <reference path="b">
///
import dflt, { a as b } from "c" ;
// yyyyy
import dflt, * as d from "d" ;
import { a as b } from "e" ; /* zzzz */
import * as d from "f" ;
import "g" ;
export { a as b } from "h";
export * as d from "i";
export * from "j";
export y;
const y = 3;
"###;
    let r = p.parse(x);
    let a = r.get_ast(root);
    assert::equal(&a.as_ref().err(), &None);
    assert::equal(
        &a.as_ref().ok().unwrap().value,
        &Item{ tag: root, text: &x[0..290] });
    assert::equal(
        &a.as_ref().ok().unwrap().select_by_value(&|v| v.tag == MODULE_NAME)
            .refine_by_value(&|v| v.tag == S_CHARS)
            .copy_selected_values().iter()
            .map(|v| v.text).collect(),
        &vec!("c", "d", "e", "f", "g", "h", "i", "j"));
    assert::equal(
        &a.as_ref().ok().unwrap().select_by_value(&|v| v.tag == INCLUDE_NAMESPACE)
            .refine_by_value(&|v| v.tag == S_CHARS)
            .copy_selected_values().iter()
            .map(|v| v.text).collect(),
        &vec!("a", "b"));
}
