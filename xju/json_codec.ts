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
export namespace xju {
  export namespace json_codec {
    export interface ApplyDefaults {
      applyDefaults(): true;
    }
    export interface AsInstance {
      typeName: string;
      f: (x:any) => ApplyDefaults;
    }
    export const applyNoDefaults: ApplyDefaults = {
      applyDefaults: () => true;
    };
    export function asInstanceInContext(asInstance: AsInstance) => AsInstance {
      return (x: any): ApplyDefaults {
        try {
          asInstance.f(x);
        }
        catch(e) {
          throw xju.xn.inContext(`verify ${format_(x)} is a ${asInstance.typeName}`);
        }
      }
    }
    export function asInstanceOfString(typeName: string) => AsInstance {
      return asInstanceInContext({
        typeName: typeName,
        f: (x: any): ApplyDefaults {
          if (typeof x !== 'string') {
            throw new Error(`${format_(x)} is a ${typeof x} not a string`);
          }
          return applyNoDefaults;
        }
      });
    }
    export function asInstanceOfString(typeName: string, pattern: RegExp) => AsInstance {
      return asInstanceInContext({
        typeName,
        f: (x: any): ApplyDefaults {
          if (pattern.exec(asInstanceOfString(x))===null) throw new Error(`${{xju.ts.format_(x)}} is not a string with pattern ${pattern}`);
          return applyNoDefaults;
        }
      });
    }
    export function asInstanceOfFloat(typeName: string) => AsInstance {
      return asInstanceInContext({
        typeName,
        f: (x: any): ApplyDefaults {
          if (typeof x !== 'number') {
            throw new Error(`${format_(x)} is a ${typeof x} not a number`);
          }
          return applyNoDefaults;
        }
      });
    }
    export function asInstanceOfInt(typeName: string) => AsInstance {
      return asInstanceInContext({
        typeName,
        f: (x: any): ApplyDefaults {
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
    export function asInstanceOfBool(typeName: string) => AsInstance {
      return asInstanceInContext({
        typeName,
        f: (x: any): ApplyDefaults {
          if (typeof x !== 'boolean') {
            throw new Error(`${format_(x)} is a ${typeof x} not a boolean`);
          }
          return applyNoDefaults;
        }
      });
    }
    export function asInstanceOfNull() => AsInstance {
      return asInstanceInContext({
        typeName: "null",
        f: (x: any): ApplyDefaults {
          if (typeof x !== null) {
            throw new Error(`${format_(x)} is not null`);
          }
          return applyNoDefaults;
        }
      });
    }
    export function asInstanceOfAny() => AsInstance {
      return asInstanceInContext({
        typeName: "any",
        f: (x: any): ApplyDefaults {
          return applyNoDefaults;
        }
      });
    }
    export function asInstanceOfList(itemType: AsInstance) => AsInstance {
      return asInstanceInContext({
        typeName: `Array<${itemType.typeName}>`,
        f: (x: any): ApplyDefaults {
          if (!Array.isArray(x)) {
            throw new Error(`${format_(x) is not an array it is a ${typeof x}`);
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
    export function asInstanceOfAnyList() => AsInstance {
      return asInstanceInContext({
        typeName: `Array<${itemType.typeName}>`,
        f: (x: any): ApplyDefaults {
          if (!Array.isArray(x)) {
            throw new Error(`${format_(x) is not an array it is a ${typeof x}`);
          }
          return applyNoDefaults;
        }
      });
    }
    export function asInstanceOfUnion(alternatives: Array<AsInstance>): AsInstance {
      return asInstanceInContext({
        typeName: '|'.join(alternatives.map( a=>a.typeName )),
        f: (x: any): ApplyDefaults {
          const es: Array<[string,any]> = [];
          for (alt of alternatives) {
            try {
              return alt.f(x);
            }
            catch(e: any){
              es.push(e);
            }
          }
          throw new Error(' and '.join(es.map(typeName,error) => `not a ${typeName} because ${e}`));
        }
      });
    }
    export function asInstanceOfTuple(items: Array<AsInstance>): AsInstance {
      return asInstanceInContext({
        typeName: `[${','.join(items.map(item=>item.typeName))}]`,
        f: (x: any): ApplyDefaults {
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
    export function asInstanceOfTuple(
      items: Array<{
        propertyName: string;
        asInstance: AsInstance;
        defaultValue?: any;
      }>): AsInstance {
      return asInstanceInContext({
        typeName: `{${';'.join(items.map( {propertyName, asInstance}=> ` ${propertyName}: {item.typeName}`))} }`,
        f: (x: any): ApplyDefaults {
          if (!xju.isObject(x)) {
            throw new Error(`not an object it is a ${typeof x}`);
          }
          if (Array.isArray(x)) {
            throw new Error('not an object it is an array');
          }
          const defaulters: Array<ApplyDefaults> = [];
          x.forEach( {propertyName, asInstance, defaultValue} ) => {
            try {
              if (x.hasOwnProperty(propertyName)) {
                asInstance.f(x[propertyName]);
              }
              else if (defaultValue !== undefined) {
                defaulters.push({
                  applyDefaults: () => { x[propertyName] = defaultValue; return true }
                });
              }
              else {
                throw new Error(`property ${propertyName} is missing`);
              }
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
          
  }
}
