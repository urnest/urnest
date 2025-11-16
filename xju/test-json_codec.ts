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
xju.json_codec.asInstanceOfString('Surname').f('Fred').applyDefaults();
try{
  xju.json_codec.asInstanceOfString('Surname').f(7).applyDefaults();
  xju.assertNeverReached('Surname');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify 7 is a Surname because 7 is a number not a string');
}

xju.json_codec.asInstanceOfStringPattern('Surname', /^[A-Z]/).f('Fred').applyDefaults();
try{
  xju.json_codec.asInstanceOfStringPattern('Surname', /^[A-Z]/).f('fred').applyDefaults();
  xju.assertNeverReached('Surname');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify "fred" is a Surname because "fred" is not a string with pattern /^[A-Z]/');
}

xju.json_codec.asInstanceOfFloat('Metres').f(8.2).applyDefaults();
try{
  xju.json_codec.asInstanceOfFloat('Metres').f([]).applyDefaults();
  xju.assertNeverReached('Metres');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify [] is a Metres because [] is a object not a number');
}

xju.json_codec.asInstanceOfInt('ByteCount').f(8).applyDefaults();
try{
  xju.json_codec.asInstanceOfInt('ByteCount').f(8.2).applyDefaults();
  xju.assertNeverReached('ByteCount');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify 8.2 is a ByteCount because 8.2 is not a whole number');
}

xju.json_codec.asInstanceOfBool('Enabled').f(true).applyDefaults();
try{
  xju.json_codec.asInstanceOfBool('Enabled').f(8.2).applyDefaults();
  xju.assertNeverReached('Enabled');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify 8.2 is a Enabled because 8.2 is a number not a boolean');
}

xju.json_codec.asInstanceOfNull().f(null).applyDefaults();
try{
  xju.json_codec.asInstanceOfNull().f(8.2).applyDefaults();
  xju.assertNeverReached(null);
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify 8.2 is a null because 8.2 is not null');
}

xju.json_codec.asInstanceOfAny().f(8).applyDefaults();

xju.json_codec.asInstanceOfAnyList().f([1,'fred',null]).applyDefaults();
try{
  xju.json_codec.asInstanceOfAnyList().f(8.2).applyDefaults();
  xju.assertNeverReached('[]');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify 8.2 is a Array because 8.2 is not an array it is a number');
}

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

var as=xju.json_codec.asInstanceOfClass([
  {
    propertyName: 'a',
    asInstance: xju.json_codec.asInstanceOfInt('int')
  },
  {
    propertyName: 'b',
    asInstance: xju.json_codec.asInstanceOfString('Surname'),
    defaultValue: 'Mack'
  }
]);

{
  const x: any = {a: 2};
  as.f(x).applyDefaults();
  xju.assertEqual(x, {a:2, b:'Mack'});
}
{
  const x: any = {a: 2, b:'Jones'};
  as.f(x).applyDefaults();
  xju.assertEqual(x, {a:2, b:'Jones'});
}

try{
  as.f(9).applyDefaults();
  xju.assertNeverReached('class');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify 9 is a { a: int; b: Surname } because not an object it is a number');
}

try{
  as.f([1]).applyDefaults();
  xju.assertNeverReached('class');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify [1] is a { a: int; b: Surname } because not an object it is an array');
}

try{
  as.f({a: 'fred', b: 'Mack'}).applyDefaults();
  xju.assertNeverReached('class');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify { a:"fred", b:"Mack" } is a { a: int; b: Surname } because failed to validate property a because failed to verify "fred" is a int because "fred" is a string not a number');
}

try{
  as.f({b: 'Mack'}).applyDefaults();
  xju.assertNeverReached('class');
}
catch(e){
  xju.assertXcontainsY(xju.str(e), 'Error: failed to verify { b:"Mack" } is a { a: int; b: Surname } because failed to validate property a because property a is missing');
}
