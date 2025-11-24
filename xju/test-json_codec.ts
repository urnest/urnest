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
/// <reference path="../xju/json_codec.ts"/>

xju.assertEqual(xju.json_codec.applyNoDefaults.applyDefaults(), true);

xju.assertEqual((xju.json_codec.isInstanceOfString('Fred') as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assertEqual(xju.json_codec.isInstanceOfString(7), false);

xju.json_codec.asInstanceOfString('Surname').f('Fred').applyDefaults();
try{
  xju.json_codec.asInstanceOfString('Surname').f(7).applyDefaults();
  xju.assertNeverReached('Surname');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify 7 is a Surname because 7 is a number not a string');
}

xju.assertEqual(xju.json_codec.isKeyOfString('Fred'), true);
xju.assertEqual(xju.json_codec.isKeyOfString(7), false);

xju.json_codec.asKeyOfString('Surname').f('Fred');
try{
  xju.json_codec.asKeyOfString('Surname').f(7);
  xju.assertNeverReached('Surname');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify 7 is a keyof Surname because 7 is a number not a string');
}

xju.assertEqual((xju.json_codec.isInstanceOfStringPattern(/^[A-Z]/)('Fred') as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assertEqual(xju.json_codec.isInstanceOfStringPattern(/^[A-Z]/)(7), false);

xju.json_codec.asInstanceOfStringPattern('Surname', /^[A-Z]/).f('Fred').applyDefaults();
try{
  xju.json_codec.asInstanceOfStringPattern('Surname', /^[A-Z]/).f('fred').applyDefaults();
  xju.assertNeverReached('Surname');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify "fred" is a Surname because "fred" is not a string with pattern /^[A-Z]/');
}

xju.assertEqual(xju.json_codec.isKeyOfStringPattern(/^[A-Z]/)('Fred'), true);
xju.assertEqual(xju.json_codec.isKeyOfStringPattern(/^[A-Z]/)('fred'), false);
xju.assertEqual(xju.json_codec.isKeyOfStringPattern(/^[A-Z]/)(7), false);

xju.json_codec.asKeyOfStringPattern('Surname', /^[A-Z]/).f('Fred');
try{
  xju.json_codec.asKeyOfStringPattern('Surname', /^[A-Z]/).f('fred');
  xju.assertNeverReached('Surname');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify "fred" is a keyof Surname because "fred" is not a string with pattern /^[A-Z]/');
}


xju.assertEqual((xju.json_codec.isInstanceOfFloat(8.2) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assertEqual(xju.json_codec.isInstanceOfFloat([]), false);

xju.json_codec.asInstanceOfFloat('Metres').f(8.2).applyDefaults();
try{
  xju.json_codec.asInstanceOfFloat('Metres').f([]).applyDefaults();
  xju.assertNeverReached('Metres');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify [] is a Metres because [] is a object not a number');
}

xju.assertEqual(xju.json_codec.isKeyOfFloat("8.2"), true);
xju.assertEqual(xju.json_codec.isKeyOfFloat(8.2), false);
xju.assertEqual(xju.json_codec.isKeyOfFloat([]), false);

xju.json_codec.asKeyOfFloat('Metres').f("8.2");
try{
  xju.json_codec.asKeyOfFloat('Metres').f("fred");
  xju.assertNeverReached('Metres');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify "fred" is a keyof Metres because "fred" does not look like a number');
}
try{
  xju.json_codec.asKeyOfFloat('Metres').f([]);
  xju.assertNeverReached('Metres');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify [] is a keyof Metres because [] is a object not a string');
}

xju.assertEqual((xju.json_codec.isInstanceOfInt(8) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assertEqual(xju.json_codec.isInstanceOfInt(8.2), false);

xju.json_codec.asInstanceOfInt('ByteCount').f(8).applyDefaults();
try{
  xju.json_codec.asInstanceOfInt('ByteCount').f(8.2).applyDefaults();
  xju.assertNeverReached('ByteCount');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify 8.2 is a ByteCount because 8.2 is not a whole number');
}

xju.assertEqual(xju.json_codec.isKeyOfInt("8"), true);
xju.assertEqual(xju.json_codec.isKeyOfInt("8.2"), false);
xju.assertEqual(xju.json_codec.isKeyOfInt(8), false);

xju.json_codec.asKeyOfInt('ByteCount').f("8");
try{
  xju.json_codec.asKeyOfInt('ByteCount').f("8.2");
  xju.assertNeverReached('ByteCount');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify "8.2" is a keyof ByteCount because 8.2 is not a whole number');
}

xju.assertEqual((xju.json_codec.isInstanceOfBool(true) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assertEqual(xju.json_codec.isInstanceOfBool(8.2), false);

xju.json_codec.asInstanceOfBool('Enabled').f(true).applyDefaults();
try{
  xju.json_codec.asInstanceOfBool('Enabled').f(8.2).applyDefaults();
  xju.assertNeverReached('Enabled');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify 8.2 is a Enabled because 8.2 is a number not a boolean');
}

xju.assertEqual(xju.json_codec.isKeyOfBool("true"), true);
xju.assertEqual(xju.json_codec.isKeyOfBool(8.2), false);
xju.assertEqual(xju.json_codec.isKeyOfBool(false), false);

xju.json_codec.asKeyOfBool('Enabled').f("true");
try{
  xju.json_codec.asKeyOfBool('Enabled').f(8.2);
  xju.assertNeverReached('Enabled');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify 8.2 is a keyof Enabled because 8.2 is a number not a string');
}
try{
  xju.json_codec.asKeyOfBool('Enabled').f("fred");
  xju.assertNeverReached('Enabled');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify "fred" is a keyof Enabled because "fred" is not "true" or "false"');
}

// null
xju.assertEqual((xju.json_codec.isInstanceOfNull(null) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assertEqual(xju.json_codec.isInstanceOfNull(8.2), false);

xju.json_codec.asInstanceOfNull().f(null).applyDefaults();
try{
  xju.json_codec.asInstanceOfNull().f(8.2).applyDefaults();
  xju.assertNeverReached(null);
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify 8.2 is a null because 8.2 is not null');
}

xju.assertEqual(xju.json_codec.isKeyOfNull("null"), true);
xju.assertEqual(xju.json_codec.isKeyOfNull(8.2), false);
xju.assertEqual(xju.json_codec.isKeyOfNull(null), false);

xju.json_codec.asKeyOfNull().f("null");
try{
  xju.json_codec.asKeyOfNull().f("8.2");
  xju.assertNeverReached(null);
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify "8.2" is a keyof null because "8.2" is not "null"');
}
try{
  xju.json_codec.asKeyOfNull().f(8.2);
  xju.assertNeverReached(null);
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify 8.2 is a keyof null because 8.2 is a number not a string');
}

// literal
var is=xju.json_codec.isInstanceOfLiteral('fred');
xju.assertEqual((is('fred') as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assertEqual(is(8.2), false);

var isKey=xju.json_codec.isKeyOfLiteral('fred');
xju.assertEqual(isKey('fred'),true);
xju.assertEqual(isKey('jock'),false);
xju.assertEqual(isKey(8.2),false);

var as=xju.json_codec.asInstanceOfLiteral('fred');
as.f('fred').applyDefaults();
try{
  as.f(8.2);
  xju.assertNeverReached('literal');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify 8.2 is a "fred" because 8.2 != "fred"');
}

var asKey=xju.json_codec.asKeyOfLiteral('fred');
asKey.f('fred');
try{
  asKey.f('jock');
  xju.assertNeverReached('literal');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify "jock" is a keyof "fred" because "jock" != "fred"');
}
try{
  asKey.f(8);
  xju.assertNeverReached('literal');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify 8 is a keyof "fred" because 8 is not a string');
}

var is=xju.json_codec.isInstanceOfLiteral(17);
xju.assertEqual((is(17) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assertEqual(is("8.2"), false);

var isKey=xju.json_codec.isKeyOfLiteral(17);
xju.assertEqual(isKey("17"),true);
xju.assertEqual(isKey('jock'),false);
xju.assertEqual(isKey(8.2),false);

var as=xju.json_codec.asInstanceOfLiteral(17);
as.f(17).applyDefaults();
try{
  as.f(8.2);
  xju.assertNeverReached('literal');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify 8.2 is a 17 because 8.2 != 17');
}

var asKey=xju.json_codec.asKeyOfLiteral(17);
asKey.f('17');
try{
  asKey.f('jock');
  xju.assertNeverReached('literal');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify "jock" is a keyof 17 because "jock" is not the JSON representation of 17');
}

// any
xju.assertEqual((xju.json_codec.isInstanceOfAny(8) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.json_codec.asInstanceOfAny().f(8).applyDefaults();

// any list
xju.assertEqual((xju.json_codec.isInstanceOfAnyList([1,'fred',null]) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assertEqual(xju.json_codec.isInstanceOfAnyList(8.2), false);

xju.json_codec.asInstanceOfAnyList.f([1,'fred',null]).applyDefaults();
try{
  xju.json_codec.asInstanceOfAnyList.f(8.2).applyDefaults();
  xju.assertNeverReached('[]');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify 8.2 is a Array because 8.2 is not an array it is a number');
}

// list
xju.assertEqual((xju.json_codec.isInstanceOfList(xju.json_codec.isInstanceOfInt)([1,2,3]) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assertEqual(xju.json_codec.isInstanceOfList(xju.json_codec.isInstanceOfInt)([1,'fred']), false);
xju.assertEqual(xju.json_codec.isInstanceOfList(xju.json_codec.isInstanceOfInt)({}), false);

xju.json_codec.asInstanceOfList(xju.json_codec.asInstanceOfInt('int')).f([1,2,3]).applyDefaults();
try{
  xju.json_codec.asInstanceOfList(xju.json_codec.asInstanceOfInt('int')).f([1,'fred']).applyDefaults();
  xju.assertNeverReached('int[]');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify [1,"fred"] is a Array<int> because failed to validate item 1 because failed to verify "fred" is a int because "fred" is a string not a number');
}
try{
  xju.json_codec.asInstanceOfList(xju.json_codec.asInstanceOfInt('ByteCount')).f({}).applyDefaults();
  xju.assertNeverReached('int[]');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify { } is a Array<ByteCount> because { } is not an array it is a object');
}

// bytes
xju.assertEqual((xju.json_codec.isInstanceOfBytes([0,1,2,3,255]) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assertEqual(xju.json_codec.isInstanceOfBytes([1,-1]), false);
xju.assertEqual(xju.json_codec.isInstanceOfBytes([256,0]), false);
xju.assertEqual(xju.json_codec.isInstanceOfBytes([1,'fred']), false);
xju.assertEqual(xju.json_codec.isInstanceOfBytes({}), false);

xju.json_codec.asInstanceOfBytes.f([0,1,2,3,255]).applyDefaults();
try{
  xju.json_codec.asInstanceOfBytes.f([-1,0,1,2,3,255]).applyDefaults();
  xju.assertNeverReached('bytes');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify [-1,0,1,2,3,255] is a bytes because failed to validate item 0 because -1 is not in range 0..255');
}
try{
  xju.json_codec.asInstanceOfBytes.f([,0,1,2,3,255,256]).applyDefaults();
  xju.assertNeverReached('bytes');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify [undefined,0,1,2,3,255,256] is a bytes because failed to validate item 6 because 256 is not in range 0..255');
}
try{
  xju.json_codec.asInstanceOfBytes.f({}).applyDefaults();
  xju.assertNeverReached('bytes');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify { } is a bytes because failed to verify { } is a Array<byte> because { } is not an array it is a object');
}

// union
var is=xju.json_codec.isInstanceOfUnion([
  xju.json_codec.isInstanceOfInt,
  xju.json_codec.isInstanceOfString
]);
xju.assertEqual((is(1) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assertEqual((is('fred') as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assertEqual(is([1]), false);

var isKey=xju.json_codec.isKeyOfUnion([
  xju.json_codec.isKeyOfInt,
  xju.json_codec.isKeyOfString
]);
xju.assertEqual(isKey("1"), true);
xju.assertEqual(isKey('fred'), true);
xju.assertEqual(isKey([1]), false);

var as=xju.json_codec.asInstanceOfUnion([
  xju.json_codec.asInstanceOfInt('int'),
  xju.json_codec.asInstanceOfString('Surname')
]);
as.f(1).applyDefaults();
as.f('fred').applyDefaults();
try{
  as.f([1]).applyDefaults();
  xju.assertNeverReached('Array<int|string>');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify [1] is a int|Surname because Error: failed to verify [1] is a int because [1] is a object not a number and Error: failed to verify [1] is a Surname because [1] is a object not a string');
}

var asKey=xju.json_codec.asKeyOfUnion([
  xju.json_codec.asKeyOfInt('int'),
  xju.json_codec.asKeyOfBool('Enabled')
]);
asKey.f('1');
asKey.f('true');
try{
  asKey.f([1]);
  xju.assertNeverReached('Array<int|string>');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify [1] is a keyof int|Enabled because Error: failed to verify [1] is a keyof int because [1] is a object not a string and Error: failed to verify [1] is a keyof Enabled because [1] is a object not a string');
}

// tuple
var is=xju.json_codec.isInstanceOfTuple([
  xju.json_codec.isInstanceOfInt,
  xju.json_codec.isInstanceOfString
]);
xju.assertEqual((is([1,'fred']) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assertEqual(is({}), false);
xju.assertEqual(is([1]), false);
xju.assertEqual(is([1,'fred',2]), false);
xju.assertEqual(is([1, 8.2]), false);

var as=xju.json_codec.asInstanceOfTuple([
  xju.json_codec.asInstanceOfInt('int'),
  xju.json_codec.asInstanceOfString('Surname')
]);
as.f([1,'fred']).applyDefaults();
try{
  as.f({}).applyDefaults();
  xju.assertNeverReached('[int,string]');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify { } is a [int,Surname] because not an array it is a object');
}
try{
  as.f([1]).applyDefaults();
  xju.assertNeverReached('[int,string]');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify [1] is a [int,Surname] because length is 1 not 2');
}
as.f([1,'fred']).applyDefaults();
try{
  as.f([1, 8.2]).applyDefaults();
  xju.assertNeverReached('[int,string]');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify [1,8.2] is a [int,Surname] because failed to validate item 1 because failed to verify 8.2 is a Surname because 8.2 is a number not a string');
}

var is=xju.json_codec.isInstanceOfClass('Fred',[
  {
    propertyName: 'a',
    isInstance: xju.json_codec.isInstanceOfInt
  },
  {
    propertyName: 'b',
    isInstance: xju.json_codec.isInstanceOfClass('Fred',[
      {
        propertyName: 'c',
        isInstance: xju.json_codec.isInstanceOfString,
        defaultValue: 'Mack'
      }]),
    defaultValue: {'c': 'Betty'}
  }
]);

{
  const x: any = {a: 2};
  (is(x) as xju.json_codec.ApplyDefaults).applyDefaults();
  xju.assertEqual(x, {a:2, b:{c:'Betty'}});
}
{
  const x: any = {a: 2, b:{}};
  (is(x) as xju.json_codec.ApplyDefaults).applyDefaults();
  xju.assertEqual(x, {a:2, b:{c:'Mack'}});
}
{
  const x: any = {a: 2, b:{c: 'Al'}};
  (is(x) as xju.json_codec.ApplyDefaults).applyDefaults();
  xju.assertEqual(x, {a:2, b:{c:'Al'}});
}

xju.assertEqual(is(9), false);
xju.assertEqual(is([1]), false);
xju.assertEqual(is({a: 'fred', b: 'Mack'}), false);
xju.assertEqual(is({b: 'Mack'}), false);

var as=xju.json_codec.asInstanceOfClass('Fred',[
  {
    propertyName: 'a',
    asInstance: xju.json_codec.asInstanceOfInt('int')
  },
  {
    propertyName: 'b',
    asInstance: xju.json_codec.isInstanceOfClass('Fred',[
      {
        propertyName: 'c',
        isInstance: xju.json_codec.asInstanceOfString('Surname'),
        defaultValue: 'Mack'
      }]),
    defaultValue: {'c': 'Betty'}
  }
]);

{
  const x: any = {a: 2};
  as.f(x).applyDefaults();
  xju.assertEqual(x, {a:2, b:{c:'Betty'}});
}
{
  const x: any = {a: 2, b:{}};
  as.f(x).applyDefaults();
  xju.assertEqual(x, {a:2, c:'Mack'});
}
{
  const x: any = {a: 2, b:{c:'Jones'}};
  as.f(x).applyDefaults();
  xju.assertEqual(x, {a:2, b:{c:'Jones'}});
}

try{
  as.f(9).applyDefaults();
  xju.assertNeverReached('class');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify 9 is a Fred because not an object it is a number');
}

try{
  as.f([1]).applyDefaults();
  xju.assertNeverReached('class');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify [1] is a Fred because not an object it is an array');
}

try{
  as.f({a: 'fred', b: 'Mack'}).applyDefaults();
  xju.assertNeverReached('class');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify { a:"fred", b:"Mack" } is a Fred because failed to validate property a because failed to verify "fred" is a int because "fred" is a string not a number');
}

try{
  as.f({b: 'Mack'}).applyDefaults();
  xju.assertNeverReached('class');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify { b:"Mack" } is a Fred because failed to validate property a because property a is missing');
}
