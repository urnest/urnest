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
    export type IsInstance = (x:any) => false|ApplyDefaults;
    export type IsKey = (x:any) => boolean;
    export interface AsKey {
      typeName: string;
      f: (x:any) => void;
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
    export function asKeyInContext(asKey: AsKey): AsKey {
      return {
        typeName: asKey.typeName,
        f: (x: any) => {
          try {
            asKey.f(x);
          }
          catch(e) {
            throw xju.xn.inContext(`verify ${format_(x)} is a keyof ${asKey.typeName}`, e as Error);
          }
        }
      }
    }
    export const isInstanceOfString: IsInstance = (x:any) => {
      return typeof x == 'string' && applyNoDefaults;
    }
    export const isKeyOfString: IsKey = (x:any) => {
      return typeof x == 'string';
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
    export function asKeyOfString(typeName: string): AsKey {
      return asKeyInContext({
        typeName,
        f: (x:any) => {
          if (typeof x !== 'string') {
            throw new Error(`${format_(x)} is a ${typeof x} not a string`);
          }
        }
      });
    }

    export function isInstanceOfStringPattern(pattern: RegExp): IsInstance {
      return (x:any) => {
        return typeof x == 'string' &&
          pattern.exec(x)!==null &&
          applyNoDefaults;
      };
    }
    export function isKeyOfStringPattern(pattern: RegExp): IsKey {
      return (x:any) => {
        return typeof x == 'string' &&
          pattern.exec(x)!==null;
      };
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
    export function asKeyOfStringPattern(typeName: string, pattern: RegExp): AsKey {
      return asKeyInContext({
        typeName,
        f: (x: any) => {
          asKeyOfString(typeName);
          if (pattern.exec(x)===null) throw new Error(`${xju.format_(x)} is not a string with pattern ${pattern}`);
        }
      });
    }
    export const isInstanceOfFloat: IsInstance = (x:any) => {
      return typeof x == 'number' && applyNoDefaults;
    }
    export const isKeyOfFloat: IsKey = (x:any) => {
      return typeof x == 'string' && !isNaN(parseFloat(x));
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
    export function asKeyOfFloat(typeName: string): AsKey {
      return asKeyInContext({
        typeName,
        f: (x: any) => {
          if (typeof x !== 'string') {
            throw new Error(`${format_(x)} is a ${typeof x} not a string`);
          }
          if (isNaN(parseFloat(x))) {
            throw new Error(`${format_(x)} does not look like a number`);
          }
        }
      });
    }
    export const isInstanceOfInt: IsInstance = (x:any) => {
      return typeof x == 'number' &&
        x % 1 == 0 &&
        applyNoDefaults;
    }
    export const isKeyOfInt: IsKey = (x:any) => {
      return typeof x == 'string' && (parseFloat(x) % 1) == 0;
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
    export function asKeyOfInt(typeName: string): AsKey {
      return asKeyInContext({
        typeName,
        f: (x: any) => {
          if (typeof x !== 'string') {
            throw new Error(`${format_(x)} is a ${typeof x} not a string`);
          }
          const y = parseFloat(x);
          if (isNaN(y)) {
            throw new Error(`${format_(x)} does not look like a number`);
          }
          if (y % 1 !== 0) {
            throw new Error(`${format_(y)} is not a whole number`);
          }
        }
      });
    }
    export const isInstanceOfBool: IsInstance = (x:any) => {
      return typeof x == 'boolean' &&
        applyNoDefaults;
    }
    export const isKeyOfBool: IsKey = (x:any) => {
      return x == 'true' || x == 'false';
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
    export function asKeyOfBool(typeName: string): AsKey {
      return asKeyInContext({
        typeName,
        f: (x: any) => {
          if (typeof x !== 'string') {
            throw new Error(`${format_(x)} is a ${typeof x} not a string`);
          }
          if (x !== 'true' && x !== 'false') {
            throw new Error(`${format_(x)} is not "true" or "false"`);
          }
        }
      });
    }
    export const isInstanceOfNull: IsInstance = (x:any) => {
      return x === null &&
        applyNoDefaults;
    }
    export const isKeyOfNull: IsKey = (x:any) => {
      return x == 'null';
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
    export function asKeyOfNull(): AsKey {
      return asKeyInContext({
        typeName: "null",
        f: (x: any) => {
          if (typeof x !== 'string') {
            throw new Error(`${format_(x)} is a ${typeof x} not a string`);
          }
          if (x !== "null") {
            throw new Error(`${format_(x)} is not "null"`);
          }
        }
      });
    }

    export function isInstanceOfLiteral(literalValue: any): IsInstance {
      return (x:any) => {
        try {
          xju.assertEqual(x, literalValue);
          return applyNoDefaults;
        }
        catch (e:any) {
          return false;
        }
      }
    }
    export function isKeyOfLiteral(literalValue: any): IsKey {
      return (x:any) => {
        if (typeof x !== 'string') {
          return false;
        }
        if (typeof literalValue === 'string') {
          return x === literalValue;
        }
        try{
          return JSON.parse(x) === literalValue;
        }
        catch(e){
          return false;
        }
      }
    }
    export function asInstanceOfLiteral(literalValue: any): AsInstance {
      const typeName=JSON.stringify(literalValue);
      return asInstanceInContext({
        typeName,
        f: (x: any): ApplyDefaults => {
          xju.assertEqual(x, literalValue);
          return applyNoDefaults;
        }
      });
    }
    export function asKeyOfLiteral(literalValue: any): AsKey {
      const typeName=JSON.stringify(literalValue);
      return asKeyInContext({
        typeName,
        f: (x:any) => {
          if (typeof x !== 'string') {
            throw new Error(`${format_(x)} is not a string`);
          }
          if (typeof literalValue === 'string') {
            xju.assertEqual(x, literalValue);
          }
          else{
            try{
              xju.assertEqual(JSON.parse(x), literalValue);
            }
            catch(e){
              throw new Error(`${xju.format_(x)} is not the JSON representation of ${literalValue}`);
            }
          }
        }
      });
    }

    // any
    export const isInstanceOfAny: IsInstance = (x:any) => {
      return applyNoDefaults;
    }
    export function asInstanceOfAny(): AsInstance {
      return asInstanceInContext({
        typeName: "any",
        f: (x: any): ApplyDefaults => {
          return applyNoDefaults;
        }
      });
    }

    // list[T]
    export function isInstanceOfList(itemType: IsInstance): IsInstance {
      return (x:any) => {
        if (!Array.isArray(x)) return false;
        const defaulters: Array<ApplyDefaults> = [];
        for (let item of x) {
          const r = itemType(item);
          if (r === false) return false;
          defaulters.push(r);
        }
        return {
          applyDefaults: () => {
            defaulters.forEach( (d) => d.applyDefaults() );
            return true;
          }
        };
      };
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

    // bytes
    export const isInstanceOfBytes: IsInstance = (x:any) => {
      if (!Array.isArray(x)) return false;
      const defaulters: Array<ApplyDefaults> = [];
      for (let item of x) {
        const r = isInstanceOfInt(item);
        if (r === false) return false;
        if (item as number < 0 || item as number > 255) {
          return false;
        }
        defaulters.push(r);
      }
      return {
        applyDefaults: () => {
          defaulters.forEach( (d) => d.applyDefaults() );
          return true;
        }
      };
    };
    export const asInstanceOfBytes: AsInstance = (() => {
      const asInts = asInstanceOfList(asInstanceOfInt("byte"));
      return asInstanceInContext({
        typeName: `bytes`,
        f: (x: any): ApplyDefaults => {
          const r = asInts.f(x);
          (x as Array<number>).forEach((item, i) => {
            if (item < 0 || item > 255) {
              throw xju.xn.inContext(
                `validate item ${i}`,
                new Error(`${xju.format_(item)} is not in range 0..255`));
            }
          });
          return r;
        }
      });
    })();

    // list[any]
    export const isInstanceOfAnyList: IsInstance = (x:any) => {
      if (!Array.isArray(x)) return false;
      return applyNoDefaults;
    }
    export const asInstanceOfAnyList: AsInstance = asInstanceInContext({
      typeName: `Array`,
      f: (x: any): ApplyDefaults => {
        if (!Array.isArray(x)) {
          throw new Error(`${xju.format_(x)} is not an array it is a ${typeof x}`);
        }
        return applyNoDefaults;
      }
    });

    // union
    export function isInstanceOfUnion(alternatives: Array<IsInstance>): IsInstance {
      return (x:any) => {
        for (const alternative of alternatives) {
          const r=alternative(x);
          if (r !== false) {
            return r;
          }
        }
        return false;
      };
    }
    export function isKeyOfUnion(alternatives: Array<IsKey>): IsKey {
      return (x:any) => {
        for (const alternative of alternatives) {
          const r=alternative(x);
          if (r) {
            return r;
          }
        }
        return false;
      };
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
    export function asKeyOfUnion(alternatives: Array<AsKey>): AsKey {
      return asKeyInContext({
        typeName: alternatives.map( a=>a.typeName ).join('|'),
        f: (x: any) => {
          const es: Array<[string,any]> = [];
          for (const alt of alternatives) {
            try {
              alt.f(x);
              return;
            }
            catch(e: any){
              es.push(e);
            }
          }
          throw new Error(es.map( (error) => `${error}`).join(' and '));
        }
      });
    }

    // dict
    export function isInstanceOfDict(
      isKey: IsKey,
      isValue: IsInstanceOf): IsInstance {
      return (x:any) => {
        if (!xju.isObject(x)) return false;
        if (Array.isArray(x)) return false;
        const defaulters: Array<ApplyDefaults> = [];
        for(const key in x) {
          if (x.hasOwnProperty(key)) {
            if (!isKey(key)) return false;
            const r = isInstance(x[key as keyof typeof x]);
            if (r===false) return false;
            defaulters.push(r);
          }
        }          
        return {
          applyDefaults: () => {
            defaulters.forEach( (d) => d.applyDefaults() );
            return true;
          }
        };
      };
    }
    export function asInstanceOfDict(
      asKey: AsKey,
      asValue: AsInstanceOf
    ): AsInstance {
      return asInstanceInContext({
        typeName: `{ [key ${asKey.typename}] : ${asValue.typeName} }`,
        f: (x: any): ApplyDefaults => {
          if (!xju.isObject(x)) {
            throw new Error(`not an object it is a ${typeof x}`);
          }
          if (Array.isArray(x)) {
            throw new Error('not an object it is an array');
          }
          const defaulters: Array<ApplyDefaults> = [];
          for(const key in x){
            try {
              asKey(key);
              if (x.hasOwnProperty(key)) {
                defaulters.push(asValue.f(x[key as keyof typeof x]));
              }
            }
            catch(e:any) {
              throw xju.xn.inContext(`validate property ${key}`, e);
            }
          }
          return {
            applyDefaults: () => {
              defaulters.forEach( (d) => d.applyDefaults() );
              return true;
            }
          }
        }
      });
    }

    // dict[any,any]
    export const isInstanceOfAnyDict = (x:any) => {
      if (!xju.isObject(x)) return false;
      if (Array.isArray(x)) return false;
      return applyNoDefaults;
    };
    export const asInstanceOfAnyDict = asInstanceInContext({
      typeName: `{ [key any] : any }`,
      f: (x: any): ApplyDefaults => {
        if (!xju.isObject(x)) {
          throw new Error(`not an object it is a ${typeof x}`);
        }
        if (Array.isArray(x)) {
          throw new Error('not an object it is an array');
        }
        return applyNoDefaults;
      }
    });

    // tuple
    export function isInstanceOfTuple(items: Array<IsInstance>): IsInstance {
      return (x:any) => {
        if (!Array.isArray(x)) return false;
        if (x.length != items.length) return false;
        const defaulters: Array<ApplyDefaults> = [];
        for (let i=0; i != items.length; ++i) {
          const r = items[i](x[i]);
          if (r===false) return false;
          defaulters.push(r);
        }
        return  {
          applyDefaults: () => {
            defaulters.forEach( (d) => d.applyDefaults() );
            return true;
          }
        };
      };
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

    // class
    export function isInstanceOfClass(
      className: string,
      items: Array<{
        propertyName: string;
        isInstance: IsInstance;
        defaultValue?: any;
      }>
    ): IsInstance {
      return (x:any) => {
        if (!xju.isObject(x)) return false;
        if (Array.isArray(x)) return false;
        const defaulters: Array<ApplyDefaults> = [];
        for(const {propertyName, isInstance, defaultValue} of items) {
          if (x.hasOwnProperty(propertyName)) {
            const r = isInstance(x[propertyName as keyof typeof x]);
            if (r===false) return false;
            defaulters.push(r);
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
            return false;
          }
        }          
        return {
          applyDefaults: () => {
            defaulters.forEach( (d) => d.applyDefaults() );
            return true;
          }
        };
      };
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
                defaulters.push(asInstance.f(x[propertyName as keyof typeof x]));
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
