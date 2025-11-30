/// <reference path="xju.ts" />
/// <reference path="xju/assert.ts" />

xju.assert.assertEqual(xju.str(99), "99");
xju.assert.assertTrue(xju.isNonArrayObject({}));
xju.assert.assertFalse(xju.isNonArrayObject([]));
xju.assert.assertFalse(xju.isNonArrayObject(null));
xju.assert.assertFalse(xju.isNonArrayObject(7));
xju.assert.assertFalse(xju.isNonArrayObject("fred"));
xju.assert.assertEqual(xju.format_(99), "99");
xju.assert.assertEqual(xju.format_([1,2]), "[1,2]");
xju.assert.assertEqual(xju.format_({a:1,b:2}), "{ a:1, b:2 }");
let x = {} as any;
xju.assert.assertEqual(xju.format_(x.b), 'undefined');
