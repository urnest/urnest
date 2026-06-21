// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//
// typescript (including javascript) language specific parsers
use std::sync::OnceLock;

use crate::{
    Ref,
    literal,
    any_char_except,
    tagged,
    not,
    looking_at,
    parse_x_until_y,
    CharSet,
    any_inline_space,
    any_space,
    switch,
    list_of,
    char,
    zero_or_more,
};

use crate::c::{
    double_slash,
    string_literal as c_string_literal,
    cxx_comment_line,
    c_comment,
    slash_star,
    identifier
};

// restrictions:
// - javascript imports are "hoisting" and in practice every sane
//   developer ensures they are up-front in the file, not mixed in
//   with other stuff, so can stop parsing imports as soon as
//   we come across something else
// - insist on double quoted string literals
//
// https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Statements/import
// https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Statements/export
pub fn ts_intro() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(
        || sp() +
            parse_x_until_y(
                (
                    switch(
                        (looking_at(include_namespace_intro()), include_namespace()),
                        [
                            (looking_at(double_slash()), cxx_comment_line()),
                            (looking_at(slash_star()), c_comment()),
                            (looking_at(kw_import()), import())
                        ].to_vec()) |
                    export_aggregate()
                ) + sp(),
                not(
                    looking_at(
                        include_namespace_intro() |
                        double_slash() |
                        slash_star() |
                        kw_import() |
                        export_aggregate())))
    ).clone()
}

pub fn include_namespace_intro() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(
        || triple_slash() + any_inline_space() + literal("<reference")+any_inline_space()+
            literal("path")+any_inline_space()+equals()).clone()
}

pub const INCLUDE_NAMESPACE: &str = "include typescript namespace";
pub fn include_namespace() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(
        || include_namespace_intro()+isp()+
            tagged(INCLUDE_NAMESPACE, c_string_literal())+isp()+greater()+
            zero_or_more(any_char_except(CharSet{value: "\n"}))+
            sp()
    ).clone()
}

pub const IMPORT: &str = "import";
pub fn import() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(|| tagged(
        IMPORT,
        kw_import() + sp() + (
            ((
                    // can combine default import with either named import or namespace import
                    (name() + sp() + comma() + sp() + (named_import() | namespace_import())) |
                    named_import() |
                    namespace_import() |
                    name() // default import
            ) + sp() + from_module()) |
            (
                module_name() // side-effect import
            )
        ) + sp() + semicolon() + isp())
    ).clone()
}

pub fn named_import() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(
        || list_of(
            open_brace() + sp(),
            (
                (name() + sp() + kw_as() + sp() + name()) |
                (c_string_literal() + sp() + kw_as() + sp() + name()) |
                name()
            ) + sp(),
            comma()+sp(),
            close_brace()
        ) + sp()
    ).clone()
}

pub fn from_module() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(
        || kw_from() + sp() + module_name() + sp()
    ).clone()
}

pub const MODULE_NAME: &str = "module name";
pub fn module_name() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(
        || tagged(MODULE_NAME, c_string_literal())
    ).clone()
}

pub fn namespace_import() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(
        || star() + sp() + kw_as() + sp() + name() + sp()
    ).clone()
}

pub const EXPORT_AGGREGATE: &str = "export aggregate";
pub fn export_aggregate() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(|| tagged(
        EXPORT_AGGREGATE,
        kw_export() + sp() + (
            named_import() |
            namespace_import() |
            star()
        ) + sp() + from_module() +
            sp() + semicolon()+ isp())
    ).clone()
}

                       
pub fn kw_import() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(|| keyword("import")).clone()
}
pub fn kw_export() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(|| keyword("export")).clone()
}
pub fn kw_from() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(|| keyword("from")).clone()
}
pub fn kw_as() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(|| keyword("as")).clone()
}
pub fn keyword(x: &'static str) -> Ref<'static> {
    literal(x) + looking_at(any_char_except(CharSet{value: "a-zA-Z_0-9"}))
}
pub fn sp() -> Ref<'static> {
    any_space()
}
pub fn isp() -> Ref<'static> {
    any_inline_space()
}
pub fn triple_slash() ->Ref<'static> {
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(|| literal("///")).clone()
}
pub fn comma() ->Ref<'static> {
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(|| char(',')).clone()
}
pub fn star() ->Ref<'static> {
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(|| char('*')).clone()
}
pub fn equals() ->Ref<'static> {
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(|| char('=')).clone()
}
pub fn semicolon() ->Ref<'static> {
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(|| char(';')).clone()
}
pub fn open_brace() ->Ref<'static> {
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(|| char('{')).clone()
}
pub fn close_brace() ->Ref<'static> {
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(|| char('}')).clone()
}
pub fn greater() ->Ref<'static> {
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(|| char('>')).clone()
}
pub fn name() -> Ref<'static> {
    identifier()
}

/*
was going to translate
https://github.com/antlr/grammars-v4/blob/master/javascript/javascript/JavaScriptParser.g4
... but I think that grammar has some problems?
*/
