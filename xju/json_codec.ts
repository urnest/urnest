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

/// <reference path="../xju/xn.ts"/>
/// <reference path="../xju.ts"/>

namespace xju {
  export namespace json_codec {
    export interface ApplyDefaults {
      applyDefaults(): true;
    }
    export interface AsInstance {
      typeName: string;
      f: (x:any) => ApplyDefaults;
    }
    export const applyNoDefaults: ApplyDefaults = {
      applyDefaults: () => true
    };
    export function asInstanceInContext(asInstance: AsInstance): AsInstance {
      return {
        typeName: asInstance.typeName,
        f: (x: any): ApplyDefaults => {
          try {
            return asInstance.f(x);
          }
          catch(e) {
            throw xju.xn.inContext(`verify ${format_(x)} is a ${asInstance.typeName}`, e as Error);
          }
        }
      }
    }
    export function asInstanceOfString(typeName: string): AsInstance {
      return asInstanceInContext({
        typeName: typeName,
        f: (x: any): ApplyDefaults => {
          if (typeof x !== 'string') {
            throw new Error(`${format_(x)} is a ${typeof x} not a string`);
          }
          return applyNoDefaults;
        }
      });
    }
    export function asInstanceOfStringPattern(typeName: string, pattern: RegExp): AsInstance {
      return asInstanceInContext({
        typeName,
        f: (x: any): ApplyDefaults => {
          asInstanceOfString(x);
          if (pattern.exec(x)===null) throw new Error(`${xju.format_(x)} is not a string with pattern ${pattern}`);
          return applyNoDefaults;
        }
      });
    }
    export function asInstanceOfFloat(typeName: string): AsInstance {
      return asInstanceInContext({
        typeName,
        f: (x: any): ApplyDefaults => {
          if (typeof x !== 'number') {
            throw new Error(`${format_(x)} is a ${typeof x} not a number`);
          }
          return applyNoDefaults;
        }
      });
    }
    export function asInstanceOfInt(typeName: string): AsInstance {
      return asInstanceInContext({
        typeName,
        f: (x: any): ApplyDefaults => {
          if (typeof x !== 'number') {
            throw new Error(`${format_(x)} is a ${typeof x} not a number`);
          }
          if (x % 1 !== 0) {
            throw new Error(`${format_(x)} is not a whole number`);
          }
          return applyNoDefaults;
        }
      });
    }
    export function asInstanceOfBool(typeName: string): AsInstance {
      return asInstanceInContext({
        typeName,
        f: (x: any): ApplyDefaults => {
          if (typeof x !== 'boolean') {
            throw new Error(`${format_(x)} is a ${typeof x} not a boolean`);
          }
          return applyNoDefaults;
        }
      });
    }
    export function asInstanceOfNull(): AsInstance {
      return asInstanceInContext({
        typeName: "null",
        f: (x: any): ApplyDefaults => {
          if (x !== null) {
            throw new Error(`${format_(x)} is not null`);
          }
          return applyNoDefaults;
        }
      });
    }
    export function asInstanceOfAny(): AsInstance {
      return asInstanceInContext({
        typeName: "any",
        f: (x: any): ApplyDefaults => {
          return applyNoDefaults;
        }
      });
    }
    export function asInstanceOfList(itemType: AsInstance): AsInstance {
      return asInstanceInContext({
        typeName: `Array<${itemType.typeName}>`,
        f: (x: any): ApplyDefaults => {
          if (!Array.isArray(x)) {
            throw new Error(`${xju.format_(x)} is not an array it is a ${typeof x}`);
          }
          const defaulters: Array<ApplyDefaults> = [];
          x.forEach( (item, i) => {
            try {
              defaulters.push(itemType.f(item));
            }
            catch(e:any) {
              throw xju.xn.inContext(`validate item ${i}`, e);
            }
          });
          return {
            applyDefaults: () => {
              defaulters.forEach( (d) => d.applyDefaults() );
              return true;
            }
          }
        }
      });
    }
    export function asInstanceOfAnyList(): AsInstance {
      return asInstanceInContext({
        typeName: `Array`,
        f: (x: any): ApplyDefaults => {
          if (!Array.isArray(x)) {
            throw new Error(`${xju.format_(x)} is not an array it is a ${typeof x}`);
          }
          return applyNoDefaults;
        }
      });
    }
    export function asInstanceOfUnion(alternatives: Array<AsInstance>): AsInstance {
      return asInstanceInContext({
        typeName: alternatives.map( a=>a.typeName ).join('|'),
        f: (x: any): ApplyDefaults => {
          const es: Array<[string,any]> = [];
          for (const alt of alternatives) {
            try {
              return alt.f(x);
            }
            catch(e: any){
              es.push(e);
            }
          }
          throw new Error(es.map( (error) => `${error}`).join(' and '));
        }
      });
    }
    export function asInstanceOfTuple(items: Array<AsInstance>): AsInstance {
      return asInstanceInContext({
        typeName: `[${items.map(item=>item.typeName).join(',')}]`,
        f: (x: any): ApplyDefaults => {
          if (!Array.isArray(x)) {
            throw new Error(`not an array it is a ${typeof x}`);
          }
          if (x.length != items.length) {
            throw new Error(`length is ${x.length} not ${items.length}`);
          }
          const defaulters: Array<ApplyDefaults> = [];
          x.forEach( (item, i) => {
            try {
              defaulters.push(items[i].f(item));
            }
            catch(e:any) {
              throw xju.xn.inContext(`validate item ${i}`, e);
            }
          });
          return {
            applyDefaults: () => {
              defaulters.forEach( (d) => d.applyDefaults() );
              return true;
            }
          }
        }
      });
    }
    export function asInstanceOfClass(
      className: string,
      items: Array<{
        propertyName: string;
        asInstance: AsInstance;
        defaultValue?: any;
      }>): AsInstance {
      return asInstanceInContext({
        typeName: `${className}`,
        f: (x: any): ApplyDefaults => {
          if (!xju.isObject(x)) {
            throw new Error(`not an object it is a ${typeof x}`);
          }
          if (Array.isArray(x)) {
            throw new Error('not an object it is an array');
          }
          const defaulters: Array<ApplyDefaults> = [];
          items.forEach( ({propertyName, asInstance, defaultValue}) => {
            try {
              if (x.hasOwnProperty(propertyName)) {
                asInstance.f(x[propertyName as keyof typeof x]);
              }
              else if (defaultValue !== undefined) {
                defaulters.push({
                  applyDefaults: () => {
                    (x as any)[propertyName as keyof object] = defaultValue;
                    return true;
                  }
                });
              }
              else {
                throw new Error(`property ${propertyName} is missing`);
              }
            }
            catch(e:any) {
              throw xju.xn.inContext(`validate property ${propertyName}`, e);
            }
          });
          return {
            applyDefaults: () => {
              defaulters.forEach( (d) => d.applyDefaults() );
              return true;
            }
          }
        }
      });
    }
          
  }
}
