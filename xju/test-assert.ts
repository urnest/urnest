/// <reference path="../xju/assert.ts" />

xju.assert.assertEqual(xju.str(8), "8");
xju.assert.assertXcontainsY('fred', 're');
xju.assert.assertEqual(1,1);
try {
  xju.assert.assertEqual(1,2);
  throw new Error('1==2?');
}
catch(e:any){
  xju.assert.assertXcontainsY(`${e}`, '1 != 2');
}
xju.assert.assertNotEqual(1,2);
try {
  xju.assert.assertNotEqual(1,1);
  throw new Error('1!=1?');
}
catch(e:any){
  xju.assert.assertXcontainsY(`${e}`, '1 == 1');
}

xju.assert.assertEqual([1,2],[1,2]);
try {
  xju.assert.assertEqual([1,2],[1,3]);
  throw new Error('[1,2]==[1,3]?');
}
catch(e:any){
  xju.assert.assertXcontainsY(`${e}`, 'Error: failed to verify array [1,2] == array [1,3] because array element 1 2 != 3');
}
try {
  xju.assert.assertEqual([1,2],[1]);
  throw new Error('[1,2]==[1]?');
}
catch(e:any){
  xju.assert.assertXcontainsY(`${e}`, 'Error: failed to verify array [1,2] == array [1] because failed to verify arrays have same length because 2 != 1');
}

xju.assert.assertEqual({a:1,b:2},{a:1,b:2});
try {
  xju.assert.assertEqual({a:1,b:2},{a:1,b:3});
  throw new Error('{a:1,b:2}=={a:1,b:3}?');
}
catch(e:any){
  xju.assert.assertXcontainsY(`${e}`, 'Error: failed to verify object { a:1, b:2 } == object { a:1, b:3 } because failed to verify property b equal because 2 != 3');
}
try {
  xju.assert.assertEqual({a:1,b:2},{a:1});
  throw new Error('{a:1,b:2}=={a:1}?');
}
catch(e:any){
  xju.assert.assertXcontainsY(`${e}`, 'Error: failed to verify object { a:1, b:2 } == object { a:1 } because failed to verify array ["a","b"] == array ["a"] because failed to verify arrays have same length because 2 != 1');
}
function get(o:any, a:string) {
  return o[a];
}
xju.assert.assertEqual(xju.format_(get({},'c')),'undefined');
xju.assert.assertEqual(xju.format_([1,2]),"[1,2]");
xju.assert.assertEqual(xju.format_({a:3,b:4}),"{ a:3, b:4 }");
xju.assert.assertEqual(xju.format_('fred'),'"fred"');
xju.assert.assertTrue(true);
try {
  xju.assert.assertTrue(false);
  throw new Error('false is true?');
}
catch(e:any){
  xju.assert.assertXcontainsY(`${e}`, '"false" is false, expected true');
}
xju.assert.assertFalse(false);
try {
  xju.assert.assertFalse(true);
  throw new Error('true is false?');
}
catch(e:any){
  xju.assert.assertXcontainsY(`${e}`, '"true" is true, expected false');
}
try{
  xju.assert.assertXcontainsY('fred','jock');
  throw new Error('fred contains jock?');
}
catch(e:any){
  xju.assert.assertXcontainsY(`${e}`, 'Error: "fred" does not contain "jock"');
}

xju.assert.assertEqual(xju.assert.assertNumberInRange(7, 7, 10),7);
xju.assert.assertEqual(xju.assert.assertNumberInRange(10, 7, 10),10);
try{
  xju.assert.assertNumberInRange(6, 7, 10);
  throw new Error('6 in 7..10?');
}
catch(e:any){
  xju.assert.assertXcontainsY(`${e}`, 'Error: 6 is not in range 7..10');
}
try{
  xju.assert.assertNumberInRange(10.1, 7, 10);
  throw new Error('10.1 in 7..10?');
}
catch(e:any){
  xju.assert.assertXcontainsY(`${e}`, 'Error: 10.1 is not in range 7..10');
}

xju.assert.assertEqual(xju.assert.assertMatches(new RegExp("^a*$"), "aaaa"), ["aaaa"]);
try{
  xju.assert.assertMatches(new RegExp("^a*$"), "aaab");
  throw new Error('aaab like aaaa...?');
}
catch(e:any){
  xju.assert.assertXcontainsY(`${e}`, 'Error: "aaab" does not match /^a*$/');
}

try{
  xju.assert.assertNeverReached('fred');
  throw new Error('reached?');
}
catch(e:any){
  xju.assert.assertXcontainsY(`${e}`, 'Error: should not be here with "fred"');
}
