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
/// <reference path="../xju/assert.ts"/>
/// <reference path="../xju/json_codec.ts"/>

xju.assert.assertEqual(xju.json_codec.applyNoDefaults.applyDefaults(), true);

xju.assert.assertEqual((xju.json_codec.isInstanceOfString('Fred') as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assert.assertEqual(xju.json_codec.isInstanceOfString(7), false);

xju.json_codec.asInstanceOfString('Surname').f('Fred').applyDefaults();
try{
  xju.json_codec.asInstanceOfString('Surname').f(7).applyDefaults();
  xju.assert.assertNeverReached('Surname');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify 7 is a Surname because 7 is a number not a string');
}

xju.assert.assertEqual(xju.json_codec.isKeyOfString('Fred'), true);
xju.assert.assertEqual(xju.json_codec.isKeyOfString(7), false);

xju.json_codec.asKeyOfString('Surname').f('Fred');
try{
  xju.json_codec.asKeyOfString('Surname').f(7);
  xju.assert.assertNeverReached('Surname');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify 7 is a keyof Surname because 7 is a number not a string');
}

xju.assert.assertEqual((xju.json_codec.isInstanceOfStringPattern(/^[A-Z]/)('Fred') as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assert.assertEqual(xju.json_codec.isInstanceOfStringPattern(/^[A-Z]/)(7), false);
xju.assert.assertEqual(xju.json_codec.isInstanceOfStringPattern(/^\\S*$/)(7), false);

xju.json_codec.asInstanceOfStringPattern('Surname', /^[A-Z]/).f('Fred').applyDefaults();
try{
  xju.json_codec.asInstanceOfStringPattern('Surname', /^[A-Z]/).f('fred').applyDefaults();
  xju.assert.assertNeverReached('Surname');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify "fred" is a Surname because "fred" is not a string with pattern /^[A-Z]/');
}
try{
  xju.json_codec.asInstanceOfStringPattern('NoSpace',new RegExp("^\\S*$")).f(7).applyDefaults();
  xju.assert.assertNeverReached('NoSpace');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify 7 is a NoSpace because failed to verify 7 is a string because 7 is a number not a string');
}

xju.assert.assertEqual(xju.json_codec.isKeyOfStringPattern(/^[A-Z]/)('Fred'), true);
xju.assert.assertEqual(xju.json_codec.isKeyOfStringPattern(/^[A-Z]/)('fred'), false);
xju.assert.assertEqual(xju.json_codec.isKeyOfStringPattern(/^[A-Z]/)(7), false);

xju.json_codec.asKeyOfStringPattern('Surname', /^[A-Z]/).f('Fred');
try{
  xju.json_codec.asKeyOfStringPattern('Surname', /^[A-Z]/).f('fred');
  xju.assert.assertNeverReached('Surname');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify "fred" is a keyof Surname because "fred" is not a string with pattern /^[A-Z]/');
}


xju.assert.assertEqual((xju.json_codec.isInstanceOfFloat(8.2) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assert.assertEqual(xju.json_codec.isInstanceOfFloat([]), false);

xju.json_codec.asInstanceOfFloat('Metres').f(8.2).applyDefaults();
try{
  xju.json_codec.asInstanceOfFloat('Metres').f([]).applyDefaults();
  xju.assert.assertNeverReached('Metres');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify [] is a Metres because [] is a object not a number');
}

xju.assert.assertEqual(xju.json_codec.isKeyOfFloat("8.2"), true);
xju.assert.assertEqual(xju.json_codec.isKeyOfFloat(8.2), false);
xju.assert.assertEqual(xju.json_codec.isKeyOfFloat([]), false);

xju.json_codec.asKeyOfFloat('Metres').f("8.2");
try{
  xju.json_codec.asKeyOfFloat('Metres').f("fred");
  xju.assert.assertNeverReached('Metres');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify "fred" is a keyof Metres because "fred" does not look like a number');
}
try{
  xju.json_codec.asKeyOfFloat('Metres').f([]);
  xju.assert.assertNeverReached('Metres');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify [] is a keyof Metres because [] is a object not a string');
}

xju.assert.assertEqual((xju.json_codec.isInstanceOfInt(8) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assert.assertEqual(xju.json_codec.isInstanceOfInt(8.2), false);

xju.json_codec.asInstanceOfInt('ByteCount').f(8).applyDefaults();
try{
  xju.json_codec.asInstanceOfInt('ByteCount').f(8.2).applyDefaults();
  xju.assert.assertNeverReached('ByteCount');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify 8.2 is a ByteCount because 8.2 is not a whole number');
}

xju.assert.assertEqual(xju.json_codec.isKeyOfInt("8"), true);
xju.assert.assertEqual(xju.json_codec.isKeyOfInt("8.2"), false);
xju.assert.assertEqual(xju.json_codec.isKeyOfInt(8), false);

xju.json_codec.asKeyOfInt('ByteCount').f("8");
try{
  xju.json_codec.asKeyOfInt('ByteCount').f("8.2");
  xju.assert.assertNeverReached('ByteCount');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify "8.2" is a keyof ByteCount because 8.2 is not a whole number');
}

xju.assert.assertEqual((xju.json_codec.isInstanceOfBool(true) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assert.assertEqual(xju.json_codec.isInstanceOfBool(8.2), false);

xju.json_codec.asInstanceOfBool('Enabled').f(true).applyDefaults();
try{
  xju.json_codec.asInstanceOfBool('Enabled').f(8.2).applyDefaults();
  xju.assert.assertNeverReached('Enabled');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify 8.2 is a Enabled because 8.2 is a number not a boolean');
}

xju.assert.assertEqual(xju.json_codec.isKeyOfBool("true"), true);
xju.assert.assertEqual(xju.json_codec.isKeyOfBool(8.2), false);
xju.assert.assertEqual(xju.json_codec.isKeyOfBool(false), false);

xju.json_codec.asKeyOfBool('Enabled').f("true");
try{
  xju.json_codec.asKeyOfBool('Enabled').f(8.2);
  xju.assert.assertNeverReached('Enabled');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify 8.2 is a keyof Enabled because 8.2 is a number not a string');
}
try{
  xju.json_codec.asKeyOfBool('Enabled').f("fred");
  xju.assert.assertNeverReached('Enabled');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify "fred" is a keyof Enabled because "fred" is not "true" or "false"');
}

// null
xju.assert.assertEqual((xju.json_codec.isInstanceOfNull(null) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assert.assertEqual(xju.json_codec.isInstanceOfNull(8.2), false);

xju.json_codec.asInstanceOfNull.f(null).applyDefaults();
try{
  xju.json_codec.asInstanceOfNull.f(8.2).applyDefaults();
  xju.assert.assertNeverReached(null);
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify 8.2 is a null because 8.2 is not null');
}

xju.assert.assertEqual(xju.json_codec.isKeyOfNull("null"), true);
xju.assert.assertEqual(xju.json_codec.isKeyOfNull(8.2), false);
xju.assert.assertEqual(xju.json_codec.isKeyOfNull(null), false);

xju.json_codec.asKeyOfNull.f("null");
try{
  xju.json_codec.asKeyOfNull.f("8.2");
  xju.assert.assertNeverReached(null);
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify "8.2" is a keyof null because "8.2" is not "null"');
}
try{
  xju.json_codec.asKeyOfNull.f(8.2);
  xju.assert.assertNeverReached(null);
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify 8.2 is a keyof null because 8.2 is a number not a string');
}

// literal
var is=xju.json_codec.isInstanceOfLiteral('fred');
xju.assert.assertEqual((is('fred') as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assert.assertEqual(is(8.2), false);

var isKey=xju.json_codec.isKeyOfLiteral('fred');
xju.assert.assertEqual(isKey('fred'),true);
xju.assert.assertEqual(isKey('jock'),false);
xju.assert.assertEqual(isKey(8.2),false);

var as=xju.json_codec.asInstanceOfLiteral('fred');
as.f('fred').applyDefaults();
try{
  as.f(8.2);
  xju.assert.assertNeverReached('literal');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify 8.2 is a "fred" because 8.2 != "fred"');
}

var asKey=xju.json_codec.asKeyOfLiteral('fred');
asKey.f('fred');
try{
  asKey.f('jock');
  xju.assert.assertNeverReached('literal');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify "jock" is a keyof "fred" because "jock" != "fred"');
}
try{
  asKey.f(8);
  xju.assert.assertNeverReached('literal');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify 8 is a keyof "fred" because 8 is not a string');
}

var is=xju.json_codec.isInstanceOfLiteral(17);
xju.assert.assertEqual((is(17) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assert.assertEqual(is("8.2"), false);

var isKey=xju.json_codec.isKeyOfLiteral(17);
xju.assert.assertEqual(isKey("17"),true);
xju.assert.assertEqual(isKey('jock'),false);
xju.assert.assertEqual(isKey(8.2),false);

var as=xju.json_codec.asInstanceOfLiteral(17);
as.f(17).applyDefaults();
try{
  as.f(8.2);
  xju.assert.assertNeverReached('literal');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify 8.2 is a 17 because 8.2 != 17');
}

var asKey=xju.json_codec.asKeyOfLiteral(17);
asKey.f('17');
try{
  asKey.f('jock');
  xju.assert.assertNeverReached('literal');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify "jock" is a keyof 17 because "jock" is not the JSON representation of 17');
}

// any
xju.assert.assertEqual((xju.json_codec.isInstanceOfAny(8) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.json_codec.asInstanceOfAny().f(8).applyDefaults();

// any list
xju.assert.assertEqual((xju.json_codec.isInstanceOfAnyList([1,'fred',null]) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assert.assertEqual(xju.json_codec.isInstanceOfAnyList(8.2), false);

xju.json_codec.asInstanceOfAnyList.f([1,'fred',null]).applyDefaults();
try{
  xju.json_codec.asInstanceOfAnyList.f(8.2).applyDefaults();
  xju.assert.assertNeverReached('[]');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify 8.2 is a Array because 8.2 is not an array it is a number');
}

// list
xju.assert.assertEqual((xju.json_codec.isInstanceOfList(xju.json_codec.isInstanceOfInt)([1,2,3]) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assert.assertEqual(xju.json_codec.isInstanceOfList(xju.json_codec.isInstanceOfInt)([1,'fred']), false);
xju.assert.assertEqual(xju.json_codec.isInstanceOfList(xju.json_codec.isInstanceOfInt)({}), false);

xju.json_codec.asInstanceOfList(xju.json_codec.asInstanceOfInt('int')).f([1,2,3]).applyDefaults();
try{
  xju.json_codec.asInstanceOfList(xju.json_codec.asInstanceOfInt('int')).f([1,'fred']).applyDefaults();
  xju.assert.assertNeverReached('int[]');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify [1,"fred"] is a Array<int> because failed to validate item 1 because failed to verify "fred" is a int because "fred" is a string not a number');
}
try{
  xju.json_codec.asInstanceOfList(xju.json_codec.asInstanceOfInt('ByteCount')).f({}).applyDefaults();
  xju.assert.assertNeverReached('int[]');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify { } is a Array<ByteCount> because { } is not an array it is a object');
}

// bytes
xju.assert.assertEqual((xju.json_codec.isInstanceOfBytes([0,1,2,3,255]) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assert.assertEqual(xju.json_codec.isInstanceOfBytes([1,-1]), false);
xju.assert.assertEqual(xju.json_codec.isInstanceOfBytes([256,0]), false);
xju.assert.assertEqual(xju.json_codec.isInstanceOfBytes([1,'fred']), false);
xju.assert.assertEqual(xju.json_codec.isInstanceOfBytes({}), false);

xju.json_codec.asInstanceOfBytes.f([0,1,2,3,255]).applyDefaults();
try{
  xju.json_codec.asInstanceOfBytes.f([-1,0,1,2,3,255]).applyDefaults();
  xju.assert.assertNeverReached('bytes');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify [-1,0,1,2,3,255] is a bytes because failed to validate item 0 because -1 is not in range 0..255');
}
try{
  xju.json_codec.asInstanceOfBytes.f([,0,1,2,3,255,256]).applyDefaults();
  xju.assert.assertNeverReached('bytes');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify [undefined,0,1,2,3,255,256] is a bytes because failed to validate item 6 because 256 is not in range 0..255');
}
try{
  xju.json_codec.asInstanceOfBytes.f({}).applyDefaults();
  xju.assert.assertNeverReached('bytes');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify { } is a bytes because failed to verify { } is a Array<byte> because { } is not an array it is a object');
}

// union
var is=xju.json_codec.isInstanceOfUnion([
  xju.json_codec.isInstanceOfInt,
  xju.json_codec.isInstanceOfString
]);
xju.assert.assertEqual((is(1) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assert.assertEqual((is('fred') as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assert.assertEqual(is([1]), false);

var isKey=xju.json_codec.isKeyOfUnion([
  xju.json_codec.isKeyOfInt,
  xju.json_codec.isKeyOfString
]);
xju.assert.assertEqual(isKey("1"), true);
xju.assert.assertEqual(isKey('fred'), true);
xju.assert.assertEqual(isKey([1]), false);

var as=xju.json_codec.asInstanceOfUnion([
  xju.json_codec.asInstanceOfInt('int'),
  xju.json_codec.asInstanceOfString('Surname')
]);
as.f(1).applyDefaults();
as.f('fred').applyDefaults();
try{
  as.f([1]).applyDefaults();
  xju.assert.assertNeverReached('Array<int|string>');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify [1] is a int|Surname because failed to verify [1] is a int because [1] is a object not a number and failed to verify [1] is a Surname because [1] is a object not a string');
}

var asKey=xju.json_codec.asKeyOfUnion([
  xju.json_codec.asKeyOfInt('int'),
  xju.json_codec.asKeyOfBool('Enabled')
]);
asKey.f('1');
asKey.f('true');
try{
  asKey.f([1]);
  xju.assert.assertNeverReached('Array<int|string>');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify [1] is a keyof int|Enabled because failed to verify [1] is a keyof int because [1] is a object not a string and failed to verify [1] is a keyof Enabled because [1] is a object not a string');
}

// dict
var is=xju.json_codec.isInstanceOfDict(
  xju.json_codec.isKeyOfInt,
  xju.json_codec.isInstanceOfString
);
xju.assert.assertEqual((is({"1": "Blan"}) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assert.assertEqual(is({"1": 2}), false);
xju.assert.assertEqual(is({"fred": "Blan"}), false);

var as=xju.json_codec.asInstanceOfDict(
  xju.json_codec.asKeyOfInt('int'),
  xju.json_codec.asInstanceOfString('Surname')
);
as.f({"1": "Blan"}).applyDefaults();
try{
  as.f(null).applyDefaults();
  xju.assert.assertNeverReached('dict[int,str]');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify null is a { [key int] : Surname } because null is not a non-array object');
}
try{
  as.f([2]).applyDefaults();
  xju.assert.assertNeverReached('dict[int,str]');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify [2] is a { [key int] : Surname } because [2] is not a non-array object');
}
try{
  as.f({"1":2}).applyDefaults();
  xju.assert.assertNeverReached('dict[int,str]');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify { 1:2 } is a { [key int] : Surname } because failed to validate property 1 because failed to verify 2 is a Surname because 2 is a number not a string');
}
try{
  as.f({"fred":"Blan"}).applyDefaults();
  xju.assert.assertNeverReached('dict[int,str]');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify { fred:"Blan" } is a { [key int] : Surname } because failed to validate property fred because failed to verify "fred" is a keyof int because "fred" does not look like a number');
}

// dict[any,any]
var is=xju.json_codec.isInstanceOfAnyDict;
xju.assert.assertEqual((is({"1": "Blan"}) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assert.assertEqual(is(null), false);
xju.assert.assertEqual(is([]), false);

var as=xju.json_codec.asInstanceOfAnyDict;
as.f({"1": "Blan"}).applyDefaults();
try{
  as.f(null).applyDefaults();
  xju.assert.assertNeverReached('dict[int,str]');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify null is a { [key any] : any } because null is not a non-array object');
}
try{
  as.f([2]).applyDefaults();
  xju.assert.assertNeverReached('dict[int,str]');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify [2] is a { [key any] : any } because [2] is not a non-array object');
}

// tuple
var is=xju.json_codec.isInstanceOfTuple([
  xju.json_codec.isInstanceOfInt,
  xju.json_codec.isInstanceOfString
]);
xju.assert.assertEqual((is([1,'fred']) as xju.json_codec.ApplyDefaults).applyDefaults(), true);
xju.assert.assertEqual(is({}), false);
xju.assert.assertEqual(is([1]), false);
xju.assert.assertEqual(is([1,'fred',2]), false);
xju.assert.assertEqual(is([1, 8.2]), false);

var as=xju.json_codec.asInstanceOfTuple([
  xju.json_codec.asInstanceOfInt('int'),
  xju.json_codec.asInstanceOfString('Surname')
]);
as.f([1,'fred']).applyDefaults();
try{
  as.f({}).applyDefaults();
  xju.assert.assertNeverReached('[int,string]');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify { } is a [int,Surname] because not an array it is a object');
}
try{
  as.f([1]).applyDefaults();
  xju.assert.assertNeverReached('[int,string]');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify [1] is a [int,Surname] because length is 1 not 2');
}
as.f([1,'fred']).applyDefaults();
try{
  as.f([1, 8.2]).applyDefaults();
  xju.assert.assertNeverReached('[int,string]');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify [1,8.2] is a [int,Surname] because failed to validate item 1 because failed to verify 8.2 is a Surname because 8.2 is a number not a string');
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
  xju.assert.assertEqual(x, {a:2, b:{c:'Betty'}});
}
{
  const x: any = {a: 2, b:{}};
  (is(x) as xju.json_codec.ApplyDefaults).applyDefaults();
  xju.assert.assertEqual(x, {a:2, b:{c:'Mack'}});
}
{
  const x: any = {a: 2, b:{c: 'Al'}};
  (is(x) as xju.json_codec.ApplyDefaults).applyDefaults();
  xju.assert.assertEqual(x, {a:2, b:{c:'Al'}});
}

xju.assert.assertEqual(is(9), false);
xju.assert.assertEqual(is([1]), false);
xju.assert.assertEqual(is({a: 'fred', b: 'Mack'}), false);
xju.assert.assertEqual(is({b: 'Mack'}), false);

var as=xju.json_codec.asInstanceOfClass('Fred',[
  {
    propertyName: 'a',
    asInstance: xju.json_codec.asInstanceOfInt('int')
  },
  {
    propertyName: 'b',
    asInstance: xju.json_codec.asInstanceOfClass('Fred',[
      {
        propertyName: 'c',
        asInstance: xju.json_codec.asInstanceOfString('Surname'),
        defaultValue: 'Mack'
      }]),
    defaultValue: {'c': 'Betty'}
  }
]);

{
  const x: any = {a: 2};
  as.f(x).applyDefaults();
  xju.assert.assertEqual(x, {a:2, b:{c:'Betty'}});
}
{
  const x: any = {a: 2, b:{}};
  as.f(x).applyDefaults();
  xju.assert.assertEqual(x, {a:2, b:{c:'Mack'}});
}
{
  const x: any = {a: 2, b:{c:'Jones'}};
  as.f(x).applyDefaults();
  xju.assert.assertEqual(x, {a:2, b:{c:'Jones'}});
}

try{
  as.f(9).applyDefaults();
  xju.assert.assertNeverReached('class');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify 9 is a Fred because 9 is not a non-array object');
}

try{
  as.f([1]).applyDefaults();
  xju.assert.assertNeverReached('class');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify [1] is a Fred because [1] is not a non-array object');
}

try{
  as.f({a: 'fred', b: 'Mack'}).applyDefaults();
  xju.assert.assertNeverReached('class');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify { a:"fred", b:"Mack" } is a Fred because failed to validate property a because failed to verify "fred" is a int because "fred" is a string not a number');
}

try{
  as.f({b: {}}).applyDefaults();
  xju.assert.assertNeverReached('class');
}
catch(e){
  xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify { b:{ } } is a Fred because failed to validate property a because property a is missing');
}
