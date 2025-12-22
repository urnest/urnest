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

/// <reference path="../xju/assert.ts"/>
/// <reference path="../xju/json_codec_typescript_test.ts"/>

//encodableData
(() => {
    const good = good_encodable_data;

    xju.assert.assertEqual(asInstanceOfEncodableData({"an_int": 7, "a_float": 9.2, "a_str": "fred", "a_new_int": 22, "an_xju_int": 88, "a_new_float": 2.2, "an_xju_float": 18.2, "a_new_str": "red", "an_xju_str": "ally", "a_null": null, "a_boolean": true, "a_union": "walker", "a_list": [1, 2, 3], "any_list": [1, null, "a"], "a_set": [4, 5, 6], "any_set": [null, 2], "a_frozen_set": [7], "any_frozen_set": [2, 4.6], "some_bytes": [10, 11, 12], "a_tuple": [8, "jock", "fred"], "a_literal_str": "mai", "a_literal_int": 7, "a_literal_bool": true, "a_class": {"first_name": "fran", "middle_names": ["jan"], "last_name": "lan", "class": "Upper"}, "a_dont_encode": {"y": "yyy"}, "fred_7_false": false, "a_recurse_self": {"defs": {}}, "a_timestamp": 45000.0, "an_enum": "fred", "a_mixed_in_enum": {"k": 2, "v": "jock"}, "a_recursive": {"a": [{"o": ["1", "2", ["7"]]}, "3", [{"a": ["4", "6", ["8"]]}]]}, "an_enum_value": "fred", "a_json": 9, "a_custom_encoded": 2975337473, "another_custom_encoded": "177.88.12.0/22", "non_key_custom_encoded": 3, "a_custom_generic": "fred", "a_custom_generic_usable_as_key": "fred"}) as object, good());
    xju.assert.assertEqual(isInstanceOfEncodableData({"an_int": 7, "a_float": 9.2, "a_str": "fred", "a_new_int": 22, "an_xju_int": 88, "a_new_float": 2.2, "an_xju_float": 18.2, "a_new_str": "red", "an_xju_str": "ally", "a_null": null, "a_boolean": true, "a_union": "walker", "a_list": [1, 2, 3], "any_list": [1, null, "a"], "a_set": [4, 5, 6], "any_set": [null, 2], "a_frozen_set": [7], "any_frozen_set": [2, 4.6], "some_bytes": [10, 11, 12], "a_tuple": [8, "jock", "fred"], "a_literal_str": "mai", "a_literal_int": 7, "a_literal_bool": true, "a_class": {"first_name": "fran", "middle_names": ["jan"], "last_name": "lan", "class": "Upper"}, "a_dont_encode": {"y": "yyy"}, "fred_7_false": false, "a_recurse_self": {"defs": {}}, "a_timestamp": 45000.0, "an_enum": "fred", "a_mixed_in_enum": {"k": 2, "v": "jock"}, "a_recursive": {"a": [{"o": ["1", "2", ["7"]]}, "3", [{"a": ["4", "6", ["8"]]}]]}, "an_enum_value": "fred", "a_json": 9, "a_custom_encoded": 2975337473, "another_custom_encoded": "177.88.12.0/22", "non_key_custom_encoded": 3, "a_custom_generic": "fred", "a_custom_generic_usable_as_key": "fred"}), true);

    xju.assert.assertEqual(asInstanceOfEncodableData({"an_int": 7, "a_float": 9.2, "a_new_int": 22, "an_xju_int": 88, "a_new_float": 2.2, "an_xju_float": 18.2, "a_new_str": "red", "an_xju_str": "ally", "a_null": null, "a_boolean": true, "a_union": "walker", "a_list": [1, 2, 3], "any_list": [1, null, "a"], "a_set": [4, 5, 6], "any_set": [null, 2], "a_frozen_set": [7], "any_frozen_set": [2, 4.6], "some_bytes": [10, 11, 12], "a_tuple": [8, "jock", "fred"], "a_literal_str": "mai", "a_literal_int": 7, "a_literal_bool": true, "a_class": {"first_name": "fran", "middle_names": ["jan"], "last_name": "lan", "class": "Upper"}, "a_dont_encode": {"y": "yyy"}, "fred_7_false": false, "a_recurse_self": {"defs": {}}, "a_timestamp": 45000.0, "an_enum": "fred", "a_mixed_in_enum": {"k": 2, "v": "jock"}, "a_recursive": {"a": [{"o": ["1", "2", ["7"]]}, "3", [{"a": ["4", "6", ["8"]]}]]}, "an_enum_value": "fred", "a_json": 9, "a_custom_encoded": 2975337473, "another_custom_encoded": "177.88.12.0/22", "non_key_custom_encoded": 3, "a_custom_generic": "fred", "a_custom_generic_usable_as_key": "fred"}) as object,{
      ...good(), a_str: "a_str_default"
    });
    xju.assert.assertEqual(isInstanceOfEncodableData({"an_int": 7, "a_float": 9.2, "a_new_int": 22, "an_xju_int": 88, "a_new_float": 2.2, "an_xju_float": 18.2, "a_new_str": "red", "an_xju_str": "ally", "a_null": null, "a_boolean": true, "a_union": "walker", "a_list": [1, 2, 3], "any_list": [1, null, "a"], "a_set": [4, 5, 6], "any_set": [null, 2], "a_frozen_set": [7], "any_frozen_set": [2, 4.6], "some_bytes": [10, 11, 12], "a_tuple": [8, "jock", "fred"], "a_literal_str": "mai", "a_literal_int": 7, "a_literal_bool": true, "a_class": {"first_name": "fran", "middle_names": ["jan"], "last_name": "lan", "class": "Upper"}, "a_dont_encode": {"y": "yyy"}, "fred_7_false": false, "a_recurse_self": {"defs": {}}, "a_timestamp": 45000.0, "an_enum": "fred", "a_mixed_in_enum": {"k": 2, "v": "jock"}, "a_recursive": {"a": [{"o": ["1", "2", ["7"]]}, "3", [{"a": ["4", "6", ["8"]]}]]}, "an_enum_value": "fred", "a_json": 9, "a_custom_encoded": 2975337473, "another_custom_encoded": "177.88.12.0/22", "non_key_custom_encoded": 3, "a_custom_generic": "fred", "a_custom_generic_usable_as_key": "fred"}), true);

    const errors:Array<string> = [];
    try {
        asInstanceOfEncodableData({
            ...good(),
            an_int: 7.6
        });
        xju.assert.assertNeverReached('an_int');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify { an_int:7.6, a_float:9.2, a_str:\"fred\", a_new_int:22, an_xju_int:88, a_new_float:2.2, an_xju_float:18.2, a_new_str:\"red\", an_xju_str:\"ally\", a_null:null, a_boolean:true, a_union:\"walker\", a_list:[1,2,3], any_list:[1,null,\"a\"], a_set:[4,5,6], any_set:[null,2], a_frozen_set:[7], any_frozen_set:[2,4.6], some_bytes:[10,11,12], a_tuple:[8,\"jock\",\"fred\"], a_literal_str:\"mai\", a_literal_int:7, a_literal_bool:true, a_class:{ first_name:\"fran\", middle_names:[\"jan\"], last_name:\"lan\", class:\"Upper\" }, a_dont_encode:{ y:\"yyy\" }, fred_7_false:false, a_recurse_self:{ defs:{ } }, a_timestamp:45000, an_enum:\"fred\", a_mixed_in_enum:{ k:2, v:\"jock\" }, a_recursive:{ a:[{ o:[\"1\",\"2\",[\"7\"]] },\"3\",[{ a:[\"4\",\"6\",[\"8\"]] }]] }, an_enum_value:\"fred\", a_json:9, a_custom_encoded:2975337473, another_custom_encoded:\"177.88.12.0/22\", non_key_custom_encoded:3, a_custom_generic:\"fred\", a_custom_generic_usable_as_key:\"fred\" } is a EncodableData because failed to validate property an_int because failed to verify 7.6 is a int because 7.6 is not a whole number');
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
        xju.assert.assertNeverReached('a_float');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_float because failed to verify null is a float because null is a object not a number');
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
        xju.assert.assertNeverReached('a_str');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_str because failed to verify 77 is a string because 77 is a number not a string');
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
        xju.assert.assertNeverReached('a_new_int');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_new_int because failed to verify [] is a int because [] is a object not a number');
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
        xju.assert.assertNeverReached('an_xju_int');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property an_xju_int because failed to verify [] is a ByteCount because [] is a object not a number');
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
        xju.assert.assertNeverReached('a_new_float');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_new_float because failed to verify [] is a float because [] is a object not a number');
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
        xju.assert.assertNeverReached('an_xju_float');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property an_xju_float because failed to verify [] is a Metres because [] is a object not a number');
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
        xju.assert.assertNeverReached('a_new_str');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_new_str because failed to verify { } is a string because { } is a object not a string');
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
        xju.assert.assertNeverReached('an_xju_str');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property an_xju_str because failed to verify { } is a TypeScriptUQN because { } is a object not a string');
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
        xju.assert.assertNeverReached('a_null');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_null because failed to verify { } is a null because { } is not null');
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
        xju.assert.assertNeverReached('a_boolean');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_boolean because failed to verify 99 is a bool because 99 is a number not a boolean');
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
        xju.assert.assertNeverReached('a_union');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_union because failed to verify 99 is a FullName|Surname|null because failed to verify 99 is a FullName because 99 is not a non-array object and failed to verify 99 is a Surname because failed to verify 99 is a string because 99 is a number not a string and failed to verify 99 is a null because 99 is not null');
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
        xju.assert.assertNeverReached('a_list');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_list because failed to verify [null] is a Array<int> because failed to validate item 0 because failed to verify null is a int because null is a object not a number');
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
        xju.assert.assertNeverReached('any_list');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property any_list because failed to verify null is a Array because null is not an array it is a object');
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
        xju.assert.assertNeverReached('a_set');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_set because failed to verify [null] is a Array<int> because failed to validate item 0 because failed to verify null is a int because null is a object not a number');
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
        xju.assert.assertNeverReached('any_set');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property any_set because failed to verify { } is a Array because { } is not an array it is a object');
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
        xju.assert.assertNeverReached('a_frozen_set');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_frozen_set because failed to verify [null] is a Array<int> because failed to validate item 0 because failed to verify null is a int because null is a object not a number');
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
        xju.assert.assertNeverReached('a_frozen_set2');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_frozen_set because failed to verify [null] is a Array<int> because failed to validate item 0 because failed to verify null is a int because null is a object not a number');
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
        xju.assert.assertNeverReached('some_bytes');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property some_bytes because failed to verify [3000] is a bytes because failed to validate item 0 because 3000 is not in range 0..255');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_tuple: [3000,99,"fred"]
        });
        xju.assert.assertNeverReached('a_tuple');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_tuple because failed to verify [3000,99,"fred"] is a [int,string,G<int,GT>] because failed to validate item 1 because failed to verify 99 is a string because 99 is a number not a string');
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
        xju.assert.assertNeverReached('a_literal_str');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_literal_str because failed to verify \"kai\" is a \"ann\"|\"mai\" because failed to verify \"kai\" is a \"ann\" because \"kai\" != \"ann\" and failed to verify \"kai\" is a \"mai\" because \"kai\" != \"mai\"');
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
        xju.assert.assertNeverReached('a_literal_int');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_literal_int because failed to verify 8 is a 7 because 8 != 7');
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
        xju.assert.assertNeverReached('a_literal_bool');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_literal_bool because failed to verify false is a true because false != true');
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
        xju.assert.assertNeverReached('a_class');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_class because failed to verify { first_name:8, middle_names:[\"jan\"], last_name:\"lan\" } is a FullName because failed to validate property first_name because failed to verify 8 is a string because 8 is a number not a string');
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
        xju.assert.assertNeverReached('fred_7_false');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property fred_7_false because failed to verify \"jan\" is a \"fred\"|7|false because failed to verify \"jan\" is a \"fred\" because \"jan\" != \"fred\" and failed to verify \"jan\" is a 7 because \"jan\" != 7 and failed to verify \"jan\" is a false because \"jan\" != false');
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
        xju.assert.assertNotEqual(x,x);
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_recurse_self because failed to verify 999 is a xju.json_codec.TypeScriptNamespace because 999 is not a non-array object');
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
        xju.assert.assertNotEqual(x,x);
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_recurse_self because failed to verify { defs:1 } is a xju.json_codec.TypeScriptNamespace because failed to validate property defs because failed to verify 1 is a { [key TypeScriptUQN] : TypeScriptSourceCode|xju.json_codec.TypeScriptNamespace } because 1 is not a non-array object');
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
        xju.assert.assertNotEqual(x,x);
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_recurse_self because failed to verify { defs:{ 1:null } } is a xju.json_codec.TypeScriptNamespace because failed to validate property defs because failed to verify { 1:null } is a { [key TypeScriptUQN] : TypeScriptSourceCode|xju.json_codec.TypeScriptNamespace } because failed to validate property 1 because failed to verify null is a TypeScriptSourceCode|xju.json_codec.TypeScriptNamespace because failed to verify null is a TypeScriptSourceCode because null is a object not a string and failed to verify null is a xju.json_codec.TypeScriptNamespace because null is not a non-array object');
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
        xju.assert.assertNeverReached('a_timestamp');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_timestamp because failed to verify null is a Timestamp because null is a object not a number');
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
        xju.assert.assertNeverReached('an_enum');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property an_enum because failed to verify 7 is a \"fred\"|\"jock\" because failed to verify 7 is a \"fred\" because 7 != \"fred\" and failed to verify 7 is a \"jock\" because 7 != \"jock\"');
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
        xju.assert.assertNeverReached('a_mixed_in_enum');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_mixed_in_enum because failed to verify { k:2, v:\"brad\" } is a {\"k\":1,\"v\":\"fred\"}|{\"k\":2,\"v\":\"jock\"} because failed to verify { k:2, v:\"brad\" } is a {\"k\":1,\"v\":\"fred\"} because failed to verify object { k:2, v:\"brad\" } == object { k:1, v:\"fred\" } because failed to verify property k equal because 2 != 1 and failed to verify { k:2, v:\"brad\" } is a {\"k\":2,\"v\":\"jock\"} because failed to verify object { k:2, v:\"brad\" } == object { k:2, v:\"jock\" } because failed to verify property v equal because \"brad\" != \"jock\"');
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
        xju.assert.assertNeverReached('an_enum_value');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property an_enum_value because failed to verify \"paul\" is a \"fred\" because \"paul\" != \"fred\"');
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
        xju.assert.assertNeverReached('a_recursive');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_recursive because failed to verify [] is a string|And|Or because failed to verify [] is a string because [] is a object not a string and failed to verify [] is a And because [] is not a non-array object and failed to verify [] is a Or because [] is not a non-array object');
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
        xju.assert.assertNeverReached('a_recursive');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_recursive because failed to verify { a:[{ o:[\"1\",\"2\",[]] },\"3\"] } is a string|And|Or because failed to verify { a:[{ o:[\"1\",\"2\",[]] },\"3\"] } is a string because { a:[{ o:[\"1\",\"2\",[]] },\"3\"] } is a object not a string and failed to verify { a:[{ o:[\"1\",\"2\",[]] },\"3\"] } is a And because failed to validate property a because failed to verify [{ o:[\"1\",\"2\",[]] },\"3\"] is a [string|And|Or,string|And|Or,Array<string|And|Or>] because length is 2 not 3 and failed to verify { a:[{ o:[\"1\",\"2\",[]] },\"3\"] } is a Or because failed to validate property o because property o is missing');
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
        xju.assert.assertNeverReached('a_recursize');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_recursive because failed to verify { a:[{ o:[\"1\",\"2\",[]] },\"3\",[{ a:[\"6\",[]] }]] } is a string|And|Or because failed to verify { a:[{ o:[\"1\",\"2\",[]] },\"3\",[{ a:[\"6\",[]] }]] } is a string because { a:[{ o:[\"1\",\"2\",[]] },\"3\",[{ a:[\"6\",[]] }]] } is a object not a string and failed to verify { a:[{ o:[\"1\",\"2\",[]] },\"3\",[{ a:[\"6\",[]] }]] } is a And because failed to validate property a because failed to verify [{ o:[\"1\",\"2\",[]] },\"3\",[{ a:[\"6\",[]] }]] is a [string|And|Or,string|And|Or,Array<string|And|Or>] because failed to validate item 2 because failed to verify [{ a:[\"6\",[]] }] is a Array<string|And|Or> because failed to validate item 0 because failed to verify { a:[\"6\",[]] } is a string|And|Or because failed to verify { a:[\"6\",[]] } is a string because { a:[\"6\",[]] } is a object not a string and failed to verify { a:[\"6\",[]] } is a And because failed to validate property a because failed to verify [\"6\",[]] is a [string|And|Or,string|And|Or,Array<string|And|Or>] because length is 2 not 3 and failed to verify { a:[\"6\",[]] } is a Or because failed to validate property o because property o is missing and failed to verify { a:[{ o:[\"1\",\"2\",[]] },\"3\",[{ a:[\"6\",[]] }]] } is a Or because failed to validate property o because property o is missing');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableData({
            ...good(),
            a_custom_encoded:-1
        });
        xju.assert.assertNeverReached('a_custom_encoded');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_custom_encoded because failed to verify -1 is a IpV4Addr because -1 is not in range 0..4294967295');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    xju.assert.assertEqual(isInstanceOfEncodableData({
      ...good(),
      a_custom_encoded:-1
    }), false);
    try {
        asInstanceOfEncodableData({
            ...good(),
            another_custom_encoded:-1
        });
        xju.assert.assertNeverReached('another_custom_encoded');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property another_custom_encoded because failed to verify -1 is a IpV4AddrWithPrefix because failed to verify -1 is a string because -1 is a number not a string');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    xju.assert.assertEqual(isInstanceOfEncodableData({
      ...good(),
      another_custom_encoded:-1
    }), false);
    xju.assert.assertEqual(isInstanceOfEncodableData({
      ...good(),
      another_custom_encoded:"1"
    }), false);

    xju.assert.assertEqual(errors, []);

    // misc coverage:
    xju.assert.assertEqual(asInstanceOfSurname("walker"), "walker");
    xju.assert.assertEqual(xju.json_codec.asInstanceOfTypeScriptNamespace({ defs:{ "a": "true;" } }), { defs:{ "a": "true;" } });
    xju.assert.assertEqual(xju.json_codec.isInstanceOfTypeScriptNamespace({ defs:{ "a": "true;" } }), true);
    xju.assert.assertEqual(xju.json_codec.asInstanceOfTypeScriptSourceCode("true;"), "true;");
    xju.assert.assertEqual(xju.json_codec.isInstanceOfTypeScriptSourceCode("true;"), true);
    xju.assert.assertEqual(xju.time.asInstanceOfTimestamp(8.8), 8.8);
    xju.assert.assertEqual(xju.time.isInstanceOfTimestamp(8.8), true);
    xju.assert.assertEqual(isInstanceOfOr({"o": ["1","2",[]]}),true);
    xju.assert.assertEqual(asInstanceOfOr({"o": ["1","2",[]]}),{"o": ["1","2",[]]});
    xju.assert.assertEqual(isInstanceOfAnd({ "a": ["4","6",[]] }),true);
    xju.assert.assertEqual(asInstanceOfAnd({ "a": ["4","6",[]] }),{ "a": ["4","6",[]] });
    
})();


//EncodableKeyTypes
(() => {
    const good = good_encodable_key_types;

    xju.assert.assertEqual(asInstanceOfEncodableKeyTypes({"str_key": {"fred": 3}, "int_key": {"1": 4}, "more_keys": {"null": 1, "9": 2, "5.5": 3, "true": 4}, "duration_key": {"77.2": 1}, "timestamp_key": {"88.1": 1}, "non_str_literal_key": {"10": 1, "true": 2}, "str_literal_key": {"a\n\"nail\"'s tip": 3, "fred": 4, "rooster": 5}, "enum_key": {"fred": 1, "jock": 2}, "custom_key": {"3221751809": 1}, "alt_custom_key": {"192.8.8.1/24": 1}, "newstr_key": {"red": 1, "green": 2}, xjustr_key:{ally: 9}, "newfloat_key": {"22.0": 1}, xjufloat_key:{ 22.5:1 }, "newint_key": {"7": 1}, "newbool_key": {"false": 1}, "any_key": {"8": 1, "fred": 2, "null": 3, "7.6": 4, "true": 5}, "a_custom_generic_key": {"fred": true}}) as object, good());
    xju.assert.assertEqual(isInstanceOfEncodableKeyTypes(good()), true);

    const errors:Array<string> = [];
    try {
        asInstanceOfEncodableKeyTypes({
            ...good(),
            str_key:null
        });
        xju.assert.assertNeverReached('str_key');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property str_key because failed to verify null is a { [key string] : int } because null is not a non-array object');
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
        xju.assert.assertNeverReached('int_key');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property int_key because failed to verify null is a { [key int] : int } because null is not a non-array object');
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
        xju.assert.assertNeverReached('more_keys');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property more_keys because failed to verify null is a { [key null|ByteCount|float|bool] : int } because null is not a non-array object');
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
        xju.assert.assertNeverReached('duration_key');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property duration_key because failed to verify null is a { [key Duration] : int } because null is not a non-array object');
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
        xju.assert.assertNeverReached('timestamp_key');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property timestamp_key because failed to verify null is a { [key Timestamp] : int } because null is not a non-array object');
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
        xju.assert.assertNeverReached('non_str_literal_key');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), "failed to validate property non_str_literal_key because failed to verify null is a { [key 10|true] : int } because null is not a non-array object");
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
        xju.assert.assertNeverReached('str_literal_key');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property str_literal_key because failed to verify null is a { [key \"a\\n\\\"nail\\\"\'s tip\"|\"fred\"|Surname] : int } because null is not a non-array object');
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
        xju.assert.assertNeverReached('enum_key');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property enum_key because failed to verify null is a { [key \"fred\"|\"jock\"] : int } because null is not a non-array object');
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
        xju.assert.assertNeverReached('custom_key');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property custom_key because failed to verify null is a { [key IpV4Addr] : int } because null is not a non-array object');
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
        xju.assert.assertNeverReached('alt_custom_key');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property alt_custom_key because failed to verify null is a { [key IpV4Addr] : int } because null is not a non-array object');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        asInstanceOfEncodableKeyTypes({
            ...good(),
            alt_custom_key:{7:1}
        });
        xju.assert.assertNeverReached('alt_custom_key2');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property 7 because failed to verify \"7\" is a keyof IpV4Addr because \"7\" does not match /^([0-9]+)[.]([0-9]+)[.]([0-9]+)[.]([0-9]+)[/]([0-9]+)$/');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    xju.assert.assertEqual(isInstanceOfEncodableKeyTypes({
      ...good(),
      alt_custom_key:{7:1}
    }),false);
    try {
        asInstanceOfEncodableKeyTypes({
            ...good(),
            newstr_key:null
        });
        xju.assert.assertNeverReached('newstr_key');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property newstr_key because failed to verify null is a { [key string] : int } because null is not a non-array object');
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
        xju.assert.assertNeverReached('xjustr_key');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property xjustr_key because failed to verify null is a { [key TypeScriptUQN] : int } because null is not a non-array object');
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
        xju.assert.assertNeverReached('newfloat_key');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property newfloat_key because failed to verify null is a { [key float] : int } because null is not a non-array object');
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
        xju.assert.assertNeverReached('xjufloat_key');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property xjufloat_key because failed to verify null is a { [key Metres] : int } because null is not a non-array object');
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
        xju.assert.assertNeverReached('newint_key');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property newint_key because failed to verify null is a { [key int] : int } because null is not a non-array object');
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
        xju.assert.assertNeverReached('newbool_key');
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property newbool_key because failed to verify null is a { [key bool] : int } because null is not a non-array object');
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
        xju.assert.assertNotEqual(x, x);
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property any_key because failed to verify null is a { [key any] : any } because null is not a non-array object');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    try {
        const x = asInstanceOfEncodableKeyTypes({
            ...good(),
            a_custom_generic_key: null
        });
        xju.assert.assertNotEqual(x, x);
    }
    catch(e){
        try{
            xju.assert.assertXcontainsY(xju.str(e), 'failed to validate property a_custom_generic_key because failed to verify null is a { [key GK<int,GT>] : bool } because null is not a non-array object');
        }
        catch(ee){
            errors.push(`${ee}`);
        }
    }
    xju.assert.assertEqual(errors, []);
})();


//alias
(() => {
    var x:any = 8;
    var y:number|string = xju.asInstanceOfNorS(x);
    if (typeof y !== 'number') {
        throw new Error(`typeof y is ${typeof y} not number`);
    }
    xju.assert.assertEqual(y, 8);
    xju.assert.assertEqual(xju.isInstanceOfNorS(x),true);
    x = "fred";
    y = xju.asInstanceOfNorS(x);
    xju.assert.assertEqual(typeof y, 'string');
    xju.assert.assertEqual(y, "fred");
    try{
      y = xju.asInstanceOfNorS([]);
      xju.assert.assertNotEqual(y,y);
    }
    catch(e) {
      xju.assert.assertEqual(`${e}`, 'Error: failed to verify [] is a int|string because failed to verify [] is a int because [] is a object not a number and failed to verify [] is a string because [] is a object not a string');
    }
    xju.assert.assertEqual(xju.isInstanceOfNorS([]),false);
})();
//NewInt alias
(() => {
    var x:any = 8;
    var y:xju.misc.ByteCount = xju.misc.asInstanceOfByteCount(x);
    xju.assert.assertEqual(y, 8);
    if (!xju.misc.isInstanceOfByteCount(x)) {
        throw new Error(`${y} (of type ${typeof y}) is not a xju.misc.ByteCount`);
    }
    xju.assert.assertEqual(x, 8);
})();
//NewFloat alias
(() => {
    var x:any = 8.8;
    var y:xju.time.Duration = xju.time.asInstanceOfDuration(x);
    xju.assert.assertEqual(y, 8.8);
    if (!xju.time.isInstanceOfDuration(x)) {
        throw new Error(`${y} (of type ${typeof y}) is not a xju.time.Duration`);
    }
    xju.assert.assertEqual(x, 8.8);
})();
//NewStr alias
(() => {
    var x:any = "fred";
    var y:xju.json_codec.TypeScriptUQN = xju.json_codec.asInstanceOfTypeScriptUQN(x);
    xju.assert.assertEqual(y, "fred");
    if (!xju.json_codec.isInstanceOfTypeScriptUQN(x)) {
        throw new Error(`${y} (of type ${typeof y}) is not a xju.json_codec.TypeScriptUQN`);
    }
    xju.assert.assertEqual(x, "fred");
})();


//Lias
(() => {
    xju.assert.assertEqual(asInstanceOfLias({"x":8}),{x:8});
    isInstanceOfLias({"x":8});
    xju.assert.assertEqual(asInstanceOfLiasee({"x":8}),{x:8});
    isInstanceOfLiasee({"x":8});
    try{
      const y = asInstanceOfLiasee({"x":[]});
      xju.assert.assertNotEqual(y,y);
    }
    catch(e){
      xju.assert.assertEqual(`${e}`,'Error: failed to verify { x:[] } is a Liasee because failed to validate property x because failed to verify [] is a int because [] is a object not a number');
    }
    xju.assert.assertEqual(asInstanceOfLiaser({l:{"x":8}}),{l:{x:8}});
    xju.assert.assertEqual(isInstanceOfLiaser({l:{"x":8}}),true);
    xju.assert.assertEqual(isInstanceOfLiaser({"x":[]}),false);
    try{
      const y = asInstanceOfLiaser({l:{"x":[]}});
      xju.assert.assertNotEqual(y,y);
    }
    catch(e){
      xju.assert.assertEqual(`${e}`,'Error: failed to verify { l:{ x:[] } } is a Liaser because failed to validate property l because failed to verify { x:[] } is a Liasee because failed to validate property x because failed to verify [] is a int because [] is a object not a number');
    }

    isInstanceOfLias({"x":8});
})();

//xju.ts
(() => {
  xju.assert.assertEqual(1, 1);
  xju.assert.assertEqual([1], [1]);
  xju.assert.assertEqual({a:1},{a:1});
  const errors:Array<string> = [];
  try {
    xju.assert.assertEqual(1,2);
    xju.assert.assertNeverReached('1==2?');
  }
  catch(e){
    try{
      xju.assert.assertXcontainsY(xju.str(e), '1 != 2');
    }
    catch(ee){
      errors.push(`${ee}`);
    }
  }
  try {
    xju.assert.assertEqual([1],[2]);
    xju.assert.assertNeverReached('[1]==[2]?');
  }
  catch(e){
    try{
      xju.assert.assertXcontainsY(xju.str(e), 'array element 0 1 != 2');
    }
    catch(ee){
      errors.push(`${ee}`);
    }
  }
  try {
    xju.assert.assertEqual([1],[1,1]);
    xju.assert.assertNeverReached('[1]==[1,1]?');
  }
  catch(e){
    try{
      xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify array [1] == array [1,1] because failed to verify arrays have same length because 1 != 2');
    }
    catch(ee){
      errors.push(`${ee}`);
    }
  }
  try {
    xju.assert.assertEqual({a:1},{b:1});
    xju.assert.assertNeverReached('{a:1}=={b:1}?');
  }
  catch(e){
    try{
      xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify object { a:1 } == object { b:1 } because failed to verify array [\"a\"] == array [\"b\"] because array element 0 \"a\" != \"b\"');
    }
    catch(ee){
      errors.push(`${ee}`);
    }
  }
  try {
    xju.assert.assertEqual({a:1},{a:2});
    xju.assert.assertNeverReached('{a:1}=={a:2}?');
  }
  catch(e){
    try{
      xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify object { a:1 } == object { a:2 } because failed to verify property a equal because 1 != 2');
    }
    catch(ee){
      errors.push(`${ee}`);
    }
  }
  try {
    xju.assert.assertEqual({a:1},{a:1,b:2});
    xju.assert.assertNeverReached('{a:1}=={a:1,b:2}?');
  }
  catch(e){
    try{
      xju.assert.assertXcontainsY(xju.str(e), 'Error: failed to verify object { a:1 } == object { a:1, b:2 } because failed to verify array [\"a\"] == array [\"a\",\"b\"] because failed to verify arrays have same length because 1 != 2');
    }
    catch(ee){
      errors.push(`${ee}`);
    }
  }
  xju.assert.assertEqual(errors, []);
})();

// coverage
(() => {
  const good = good_encodable_data();
  xju.assert.assertEqual(asInstanceOfMetres(good.an_xju_float),18.2);
  xju.assert.assertEqual(isInstanceOfMetres(good.an_xju_float),true);

  xju.assert.assertEqual(asInstanceOfFullName({first_name: "fran", middle_names: ["jan"], last_name: "lan", "class": "Upper"}),{first_name: "fran", middle_names: ["jan"], last_name: "lan", "class": "Upper"});
  xju.assert.assertEqual(isInstanceOfFullName({first_name: "fran", middle_names: ["jan"], last_name: "lan", "class": "Upper"}),true);

  xju.assert.assertEqual(asInstanceOfSurname("Walker"),"Walker");
  xju.assert.assertEqual(isInstanceOfSurname("Walker"),true);

  xju.assert.assertEqual(asInstanceOfDontEncode({"y": "yyy"}),{"y": "yyy"});
  xju.assert.assertEqual(isInstanceOfDontEncode({"y": "yyy"}),true);

  xju.assert.assertEqual(asInstanceOfO("fred"),O.a);
  xju.assert.assertEqual(isInstanceOfO("fred"),true);

  xju.assert.assertEqual(asInstanceOfMixedEnum({"k": 2, "v": "jock"}),{"k": 2, "v": "jock"});
  xju.assert.assertEqual(isInstanceOfMixedEnum({"k": 2, "v": "jock"}),true);

  xju.assert.assertEqual(a_module.asInstanceOfNT({x:1,y:44}), {x:1,y:44,z:2.3} as a_module.NT);
  xju.assert.assertEqual(a_module.isInstanceOfNT({x:1,y:44}), true);

  xju.assert.assertEqual(a_module.asInstanceOfNTA({x:1,y:44}), {x:1,y:44,z:2.3} as a_module.NTA);
  xju.assert.assertEqual(a_module.isInstanceOfNTA({x:1,y:44}), true);

  xju.assert.assertEqual(a_module.asInstanceOfET("fred"), a_module.ET.a);
  xju.assert.assertEqual(a_module.isInstanceOfET("fred"), true);
})();
