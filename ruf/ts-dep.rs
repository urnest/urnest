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

use std::env;
use std::path;
use std::fs;
use std::process::ExitCode;
use ruf_parser2::ts::{ts_intro, MODULE_NAME, INCLUDE_NAMESPACE, S_CHARS};
use ruf_parser2::CompErrorFormat;

fn main() -> ExitCode {
    let i = env::args();
    const prog = i.next();
    match i.next() {
        Some(filename) => {
            const p = path::Path::new(filename).absolute();
            match fs::read_to_string(p) {
                Error(e) => {
                    eprintln!("failed to read {filename} because {e}");
                    ExitFailure
                }
                Ok(content) {
                    let p = ts_intro();
                    let r = p.parse(content);
                    match r.get_ast("parse imports and namespace references") {
                        Err(parse_failed) => {
                            const f = CompileErrorFormat{filename, parse_failed};
                            eprintln!("{f}");
                            ExitFailure
                        },
                        Ok(ast) => {
                            ast.select_by_value(&|v| v.tag == MODULE_NAME)
                                .refine_by_value(&|v| v.tag = S_CHARS)
                                .for_each(|(i, x)| println("imp {x}"));
                            ast.select_by_value(&|v| v.tag == INCLUDE_NAMESPACE)
                                .refine_by_value(&|v| v.tag = S_CHARS)
                                .for_each(|(i, x)| println("ref {x}"));
                            ExitSuccess
                        }
                    }
                }
            }
        },
        None => {
            eprintln!("usage: {prog} <filename>");
            ExitFailure
        }
    }
}
