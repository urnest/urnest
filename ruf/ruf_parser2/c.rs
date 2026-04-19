// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
// c language specific parsers
use std::sync::OnceLock;

use crate::{
    Ref,
    literal,
    one_of_chars,
    octal_digit,
    at_least_one,
    any_char_except,
    tagged,
    double_quote,
    WHITESPACE,
    not,
    looking_at,
    parse_x_until_y,
    CharSet,
    hex_digit,
    some_space,
    any_inline_space,
    any_char,
};

pub const STRING_ESCAPE_SEQUENCE: &str = "string escape sequence";

// tagged STRING_ESCAPE_SEQUENCE
pub fn string_escape_sequence() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(
        || literal("\\")+(
            one_of_chars(CharSet{ value: "'\"?\\abfnrtvuU"}) |
            (octal_digit()+octal_digit()+octal_digit()) |
            (octal_digit()+octal_digit()) |
            octal_digit() |
            (literal("x")+at_least_one(hex_digit())))).clone()
}

pub fn s_char() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(
        || any_char_except(CharSet{ value: "\\\"\n"}) |
        string_escape_sequence()).clone()
}

pub const S_CHARS: &str = "string chars";

// sequence of string chars i.e. content of c-string literal
// tagged S_CHARS
pub fn s_chars() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(
        || tagged(S_CHARS, parse_x_until_y(s_char(), looking_at(double_quote())))).clone()
}

// matches nothing or something, tagged WHITESPACE
pub fn eat_white() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(
        || tagged(
            WHITESPACE,
            parse_x_until_y(
                some_space() | cxx_comment() | c_comment(),
                not(looking_at(one_of_chars(CharSet{ value: " \t\r\n"}) |
                               double_slash() |
                               slash_star()))))).clone()
}

pub const STRING_LITERAL: &str = "string literal including quotes";

// string literal plus untagged trailing whitespace (tagged STRING_LITERAL)
// note includes white-space concatenated strings like:
//   "abc" "def"
// ... including across lines, and whitespace includes comments
pub fn string_literal() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(
        || tagged(STRING_LITERAL,
                  at_least_one(double_quote()+
                               s_chars()+
                               double_quote()+eat_white()))).clone()
}

pub fn double_slash() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(
        || literal("//")).clone()
}

pub fn slash_star() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(
        || literal("/*")).clone()
}

pub fn star_slash() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(
        || literal("*/")).clone()
}

// c++ comment ( // ...), tagged CXX_COMMENT
pub const CXX_COMMENT: &str = "c++ comment";
pub fn cxx_comment() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(
        || tagged(
            CXX_COMMENT,
            at_least_one(
                any_inline_space() + double_slash() + parse_x_until_y(
                    any_char_except(CharSet{ value: "\n"}),
                    crate::char('\n'))))).clone()
}

// c comment ( /* ... */), tagged C_COMMENT
pub const C_COMMENT: &str = "c comment";
pub fn c_comment() -> Ref<'static>
{
    static RESULT: OnceLock<Ref<'static>> = OnceLock::new();
    RESULT.get_or_init(
        || tagged(
            C_COMMENT,
            any_inline_space() + slash_star() + parse_x_until_y(
                    any_char(),
                    star_slash()))).clone()
}
