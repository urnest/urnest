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

/// <reference path="../xju.ts"/>
/// <reference path="../xju/xn.ts"/>

namespace xju {
  export namespace assert {
    export function assertEqual<T>(x:T, y:T): T {  // returns x
      if (Array.isArray(x) && Array.isArray(y)){
        assertArraysEqual<any>(x,y);
      }
      else if (xju.isNonArrayObject(x) && xju.isNonArrayObject(y)){
        assertObjectsEqual(x,y);
      }
      else if (!(x===y)){
        throw new Error(`${format_(x)} != ${format_(y)}`);
      }
      return x;
    }
    export function assertNotEqual<T>(x:T, y:T): T{  // returns x
      try{
        assertEqual(x,y);
      }
      catch(e){
        return x;
      }
      throw new Error(`${format_(x)} == ${format_(y)}`);
    }
    export function assertArraysEqual<T>(x:Array<T>, y:Array<T>): Array<T> { // returnx x
      try{
        try{
          assertEqual(x.length, y.length);
        }
        catch(e:any){
          throw xju.xn.inContext('verify arrays have same length', e);
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
        return x;
      }
      catch(e:any){
        throw xju.xn.inContext(`verify array ${format_(x)} == array ${format_(y)}`, e);
      }
    }
    export function assertObjectsEqual(x:object, y:object): object{  // returns x
      try{
        var xKeys=new Array<keyof object>();
        var yKeys=new Array<keyof object>();
        for(const k in x){if (x.hasOwnProperty(k)) xKeys.push(k as keyof object);}
        for(const k in y){if (y.hasOwnProperty(k)) yKeys.push(k as keyof object);}
        xKeys.sort();
        yKeys.sort();
        assertArraysEqual(xKeys,yKeys);
        for(var i=0; i!=xKeys.length; ++i){
          const k = xKeys[i];
          try{
            assertEqual(x[k],y[k]);
          }
          catch(e: any){
            throw xju.xn.inContext(`verify property ${k} equal`, e);
          }
        }
        return x;
      }
      catch(e: any){
        throw xju.xn.inContext(`verify object ${format_(x)} == object ${format_(y)}`, e);
      }
    }
    export function assertXcontainsY(x:string, y:string): string{  // returns x
      if (x.indexOf(y)==-1){
        throw new Error(`"${x}" does not contain "${y}"`)
      }
      return x;
    }
    export function assertTrue(x:boolean): true{
      if (!x){
        throw new Error(`"${x}" is false, expected true`);
      }
      return x;
    }
    export function assertFalse(x:boolean): false{
      if (x){
        throw new Error(`"${x}" is true, expected false`);
      }
      return x;
    }
    export function assertNumberInRange(n: number, min: number, max: number): number{
      if (n < min || n > max) {
        throw new Error(`${n} is not in range ${min}..${max}`);
      }
      return n;
    }
    export function assertMatches(r: RegExp, x: string): Array<any> { // returns successful r.exec(x)
      const result=r.exec(x);
      if (result === null){
        throw new Error(`${format_(x)} does not match ${r}`);
      }
      return result;
    }
    export function assertNeverReached(v:any){
      throw new Error(`should not be here with ${format_(v)}`);
    }
  }
}
