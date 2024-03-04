// Copyright (c) 2018 Trevor Taylor
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
$(document).ready(function(){
  $('.query-params').text(uwl.json.encode(uwl.queryParams()));
  $('a.ajax-post').click(function(){
    uwl.postToServer('post-json',{
	'type_':'json',
	'name':'jock',
	'age':30})
      .then(function(result){
	$('div.result').text('POST result (json)'+uwl.json.encode(result));
      });
    return false;
  });
  $('a.ajax-get').click(function(){
    uwl.getFromServer('post-json',{
	'type_':'big',
	'name':'joe',
	'age':31})
      .then(function(result){
	$('div.result').text('GET result (json)'+uwl.json.encode(result));
      });
    return false;
  });
  $('a.ajax-post-json-params').click(function(){
    uwl.postToServer('post-json-params',{
	'type_':'json',
	'name':'jock',
	'age':30})
      .then(function(result){
	$('div.result').text('POST result (json)'+uwl.json.encode(result));
      });
    return false;
  });
  uwl.trackTextInput($('input.tracked'),function(x){
    $('span.input-value').text(x);
  });
  $('a[href="await-flag"]').click(function(){
    let $result = $('.flagged');
    $result.text('Waiting...');
    uwl.postToServer('await-flag',{})
      .then(function(result){
	$result.text('Flagged');
      });
    return false;
  });
  $('a[href="set-flag"]').click(function(){
    let $result = $('.flagged');
    uwl.postToServer('set-flag',{})
      .then(function(result){
	$result.text('Flagged');
      });
    return false;
  });
  $('a.async-post').click(async function(){
    let result = await uwl.asyncPostToServer(uwl.asyncTimeout(2.0), 'post-json', {
	'type_':'json',  // note no special meaning
	'name':'jock',
      'age':30});
    $('div.result').text('POST result (json)'+uwl.json.encode(
      result));
    return false;
  });
  $('a.async-post-5s').click(async function(){
    try{
      let result = await uwl.asyncPostToServer(uwl.asyncTimeout(2.0), 'post-json-5s', {
	'type_':'json',  // note no special meaning
	'name':'jock',
	'age':30});
      $('div.result').text('UNEXPECTED: POST result (json)'+uwl.json.encode(
	result));
    }
    catch(e){
      $('div.result').text('POST error (expected deadline reached): '+e.message);
    }
    return false;
  });
  $('a.async-get').click(async function(){
    let result = await uwl.asyncGetFromServer(uwl.asyncTimeout(2.0), 'post-json', {
	'type_':'json',  // note no special meaning
	'name':'jock',
      'age':30});
    $('div.result').text('GET result (json)'+uwl.json.encode(
      result));
    return false;
  });
  $('a.async-get-5s').click(async function(){
    try{
      let result = await uwl.asyncGetFromServer(uwl.asyncTimeout(2.0), 'post-json-5s', {
	'type_':'json',  // note no special meaning
	'name':'jock',
	'age':30});
      $('div.result').text('UNEXPECTED: GET result (json)'+uwl.json.encode(
	result));
    }
    catch(e){
      $('div.result').text('GET error (expected deadline reached): '+e.message);
    }
    return false;
  });
  $('a.poll-for-condition-met').click(function(){
    $('div.result').text('');
    let t = setTimeout(function(){
      $('div.result').text('condition met');
    }, 1000);
    setTimeout(async function(){
      let result = await uwl.asyncPollFor(uwl.asyncTimeout(5.0),
					  function(){
					    if ($('div.result').text()!='condition met'){
					      throw Error('condition not met');
					    }
					    return 'condition met and verified';
					  });
      $('div.result').text(result);
    });
    return false;
  });
  $('a.poll-for-condition-timeout').click(function(){
    $('div.result').text('');
    let t = setTimeout(function(){
      $('div.result').text('condition met');
    }, 5000);
    setTimeout(async function(){
      try{
	let result = await uwl.asyncPollFor(uwl.asyncTimeout(2.0),
					    function(){
					      if ($('div.result').text()!='condition met'){
						throw Error('condition not met');
					      }
					      return 'condition met and verified';
					    });
	$('div.result').text('UNEXPECTED: '+result);
      }
      catch(e){
	$('div.result').text('expected condition not met, got: '+e.message);
	clearTimeout(t);
      }
    });
    return false;
  });
});
