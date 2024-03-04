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
  var $input=$('input');
  var $ul=$('ul');
  var $li=$ul.find('li').remove();
  var i;
  for(i=0; i!=200; ++i){
    $ul.append($li.clone().attr('id',i).text(i));
  }
  $input.change(function(){
    setTimeout(function(){
      var $lis=$ul.find('li');
      var $e=$();
      var i;
      var n=parseInt($input.val());
      if (!isNaN(n)){
	for(i=0;i!=5;++i){
	  $e=$e.add($lis[n+i]);
      };
	uwl.showElement($e,$input.outerHeight());
      }
    },10);
  });
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
  uwl.trackTextInput($('input.tracked'),function(x){
    $('span.input-value').text(x);
  });
});
