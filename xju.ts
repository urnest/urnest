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
  export function isObject(x:any):x is object { return x !== null && typeof(x) === 'object'; }
  export function assertEqual<T>(x:T, y:T){
    if (Array.isArray(x) && Array.isArray(y)){
      assertArraysEqual<any>(x,y);
    }
    else if (isObject(x) && isObject(y)){
      assertObjectsEqual(x,y);
    }
    else if (!(x===y)){
      throw new Error(`${format_(x)} != ${format_(y)}`);
    }
  }
  export function assertNotEqual<T>(x:T, y:T){
    try{
      assertEqual(x,y);
    }
    catch(e){
      return;
    }
    throw new Error(`${format_(x)} == ${format_(y)}`);
  }
  export function format_<T>(a:T): string {
    if (typeof a==='undefined') {
      return 'undefined';
    }
    if (Array.isArray(a)) return formatArray(a);
    if (isObject(a)) return formatObject(a);
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
  export function assertArraysEqual<T>(x:Array<T>, y:Array<T>){
    try{
      try{
        assertEqual(x.length, y.length);
      }
      catch(e:any){
        throw new Error(`arrays of different length because ${e}`);
      }
      var i;
      for(i=0; i!=x.length; ++i){
        try{
          assertEqual<T>(x[i],y[i]);
        }
        catch(e:any){
          throw new Error(`array element ${i} ${format_(x[i])} != ${format_(y[i])}`);
        }
      }
    }
    catch(e:any){
      throw new Error(`array ${format_(x)} != array ${format_(y)} because ${e}`);
    }
  }
  export function assertObjectsEqual(x:object, y:object){
    try{
      var xKeys=new Array<keyof object>();
      var yKeys=new Array<keyof object>();
      for(const k in x){if (x.hasOwnProperty(k)) xKeys.push(k as keyof object);}
      for(const k in y){if (y.hasOwnProperty(k)) yKeys.push(k as keyof object);}
      assertArraysEqual(xKeys,yKeys);
      for(var i=0; i!=xKeys.length; ++i){
        const k = xKeys[i];
        try{
          assertEqual(x[k],y[k]);
        }
        catch(e){
          throw new Error(`property ${k} is not equal because ${e}`);
        }
      }
    }
    catch(e){
      throw new Error(`object ${format_(x)} != object ${format_(y)} because ${e}`);
    }
  }
  export function assertXcontainsY(x:string, y:string){
    if (x.indexOf(y)==-1){
        throw new Error(`"${x}" does not contain "${y}"`)
    }
  }
  export function assertTrue(x:boolean){
    if (!x){
      throw new Error(`"${x}" is false, expected true`);
    }
  }
  export function assertFalse(x:boolean){
    if (x){
      throw new Error(`"${x}" is true, expected false`);
    }
  }
  export function assertNeverReached(v:any){
    throw new Error(`should not be here with ${format_(v)}`);
  }
}
