export {};

import {
  isInstanceOfEncodableData,
  asInstanceOfEncodableKeyTypes,
  isInstanceOfEncodableKeyTypes,
  asInstanceOfLias,
  isInstanceOfLias,
  asInstanceOfLiasee,
  isInstanceOfLiasee,
  asInstanceOfLiaser,
  isInstanceOfLiaser,
  asInstanceOfEncodableData,
  asInstanceOfSurname,
  isInstanceOfOr,
  asInstanceOfOr,
  isInstanceOfAnd,
  asInstanceOfAnd,
  xju,
} from "./json_codec_typescript_test.js";

function str(x:any):string { return `${x}`; }
function isObject(x:any):x is object { return x !== null && typeof(x) === 'object'; }
function assert_not_equal<T>(x:T, y:T){
    try{
        xju.ts.assertEqual(x,y);
    }
    catch(e){
        return;
    }
    throw new Error(`${format_(x)} == ${format_(y)}`);
}
function format_<T>(a:T): string {
    if (Array.isArray(a)) return format_array(a);
    if (isObject(a)) return format_object(a);
    return JSON.stringify(a);
}
function format_array<T>(a:Array<T>): string {
    if(a.length==0) return "[]";
    if(a.length==1) return `[${format_(a[0])}]`;
    var result=`${format_(a[0])}`;
    for(var i=1;i!=a.length;++i) result=result+`,${format_(a[i])}`;
    return `[${result}]`;
}
function format_object<T>(a:T): string {
    var result="{";
    for(const k in a) result=result+` ${k}:${format_(a[k])},`;
    return result+"}";
}

function assert_arrays_equal<T>(x:Array<T>, y:Array<T>){
    try{
        try{
            xju.ts.assertEqual(x.length, y.length);
        }
        catch(e:any){
            throw new Error(`arrays of different length because ${e}`);
        }
        var i;
        for(i=0; i!=x.length; ++i){
            try{
                xju.ts.assertEqual<T>(x[i],y[i]);
            }
            catch(e:any){
                throw new Error(`array element ${i} ${format_(x[i])} != ${format_(y[i])}`);
            }
        }
    }
    catch(e:any){
        throw new Error(`array ${format_(x)} != array ${format_(y)} because ${e}`);
    }
}
function assert_objects_equal(x:object, y:object){
    try{
        var xKeys=new Array<keyof object>();
        var yKeys=new Array<keyof object>();
        for(const k in x){if (x.hasOwnProperty(k)) xKeys.push(k as keyof object);}
        for(const k in y){if (y.hasOwnProperty(k)) yKeys.push(k as keyof object);}
        assert_arrays_equal(xKeys,yKeys);
        for(var i=0; i!=xKeys.length; ++i){
            const k = xKeys[i];
            try{
                xju.ts.assertEqual(x[k],y[k]);
            }
            catch(e){
                throw new Error(`property ${k} is not equal because ${e}`);
            }
        }
    }
    catch(e){
        throw new Error(`object ${format_(x)} != object ${format_(y)} because ${e}`);
    }
}
function assert_x_contains_y(x:string, y:string){
    if (x.indexOf(y)==-1){
        throw new Error(`"${x}" does not contain "${y}"`)
    }
}
function assert_true(x:boolean){
    if (!x){
        throw new Error(`"${x}" is false, expected true`);
    }
}
function assert_false(x:boolean){
    if (x){
        throw new Error(`"${x}" is true, expected false`);
    }
}
function assert_never_reached(){
   throw new Error(`should not be here`);
}


//encodableData
(() => {
    const good = () => ({
        an_int: 7,
        a_float: 9.2,
        a_str: 'fred',
        a_new_int: 22,
        an_xju_int: 88,
        a_new_float: 2.2,
        an_xju_float: 18.2,
        a_new_str: 'red',
        an_xju_str: "ally",
        a_null: null,
        a_boolean: true,
        a_union: 'walker',
        a_list: [1,2,3],
        any_list: [1,null,'a'],
        a_set: [4,5,6],
        any_set: [null, 2],
        a_frozen_set: [7],
        any_frozen_set: [2, 4.6], // python reorders
        some_bytes: [10,11,12],
        a_tuple: [8, 'jock'] as [number, string],
        a_literal_str: 'mai',
        a_literal_int: 7,
        a_literal_bool: true,
        a_class: {first_name:"fran",middle_names:["jan"],last_name:"lan",class:"Upper"},
        a_dont_encode:{y:'yyy'},
        fred_7_false:false,
        a_recurse_self:{defs:{}},
        a_timestamp:45000,
        an_enum:'fred',
        a_mixed_in_enum:{k:2,v:'jock'},
        a_recursive:{"a": [{"o": ["1","2",[]]},"3", [{ "a": ["4","6",[]] }]]},
        an_enum_value:"fred",
        a_custom_encoded:2975337473,
        another_custom_encoded:"177.88.12.0/22"
    });

    xju.ts.assertEqual(asInstanceOfEncodableData({"an_int": 7, "a_float": 9.2, "a_str": "fred", "a_new_int": 22, "an_xju_int": 88, "a_new_float": 2.2, "an_xju_float": 18.2, "a_new_str": "red", "an_xju_str": "ally", "a_null": null, "a_boolean": true, "a_union": "walker", "a_list": [1, 2, 3], "any_list": [1, null, "a"], "a_set": [4, 5, 6], "any_set": [null, 2], "a_frozen_set": [7], "any_frozen_set": [2, 4.6], "some_bytes": [10, 11, 12], "a_tuple": [8, "jock"], "a_literal_str": "mai", "a_literal_int": 7, "a_literal_bool": true, "a_class": {"first_name": "fran", "middle_names": ["jan"], "last_name": "lan", "class": "Upper"}, "a_dont_encode": {"y": "yyy"}, "fred_7_false": false, "a_recurse_self": {"defs": {}}, "a_timestamp": 45000.0, "an_enum": "fred", "a_mixed_in_enum": {"k": 2, "v": "jock"}, "a_recursive": {"a": [{"o": ["1", "2", []]}, "3", [{"a": ["4", "6", []]}]]}, "an_enum_value": "fred", "a_custom_encoded": 2975337473, "another_custom_encoded": "177.88.12.0/22"}) as object, good());
    xju.ts.assertEqual(isInstanceOfEncodableData({"an_int": 7, "a_float": 9.2, "a_str": "fred", "a_new_int": 22, "an_xju_int": 88, "a_new_float": 2.2, "an_xju_float": 18.2, "a_new_str": "red", "an_xju_str": "ally", "a_null": null, "a_boolean": true, "a_union": "walker", "a_list": [1, 2, 3], "any_list": [1, null, "a"], "a_set": [4, 5, 6], "any_set": [null, 2], "a_frozen_set": [7], "any_frozen_set": [2, 4.6], "some_bytes": [10, 11, 12], "a_tuple": [8, "jock"], "a_literal_str": "mai", "a_literal_int": 7, "a_literal_bool": true, "a_class": {"first_name": "fran", "middle_names": ["jan"], "last_name": "lan", "class": "Upper"}, "a_dont_encode": {"y": "yyy"}, "fred_7_false": false, "a_recurse_self": {"defs": {}}, "a_timestamp": 45000.0, "an_enum": "fred", "a_mixed_in_enum": {"k": 2, "v": "jock"}, "a_recursive": {"a": [{"o": ["1", "2", []]}, "3", [{"a": ["4", "6", []]}]]}, "an_enum_value": "fred", "a_custom_encoded": 2975337473, "another_custom_encoded": "177.88.12.0/22"}), true);

    const errors:Array<string> = [];
    try {
        asInstanceOfEncodableData({
            ...good(),
            an_int: 7.6
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'Error: { an_int:7.6, a_float:9.2, a_str:"fred", a_new_int:22, an_xju_int:88, a_new_float:2.2, an_xju_float:18.2, a_new_str:"red", an_xju_str:"ally", a_null:null, a_boolean:true, a_union:"walker", a_list:[1,2,3], any_list:[1,null,"a"], a_set:[4,5,6], any_set:[null,2], a_frozen_set:[7], any_frozen_set:[2,4.6], some_bytes:[10,11,12], a_tuple:[8,"jock"], a_literal_str:"mai", a_literal_int:7, a_literal_bool:true, a_class:{ first_name:"fran", middle_names:["jan"], last_name:"lan", class:"Upper" }, a_dont_encode:{ y:"yyy" }, fred_7_false:false, a_recurse_self:{ defs:{ } }, a_timestamp:45000, an_enum:"fred", a_mixed_in_enum:{ k:2, v:"jock" }, a_recursive:{ a:[{ o:["1","2",[]] },"3",[{ a:["4","6",[]] }]] }, an_enum_value:"fred", a_custom_encoded:2975337473, another_custom_encoded:"177.88.12.0/22" }');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_float: null
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute a_float is invalid because Error: null is not a number it is a object');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_str: 77
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'is not a EncodableData because Error: attribute a_str is invalid because Error: 77 is not a string it is a number');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_new_int: []
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'is not a EncodableData because Error: attribute a_new_int is invalid because Error: [] is not a number it is a object');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            an_xju_int: []
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute an_xju_int is invalid because Error: [] is not a xju.misc.ByteCount i.e. a number, it is a object');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_new_float: []
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'is not a EncodableData because Error: attribute a_new_float is invalid because Error: [] is not a number it is a object');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            an_xju_float: []
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute an_xju_float is invalid because Error: [] is not a Metres i.e. a number, it is a object');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_new_str: { }
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'is not a EncodableData because Error: attribute a_new_str is invalid because Error: { } is not a string it is a object');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            an_xju_str: { }
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute an_xju_str is invalid because Error: { } is not a xju.json_codec.TypeScriptUQN i.e. a string, it is a object');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_null: { }
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'is not a EncodableData because Error: attribute a_null is invalid because Error: { } is not null it is a object');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_boolean: 99
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'is not a EncodableData because Error: attribute a_boolean is invalid because Error: 99 is not a boolean it is a number');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_union: 99
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute a_union is invalid because Error: object 99 is not a FullName|Surname|null because Error: 99 is not a FullName because Error: 99 is not an object it is a number and 99 is not a Surname i.e. a string, it is a number and 99 is not null it is a number');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_list: [null]
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'is not a EncodableData because Error: attribute a_list is invalid because Error: [null] is not a Array<number> because Error: item at index 0 is invalid because Error: null is not a number it is a object');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            any_list: null
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute any_list is invalid because Error: null is not an Array it is a object');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_set: [null]
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'is not a EncodableData because Error: attribute a_set is invalid because Error: [null] is not a Array<number> /* with unique elements */ because Error: item at index 0 is invalid because Error: null is not a number it is a object');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            any_set: { }
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute any_set is invalid because Error: [object Object] is not an Array<any> /* with unique elements */ it is a object');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_frozen_set: [null]
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'is not a EncodableData because Error: attribute a_frozen_set is invalid because Error: [null] is not a Array<number> /* with unique elements */ because Error: null is not a number it is a object');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_frozen_set: [null]
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute a_frozen_set is invalid because Error: [null] is not a Array<number> /* with unique elements */ because Error: null is not a number it is a object');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            some_bytes: [3000]
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'is not a EncodableData because Error: attribute some_bytes is invalid because Error: 3000 at index 0 is not in range 0..255');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_tuple: [3000,99]
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'is not a EncodableData because Error: attribute a_tuple is invalid because Error: [3000,99] is not a [number,string] because Error: 99 is not a string it is a number');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_literal_str: "kai"
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute a_literal_str is invalid because Error: object \"kai\" is not a \"ann\"|\"mai\" because Error: the string \"kai\" is not the string \"ann\" and the string \"kai\" is not the string \"mai\"');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_literal_int: 8
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'is not a EncodableData because Error: attribute a_literal_int is invalid because Error: the number 8 is not the number 7');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_literal_bool: false
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'is not a EncodableData because Error: attribute a_literal_bool is invalid because Error: the boolean false is not true');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_class: {first_name:8,middle_names:["jan"],last_name:"lan"}
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute first_name is invalid because Error: 8 is not a string it is a number');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            fred_7_false:'jan'
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute fred_7_false is invalid because Error: object \"jan\" is not a \"fred\"|7|false because Error: the string \"jan\" is not the string \"fred\" and the string \"jan\" is not the number 7 and the string \"jan\" is not false');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        const x = asInstanceOfEncodableData({
            ...good(),
            a_recurse_self:999
        });
        assert_not_equal(x,x);
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'a_recurse_self is invalid because Error: 999 is not a xju.json_codec.TypeScriptNamespace because Error: 999 is not an object it is a number');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        const x = asInstanceOfEncodableData({
            ...good(),
            a_recurse_self:{ 'defs': 1 }
        });
        assert_not_equal(x,x);
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute a_recurse_self is invalid because Error: { defs:1 } is not a xju.json_codec.TypeScriptNamespace because Error: attribute defs is invalid because Error: 1 is not a { [key: string /* xju.json_codec.TypeScriptUQN */]: xju.json_codec.TypeScriptSourceCode|xju.json_codec.TypeScriptNamespace } because Error: 1 is not an object it is a number');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        const x = asInstanceOfEncodableData({
            ...good(),
            a_recurse_self:{ 'defs': { 1: null } }
        });
        assert_not_equal(x,x);
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute defs is invalid because Error: { 1:null } is not a { [key: string /* xju.json_codec.TypeScriptUQN */]: xju.json_codec.TypeScriptSourceCode|xju.json_codec.TypeScriptNamespace } because Error: element \"1\" is invalid because Error: object null is not a xju.json_codec.TypeScriptSourceCode|xju.json_codec.TypeScriptNamespace because Error: null is not a xju.json_codec.TypeScriptSourceCode i.e. a string, it is a object and null is not a xju.json_codec.TypeScriptNamespace because Error: null is not an object it is null');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_timestamp:null
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'null is not a xju.time.Timestamp i.e. a number, it is a object');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            an_enum:7
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute an_enum is invalid because Error: 7 is not a O because Error: 7 != "fred" and 7 != "jock"');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_mixed_in_enum:{k:2,v:'brad'}
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute a_mixed_in_enum is invalid because Error: { k:2, v:"brad" } is not a MixedEnum because Error: object { k:2, v:"brad" } != object { k:1, v:"fred" } because Error: property k is not equal because Error: 2 != 1 and object { k:2, v:"brad" } != object { k:2, v:"jock" } because Error: property v is not equal because Error: "brad" != "jock"');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            an_enum_value:"paul"
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute an_enum_value is invalid because Error: "paul" != "fred"');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_recursive:[]
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute a_recursive is invalid because Error: object [] is not a string|And|Or because Error: [] is not a string it is a object and [] is not a And because Error: [] is an array and [] is not a Or because Error: [] is an array');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_recursive:{"a": [{"o": ["1","2",[]]},"3"]}
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute o is invalid because Error: undefined is not a [string|And|Or,string|And|Or,Array<string|And|Or>] because Error: undefined is not an array it is a undefined');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_recursive:{"a": [{"o": ["1","2",[]]},"3", [{ "a": ["6",[]] }]]}
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), '[\"6\",[]] does not have 3 elements (it has 2 elements)');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    xju.ts.assertEqual(errors, []);

    // misc coverage:
    xju.ts.assertEqual(asInstanceOfSurname("walker"), "walker");
    xju.ts.assertEqual(xju.json_codec.asInstanceOfTypeScriptNamespace({ defs:{ "a": "true;" } }), { defs:{ "a": "true;" } });
    xju.ts.assertEqual(xju.json_codec.isInstanceOfTypeScriptNamespace({ defs:{ "a": "true;" } }), true);
    xju.ts.assertEqual(xju.json_codec.asInstanceOfTypeScriptSourceCode("true;"), "true;");
    xju.ts.assertEqual(xju.json_codec.isInstanceOfTypeScriptSourceCode("true;"), true);
    xju.ts.assertEqual(xju.json_codec.asInstanceOfKeyOfTypeScriptSourceCode("true;"), "true;");
    xju.ts.assertEqual(xju.json_codec.isInstanceOfKeyOfTypeScriptSourceCode("true;"), true);
    xju.ts.assertEqual(xju.time.asInstanceOfTimestamp(8.8), 8.8);
    xju.ts.assertEqual(xju.time.isInstanceOfTimestamp(8.8), true);
    xju.ts.assertEqual(isInstanceOfOr({"o": ["1","2",[]]}),true);
    xju.ts.assertEqual(asInstanceOfOr({"o": ["1","2",[]]}),{"o": ["1","2",[]]});
    xju.ts.assertEqual(isInstanceOfAnd({ "a": ["4","6",[]] }),true);
    xju.ts.assertEqual(asInstanceOfAnd({ "a": ["4","6",[]] }),{ "a": ["4","6",[]] });
    
})();


//EncodableKeyTypes
(() => {
    const good = () => ({
        str_key:{ fred:3 },
        int_key:{ 1:4 },
        more_keys:{ 9:2, null:1, 5.5:3, true:4 },
        duration_key:{ 77.2:1 },
        timestamp_key:{ 88.1:1 },
        non_str_literal_key:{ 10:1, true:2 },
        str_literal_key:{ "a\n\"nail\"'s tip":3, fred:4, rooster:5 },
        enum_key:{ fred:1, jock:2 },
        custom_key:{ 3221751809:1 },
        alt_custom_key:{ "192.8.8.1/24":1 },
        newstr_key:{ red:1, green:2 },
        xjustr_key:{ ally: 9},
        newfloat_key:{ 22:1 },
        xjufloat_key:{ 22.5:1 },
        newint_key:{ 7:1 },
        newbool_key:{ false:1 },
        any_key:{ 8:1, fred:2, null:3, 7.6:4, true:5 }
    });

    xju.ts.assertEqual(asInstanceOfEncodableKeyTypes({"str_key": {"fred": 3}, "int_key": {"1": 4}, "more_keys": {"null": 1, "9": 2, "5.5": 3, "true": 4}, "duration_key": {"77.2": 1}, "timestamp_key": {"88.1": 1}, "non_str_literal_key": {"10": 1, "true": 2}, "str_literal_key": {"a\n\"nail\"'s tip": 3, "fred": 4, "rooster": 5}, "enum_key": {"fred": 1, "jock": 2}, "custom_key": {"3221751809": 1}, "alt_custom_key": {"192.8.8.1/24": 1}, "newstr_key": {"red": 1, "green": 2}, xjustr_key:{ally: 9}, "newfloat_key": {"22": 1}, xjufloat_key:{ 22.5:1 }, "newint_key": {"7": 1}, "newbool_key": {"false": 1}, "any_key": {"8": 1, "fred": 2, "null": 3, "7.6": 4, "true": 5}}) as object, good());
    xju.ts.assertEqual(isInstanceOfEncodableKeyTypes(good()), true);

    const errors:Array<string> = [];
    try {
        asInstanceOfEncodableKeyTypes({
            ...good(),
            str_key:null
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute str_key is invalid because Error: null is not a { [key: string]: number } because Error: null is not an object it is null');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableKeyTypes({
            ...good(),
            int_key:null
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'Error: { str_key:{ fred:3 }, int_key:null, more_keys:{ 9:2, null:1, 5.5:3, true:4 }, duration_key:{ 77.2:1 }, timestamp_key:{ 88.1:1 }, non_str_literal_key:{ 10:1, true:2 }, str_literal_key:{ a\n"nail"\'s tip:3, fred:4, rooster:5 }, enum_key:{ fred:1, jock:2 }, custom_key:{ 3221751809:1 }, alt_custom_key:{ 192.8.8.1/24:1 }, newstr_key:{ red:1, green:2 }, xjustr_key:{ ally:9 }, newfloat_key:{ 22:1 }, xjufloat_key:{ 22.5:1 }, newint_key:{ 7:1 }, newbool_key:{ false:1 }, any_key:{ 8:1, fred:2, null:3, 7.6:4, true:5 } } is not a EncodableKeyTypes because Error: attribute int_key is invalid because Error: null is not a { [key: string /* int */]: number } because Error: null is not an object it is null');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableKeyTypes({
            ...good(),
            more_keys:null
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute more_keys is invalid because Error: null is not a { [key: string /* string ** null **|string ** xju.misc.ByteCount **|string ** float **|string ** bool ** */]: number } because Error: null is not an object it is null');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableKeyTypes({
            ...good(),
            duration_key:null
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute duration_key is invalid because Error: null is not a { [key: string /* xju.time.Duration */]: number } because Error: null is not an object it is null');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableKeyTypes({
            ...good(),
            timestamp_key:null
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute timestamp_key is invalid because Error: null is not a { [key: string /* xju.time.Timestamp */]: number } because Error: null is not an object it is null');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableKeyTypes({
            ...good(),
            non_str_literal_key:null
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), "attribute non_str_literal_key is invalid because Error: null is not a { [key: string /* '10'|'true' */]: number } because Error: null is not an object it is null");
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableKeyTypes({
            ...good(),
            str_literal_key:null
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute str_literal_key is invalid because Error: null is not a { [key: string /* \"a\n\"nail\"\'s tip\"|O.a|string ** Surname ** */]: number } because Error: null is not an object it is null');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableKeyTypes({
            ...good(),
            enum_key:null
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute enum_key is invalid because Error: null is not a { [key: string /* O */]: number } because Error: null is not an object it is null');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableKeyTypes({
            ...good(),
            custom_key:null
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute custom_key is invalid because Error: null is not a { [key: string /* number ** IpV4Addr ** */]: number } because Error: null is not an object it is null');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableKeyTypes({
            ...good(),
            alt_custom_key:null
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute alt_custom_key is invalid because Error: null is not a { [key: string /* string ** IpV4AddrWithPrefix ** */]: number } because Error: null is not an object it is null');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableKeyTypes({
            ...good(),
            newstr_key:null
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute newstr_key is invalid because Error: null is not a { [key: string]: number } because Error: null is not an object it is null');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableKeyTypes({
            ...good(),
            xjustr_key:null
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute xjustr_key is invalid because Error: null is not a { [key: string /* xju.json_codec.TypeScriptUQN */]: number } because Error: null is not an object it is null');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableKeyTypes({
            ...good(),
            newfloat_key:null
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute newfloat_key is invalid because Error: null is not a { [key: string /* float */]: number } because Error: null is not an object it is null');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableKeyTypes({
            ...good(),
            xjufloat_key:null
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute xjufloat_key is invalid because Error: null is not a { [key: string /* Metres */]: number } because Error: null is not an object it is null');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableKeyTypes({
            ...good(),
            newint_key:null
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute newint_key is invalid because Error: null is not a { [key: string /* int */]: number } because Error: null is not an object it is null');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableKeyTypes({
            ...good(),
            newbool_key:null
        });
        assert_never_reached();
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute newbool_key is invalid because Error: null is not a { [key: string /* bool */]: number } because Error: null is not an object it is null');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        const x = asInstanceOfEncodableKeyTypes({
            ...good(),
            any_key:null
        });
        assert_not_equal(x, x);
    }
    catch(e){
        try{
            assert_x_contains_y(str(e), 'attribute any_key is invalid because Error: null is not a { [key: string]: any } because Error: null is not an object it is null');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    xju.ts.assertEqual(errors, []);
})();


//alias
(() => {
    var x:any = 8;
    var y:number|string = xju.asInstanceOfNorS(x);
    if (typeof y !== 'number') {
        throw new Error(`typeof y is ${typeof y} not number`);
    }
    xju.ts.assertEqual(y, 8);
    xju.ts.assertEqual(xju.isInstanceOfNorS(x),true);
    x = "fred";
    y = xju.asInstanceOfNorS(x);
    xju.ts.assertEqual(typeof y, 'string');
    xju.ts.assertEqual(y, "fred");
    try{
      y = xju.asInstanceOfNorS([]);
      assert_not_equal(y,y);
    }
    catch(e) {
      xju.ts.assertEqual(`${e}`, 'Error: object [] is not a number|string because Error: [] is not a number it is a object and [] is not a string it is a object');
    }
    xju.ts.assertEqual(xju.isInstanceOfNorS([]),false);
})();
//NewInt alias
(() => {
    var x:any = 8;
    var y:xju.misc.ByteCount = xju.misc.asInstanceOfByteCount(x);
    xju.ts.assertEqual(y, 8);
    if (!xju.misc.isInstanceOfByteCount(x)) {
        throw new Error(`${y} (of type ${typeof y}) is not a xju.misc.ByteCount`);
    }
    xju.ts.assertEqual(x, 8);
})();
//NewFloat alias
(() => {
    var x:any = 8.8;
    var y:xju.time.Duration = xju.time.asInstanceOfDuration(x);
    xju.ts.assertEqual(y, 8.8);
    if (!xju.time.isInstanceOfDuration(x)) {
        throw new Error(`${y} (of type ${typeof y}) is not a xju.time.Duration`);
    }
    xju.ts.assertEqual(x, 8.8);
})();
//NewStr alias
(() => {
    var x:any = "fred";
    var y:xju.json_codec.TypeScriptUQN = xju.json_codec.asInstanceOfTypeScriptUQN(x);
    xju.ts.assertEqual(y, "fred");
    if (!xju.json_codec.isInstanceOfTypeScriptUQN(x)) {
        throw new Error(`${y} (of type ${typeof y}) is not a xju.json_codec.TypeScriptUQN`);
    }
    xju.ts.assertEqual(x, "fred");
})();


//Lias
(() => {
    xju.ts.assertEqual(asInstanceOfLias({"x":8}),{x:8});
    isInstanceOfLias({"x":8});
    xju.ts.assertEqual(asInstanceOfLiasee({"x":8}),{x:8});
    isInstanceOfLiasee({"x":8});
    try{
      const y = asInstanceOfLiasee({"x":[]});
      assert_not_equal(y,y);
    }
    catch(e){
      xju.ts.assertEqual(`${e}`,'Error: { x:[] } is not a Liasee because Error: attribute x is invalid because Error: [] is not a number it is a object');
    }
    xju.ts.assertEqual(asInstanceOfLiaser({l:{"x":8}}),{l:{x:8}});
    xju.ts.assertEqual(isInstanceOfLiaser({l:{"x":8}}),true);
    xju.ts.assertEqual(isInstanceOfLiaser({"x":[]}),false);
    try{
      const y = asInstanceOfLiaser({l:{"x":[]}});
      assert_not_equal(y,y);
    }
    catch(e){
      xju.ts.assertEqual(`${e}`,'Error: { l:{ x:[] } } is not a Liaser because Error: attribute l is invalid because Error: { x:[] } is not a Liasee because Error: attribute x is invalid because Error: [] is not a number it is a object');
    }

    isInstanceOfLias({"x":8});
})();

//xju.ts
(() => {
  xju.ts.assertEqual(1, 1);
  xju.ts.assertEqual([1], [1]);
  xju.ts.assertEqual({a:1},{a:1});
  const errors:Array<string> = [];
  try {
    xju.ts.assertEqual(1,2);
    assert_never_reached();
  }
  catch(e){
    try{
      assert_x_contains_y(str(e), '1 != 2');
    }
    catch(ee){
      errors.push(`${ee}`);
    }
  }
  try {
    xju.ts.assertEqual([1],[2]);
    assert_never_reached();
  }
  catch(e){
    try{
      assert_x_contains_y(str(e), 'array element 0 1 != 2');
    }
    catch(ee){
      errors.push(`${ee}`);
    }
  }
  try {
    xju.ts.assertEqual([1],[1,1]);
    assert_never_reached();
  }
  catch(e){
    try{
      assert_x_contains_y(str(e), 'arrays of different length because Error: 1 != 2');
    }
    catch(ee){
      errors.push(`${ee}`);
    }
  }
  try {
    xju.ts.assertEqual({a:1},{b:1});
    assert_never_reached();
  }
  catch(e){
    try{
      assert_x_contains_y(str(e), 'object { a:1 } != object { b:1 } because Error: array [\"a\"] != array [\"b\"] because Error: array element 0 \"a\" != \"b\"');
    }
    catch(ee){
      errors.push(`${ee}`);
    }
  }
  try {
    xju.ts.assertEqual({a:1},{a:2});
    assert_never_reached();
  }
  catch(e){
    try{
      assert_x_contains_y(str(e), 'object { a:1 } != object { a:2 } because Error: property a is not equal because Error: 1 != 2');
    }
    catch(ee){
      errors.push(`${ee}`);
    }
  }
  try {
    xju.ts.assertEqual({a:1},{a:1,b:2});
    assert_never_reached();
  }
  catch(e){
    try{
      assert_x_contains_y(str(e), 'object { a:1 } != object { a:1, b:2 } because Error: array [\"a\"] != array [\"a\",\"b\"] because Error: arrays of different length because Error: 1 != 2');
    }
    catch(ee){
      errors.push(`${ee}`);
    }
  }
  xju.ts.assertEqual(errors, []);
})();
