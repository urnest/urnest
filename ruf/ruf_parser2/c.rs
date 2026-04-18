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
};

pub const STRING_ESCAPE_SEQUENCE: &str = "string escape sequence";

// tagged STRING_ESCAPE_SEQUENCE
pub fn string_escape_sequence() -> Ref<'static>
{
    static result: OnceLock<Ref<'static>> = OnceLock::new();
    result.get_or_init(
        || literal("\\")+(
            one_of_chars(CharSet{ value: "'\"?\\abfnrtvuU"}) |
            (octal_digit()+octal_digit()+octal_digit()) |
            (octal_digit()+octal_digit()) |
            octal_digit() |
            (literal("x")+at_least_one(hex_digit()))))
}

pub fn s_char() -> Ref<'static>
{
    static result: OnceLock<Ref<'static>> = OnceLock::new();
    result.get_or_init(
        || any_char_except(CharSet{ value: "\\\"\n"}) |
        string_escape_sequence())
}

pub const S_CHARS: &str = "string escape sequence";

// sequence of string chars i.e. content of c-string literal
// tagged S_CHARS
pub fn s_chars() -> Ref<'static>
{
    static result: OnceLock<Ref<'static>> = OnceLock::new();
    result.get_or_init(
        || tagged(S_CHARS, parse_x_until_y(s_char(), double_quote())))
}

// matches nothing or something
pub eat_white() -> Ref<'static>
{
    static result: OnceLock<Ref<'static>> = OnceLock::new();
    result.get_or_init(
        REVISIT match!!!
         !(whitespaceChar()|doubleSlash()|slashStar())|
         named<hcp_ast::WhiteSpace>(
              "whitespace",
              zeroOrMore()*(whitespaceChar()|comments()))));
  return eatWhite;
}

pub const STRING_LITERAL: &str = "string literal including quotes";

// string literal plus untagged trailing whitespace (tagged STRING_LITERAL)
// note includes white-space concatenated strings like:
//   "abc" "def"
// ... including across lines, and whitespace includes comments
pub fn string_literal() -> Ref<'static>
{
    static result: OnceLock<Ref<'static>> = OnceLock::new();
    result.get_or_init(
        || tagged(STRING_LITERAL,
                  at_least_one(double_quote()+
                               s_chars()+
                               double_quote()+eat_white())a));
  return stringLiteral;
}
