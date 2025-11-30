// Copyright (c) 2025 Trevor Taylor
// 
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that all
// copyright notices and this permission notice appear in all copies.
// 
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
namespace xju {
  export function str(x:any):string { return `${x}`; }
  export function isNonArrayObject(x:any):x is object { return x !== null && typeof(x) === 'object' && !Array.isArray(x); }
  export function format_<T>(a:T): string {
    if (typeof a==='undefined') {
      return 'undefined';
    }
    if (Array.isArray(a)) return formatArray(a);
    if (isNonArrayObject(a)) return formatObject(a);
    return JSON.stringify(a);
  }
  export function formatArray<T>(a:Array<T>): string {
    if(a.length==0) return "[]";
    if(a.length==1) return `[${format_(a[0])}]`;
    var result=`${format_(a[0])}`;
    for(var i=1;i!=a.length;++i) result=result+`,${format_(a[i])}`;
    return `[${result}]`;
  }
  export function formatObject(a:Object): string {
    var result="{";
    for(const k of Object.keys(a).slice(0,1)) result=result+` ${k}:${format_(a[k as keyof Object])}`;
    for(const k of Object.keys(a).slice(1)) result=result+`, ${k}:${format_(a[k as keyof Object])}`;
    return result+" }";
  }
}
