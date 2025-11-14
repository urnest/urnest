export {};

import {
  xju
} from "xju.js";

xju.assertXcontainsY('fred', 're');
xju.assertEqual(1,1);
try {
  xju.assertEqual(1,2);
  throw new Error('1==2?');
}
catch(e:any){
  xju.assertXcontainsY(`${e}`, '1 != 2');
}
xju.assertNotEqual(1,2);
try {
  xju.assertNotEqual(1,1);
  throw new Error('1!=1?');
}
catch(e:any){
  xju.assertXcontainsY(`${e}`, '1 == 1');
}

xju.assertEqual([1,2],[1,2]);
try {
  xju.assertEqual([1,2],[1,3]);
  throw new Error('[1,2]==[1,3]?');
}
catch(e:any){
  xju.assertXcontainsY(`${e}`, 'Error: array [1,2] != array [1,3] because Error: array element 1 2 != 3');
}
try {
  xju.assertEqual([1,2],[1]);
  throw new Error('[1,2]==[1]?');
}
catch(e:any){
  xju.assertXcontainsY(`${e}`, 'Error: array [1,2] != array [1] because Error: arrays of different length because Error: 2 != 1');
}

xju.assertEqual({a:1,b:2},{a:1,b:2});
try {
  xju.assertEqual({a:1,b:2},{a:1,b:3});
  throw new Error('{a:1,b:2}=={a:1,b:3}?');
}
catch(e:any){
  xju.assertXcontainsY(`${e}`, 'Error: object { a:1, b:2 } != object { a:1, b:3 } because Error: property b is not equal because Error: 2 != 3');
}
try {
  xju.assertEqual({a:1,b:2},{a:1});
  throw new Error('{a:1,b:2}=={a:1}?');
}
catch(e:any){
  xju.assertXcontainsY(`${e}`, 'Error: object { a:1, b:2 } != object { a:1 } because Error: array ["a","b"] != array ["a"] because Error: arrays of different length because Error: 2 != 1');
}
function get(o:any, a:string) {
  return o[a];
}
xju.assertEqual(xju.format_(get({},'c')),'undefined');
xju.assertEqual(xju.format_([1,2]),"[1,2]");
xju.assertEqual(xju.format_({a:3,b:4}),"{ a:3, b:4 }");
xju.assertEqual(xju.format_('fred'),'"fred"');
xju.assertTrue(true);
try {
  xju.assertTrue(false);
  throw new Error('false is true?');
}
catch(e:any){
  xju.assertXcontainsY(`${e}`, '"false" is false, expected true');
}
xju.assertFalse(false);
try {
  xju.assertFalse(true);
  throw new Error('true is false?');
}
catch(e:any){
  xju.assertXcontainsY(`${e}`, '"true" is true, expected false');
}
try{
  xju.assertXcontainsY('fred','jock');
  throw new Error('fred contains jock?');
}
catch(e:any){
  xju.assertXcontainsY(`${e}`, 'Error: "fred" does not contain "jock"');
}
  
try{
  xju.assertNeverReached('fred');
  throw new Error('reached?');
}
catch(e:any){
  xju.assertXcontainsY(`${e}`, 'Error: should not be here with "fred"');
}
