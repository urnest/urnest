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
  $('a.ajax-post').click(function(){
    wal.postToServer('post-json',{
	'type_':'json',
	'name':'jock',
	'age':30})
      .then(function(result){
	$('div.result').text('POST result (json)'+wal.json.encode(result));
      });
    return false;
  });
  $('a.ajax-get').click(function(){
    wal.getFromServer('post-json',{
	'type_':'big',
	'name':'joe',
	'age':31})
      .then(function(result){
	$('div.result').text('GET result (json)'+wal.json.encode(result));
      });
    return false;
  });
});
