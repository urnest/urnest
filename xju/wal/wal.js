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

// miscellaneous utilities
//
(function( sal, undefined ) {
  // give old browswers Date.now()
  if (!Date.now) {
    Date.now = function() { return new Date().getTime(); }
  }
  // add busyClass to $x (a jquery object) DOM objects and all their children,
  // until matching result.done() called, holding class for at least
  // specified minSeconds (a float)
  wal.busy=function($x,busyClass,minSeconds){
    var self;
    self={
      count:0,
      done:function(){
	if (self.count==1){
	  $x.find('*').addBack().removeClass(busyClass);
	  self.count=0;
	}
	else{
	  --self.count;
	}
      },
      busy:function(){
	++self.count;
	if (self.count=1){
	  $x.find('*').addBack().addClass(busyClass);
	  setTimeout(self.done,minSeconds*1000);
	  ++self.count;
	}
	return self;
      }
    }
    return self.busy();
  };

  // deep copy x
  wal.clone=function(x){
    if (x===null){
      return x;
    }
    else if (typeof(x) == 'object' && x.constructor === Array){
      var result=[];
      wal.each(x, function(i){
	result.push(wal.clone(x[i]));
      });
      return result;
    }
    else if (typeof(x)=='object'){
      var result;
      if (typeof(x.ebClone)=='function'){
	result=x.ebClone();
      }
      else {
	result={}
	for(k in x){
	  result[k]=wal.clone(x[k]);
	}
      }
      return result;
    }
    else return x;
  };
  // date (like {year:2016,month:12,day:31}) falls before today
  wal.dateHasPast=function(date){
    var today=new Date();
    var yearToday=today.getYear()+1900;
    var monthToday=today.getMonth()+1;//Date() numbers months 0..11
    var dayOfMonthToday=today.getDate();
    if (date.year < yearToday){
      return true;
    }
    if (date.year > yearToday){
      return false;
    }
    if (date.month < monthToday){
      return true;
    }
    if (date.month > monthToday){
      return false;
    }
    if (date.day < dayOfMonthToday){
      return true;
    }
    return false;
  };

  // test if x is defined
  wal.defined=function(x){
    return !(x==undefined);
  };

  // iterate over x:
  // - where x is array, call f(i, x[i]) for i in 0..x.length
  // - where x is object, call f(k, x[k]) for k in x.keys
  wal.each=function(x, f){
    var xt=typeof(x);
    var isArray;
    if (xt=='object'){
      isArray = (Array === x.constructor);
      if (isArray) {
	for(var i=0;i!=x.length;++i){
	  f(i, x[i]);
	}
      }
      else{
	for(var k in x){
	  f(k, x[k]);
	}
      }
    }
  };
  // eg encodeURL('x.html',{a:1,b:'ref'} -> 'x.html?a=1&b=ref
  // note special chars in values are escaped
  wal.encodeURL=function(path,paramsDict){
    var params=wal.map(paramsDict,function(n,v){
      return encodeURIComponent(n)+'='+encodeURIComponent(v);
    });
    if (params.length){
      return path+'?'+wal.join('&',params);
    }
    return path;
  };
  // deep extend object o with object x
  // - note does not clone any element of x, y, z..., copies elements by reference
  // - returns o
  wal.extend=function(o, x /*, y, z... */) {
    var a=arguments;
    var i;
    for(i=1; i < a.length; ++i){
      o=extend_(o, a[i]);
    }
    return o;
  };

  // extends o with x, returning either modified o (where o is an object), or x
  // - where an element of x would replace an element of o, but the elements have
  //   different types, raises an Exception
  function extend_(o, x){
    var ot=typeof(o);
    if (ot=='undefined'){
      return x;
    }
    var xt=typeof(x);
    if (ot != xt){
      throw new String('target type is '+ot+' but source type is '+xt);
    }
    if (xt=='object'){
      if (o.constructor === Array) {
	if (x.constructor !== Array){
	  throw new String('target object is array but source object is not');
	}
	for(var i=0; i != x.length; ++i){
	  try{
	    if (i < o.length){
	      o[i]=extend_(o[i], x[i]);
	    }
	    else{
	      o.push(x[i]);
	    }
	  }
	  catch(e){
	    throw new String('failed to extend o['+i+'] with x['+i+'] because '+
				e.toString());
	  }
	}
	return o;
      }
      for(var k in x){
	try{
	  o[k]=extend_(o[k], x[k]);
	}
	catch(e){
	  throw new String('failed to extend o['+k+'] with x['+k+'] because '+
			      e.toString());
	}
      }
      return o;
    }
    return x;
  };
  // find all members of x that match predicate
  // - where x is an array, matches items in the array, returning their indices
  // - where x is an object, matches member values, returning their keys
  wal.find=function(x, predicate){
    var result=[];
    wal.each(x, function(key, value){
      if (predicate(value)){
	result.push(key);
      }
    });
    return result;
  };
  // oz format date, like {year:1984,month:12,day:31}, in Australian
  // date format, 31/12/1984
  // - day/month/year are not padded
  wal.formatDate=function(date){
    return wal.join('/',[date.day,date.month,date.year]);
  };
  // parse oz format date like 31/12/1984, to give date like {year:1984,month:12,day:31}
  wal.parseDate=function(date){
    var c=date.split('/');
    return {
      day:parseInt(c[0]),
      month:parseInt(c[1]),
      year:parseInt(c[2])
    };
  };
  // get window location (URL) query params, eg
  // http://fred/x.html?a=1&b=2%201 -> {a:'1',b:'2 1'}
  wal.queryParams=function(url){
    var result={};
    url=url||window.location.search;
    if (url.indexOf('?')==-1){
      return result;
    }
    wal.each(url.split('?')[1].split('&'),function(i,p){
      p=p.split('=');
      if (p.length==1){
	result[p[0]]=true;
      }
      else{
	result[p[0]]=decodeURIComponent(p[1]);
      }
    });
    return result;
  };
  // Get background of (first of) $item (a jquery object), as a css dictionary
  wal.getBackground=(function(){
    var result=function($item) {
      // jquery is bizarre here, $item.parents() is in "upwards" order,
      // but $item.parents().addBack() is in "downward" order. There
      // is no selfAnd().
      var $items=$item.parents().addBack();
      var result=getBackground_($items);
      return result;
    };
    // get the background style of the first item with non-transparent
    // background
    // pre: $items.length > 0
    function getBackground_($items) {
      if ($items.length == 1 || bgIsOpaque($items.last())) {
	return {
	  'background-color' : colourAsHex($items.last().css('background-color')||'transparent'),
          'background-image' : $items.last().css('background-image')||'none',
	  'background-position' : $items.last().css('background-position')||'left top',
	  'background-repeat' : $items.last().css('background-repeat')||'repeat'
	};
      }
      return getBackground_($items.slice(0, -1));
    }
    function bgIsOpaque($x) {
      var b=$x.css('background-color');
      return (b != 'transparent' && !zeroAlphaColour(b)) ||
	$x.css('background-image') != 'none';
    }
    var alphaRE=
      /rgba[(][0-9][0-9]*[ ]*,[ ]*[0-9][0-9]*[ ]*,[ ]*[0-9][0-9]*[ ]*,[ ]*([0-9][0-9]*)[ ]*[)]/;
    function zeroAlphaColour(c){
      var m=alphaRE.exec(''+c);
      return m && Number(m[1])==0;
    }
    // convert rgb(x,y,z) to #xyz
    // leave #xyz as #xyz
    function colourAsHex(c){
      if (c.search(/rgb/)==0){
	var r=c.split(',');
	r[0]=r[0].split('(')[1]
	r[2]=r[2].split(')')[0]
	
	c='#'+
	  hexChar[parseInt(r[0]/16)]+hexChar[r[0]%16]+
	  hexChar[parseInt(r[1]/16)]+hexChar[r[1]%16]+
	  hexChar[parseInt(r[2]/16)]+hexChar[r[2]%16];
      }
      return c;
    }
    
    var hexChar=[
      '0',
      '1',
      '2',
      '3',
      '4',
      '5',
      '6',
      '7',
      '8',
      '9',
      'A',
      'B',
      'C',
      'D',
      'E',
      'F'
    ];
    return result;
  })();

  // add context to e, like the code says
  wal.inContext=function(e, context){
    var em;
    if (typeof(e)=='object' && 
	wal.defined(e.name) &&
	wal.defined(e.message)){
      em=e.name+': '+e.message;
    }
    else{
      em=''+e;
    }
    // replace newlines with " " because IE won't let you scroll error
    // window or select the text, so we lose the stuff off the bottom
    // of the window
    return ('failed to '+context+' because '+em).replace("\n", " ");
  };
  // get keys of x
  // - if x is an array, returns its indices
  // - if x is an object, returns its keys
  wal.keys=function(x){
    return wal.map(x,function(k,v){return k;});
  }
  // map elements of x via function f
  // - if x is an array, calls f(i,v) for each v in x (i being its index)
  // - if x is an object, calls f(k,v) for each k:v in x
  // returns results in an array, which is a bit silly as logically you'd
  // think an object would map to an object
  wal.map=function(x, f){
    var result=[];
    wal.each(x, function(k, v){
      result.push(f(k, v));
    });
    return result;
  };
  wal.matchWholeString=function(s,regularExpression){
    var result=s.match(regularExpression);
    if (result && !result[0].length==s.length){
      return 'a'.match('b');
    }
    return result;
  };
  wal.max=function(a,b){
    return a>b?a:b;
  };
  wal.min=function(a,b){
    return a<b?a:b;
  };
  wal.now=function(){
    return Date.now();
  };
  // async HTTP-get url passing specified data (an object) according to raw:
  // - raw: passes data as a url-encoded query string
  //   (so data in this case must only have string values)
  // - not raw: json encodes data then passes that as the value of json_params
  //   url query string parameter
  // ... returns an object that has overridable functions, override them by:
  //   then(f) - on successful completion calls f(result)
  //             result is content (if raw) else... read the code
  //   or(f) - on failure f(s) where s is human readable string describing
  //           failure
  //   always(f) - after then/or function, calls f()
  //
  wal.getFromServer=function(url,data,raw){
    var result={
      then_: function(){},
      error_:function(e){
	alert(e);
      },
      always_:function(){
      }
    };
    result.then=function(then){
      result.then_=then;
      return result;
    }
    result.or=function(error){
      result.error_=error;
      return result;
    }
    result.always=function(always){
      result.always_=always;
      return result;
    }
    $.ajax({ 
      type: 'GET',
      url: url,
      data: raw?data:{
	'json_params':wal.json.encode(data)
      },
      dataType: 'text',
      cache:false,
      success: function(responseData_, status){
	if (raw){
	  result.then_(responseData_);
	  result.always_();
	  return;
	}
	var responseData;
	try{
	  responseData=wal.json.decode(responseData_);
	}
	catch(e){
	  result.error_(wal.inContext(''+e, 'get url '+url));
	  result.always_();
	  return;
	}
        if (typeof(responseData.error) != 'undefined' && 
	    responseData.error != '') {
	  result.error_(wal.inContext(''+responseData.error, 'get url '+url));
        }
        else {
          if (window.console&&console.log&&responseData.msg) {
	    console.log(''+responseData.msg);
	  };
	  result.then_(responseData.result);
        }
	result.always_();
      },
      error: function(jqXHR, status, e){
	result.error_(wal.inContext(''+e, 'get url '+url));
	result.always_();
      }
    });
    return result;
  };

  // join array of strings by separator string sep
  wal.join=function(sep, array){
    if (array.length==0){
      return '';
    }
    var result=''+array[0];
    for(var i=1; i != array.length; ++i){
      result=result+sep+array[i];
    }
    return result;
  }

  wal.json={};

  // encode o, using specified prefix as base indentation for all but first line
  // of encoding result (first line is not indented)
  wal.json.encode = function(o, prefix) {
    //if (typeof (JSON) == 'object' && JSON.stringify)
    //    return JSON.stringify(o);
    prefix=prefix||'';
    
    var type = typeof (o);
    
    if (o === null)
      return "null";
    
    if (type == "undefined")
      return "undefined";
    
    if (type == "number" || type == "boolean")
      return o;
    
    if (type == "string")
      return quoteString(o);
    
    if (type == 'object') {
      if (typeof o.toJSON == "function")
        return o.toJSON(prefix);
      
      if (o.constructor === Array) {
        var ret = [];
	var skipped=0;
        for ( var i = 0; i < o.length; i++){
	  if (typeof(o[i])=='function'){
	    ++skipped;
	  }
	  else if (typeof(o[i])=='undefined'){
	    ret.push("\n"+prefix+"\t"+wal.json.encode(null, prefix+"\t"));
	  }
	  else {
            ret.push("\n"+prefix+"\t"+wal.json.encode(o[i], prefix+"\t"));
	  }
	}
	if (skipped && skipped != o.length){
	  throw new String(
	    'wal.json.encode array has mixture of functions and data');
	}
        return "[" + ret.join(",") + "\n"+prefix+"]";
      }
      
      var pairs = [];
      for ( var k in o) {
        var name;
        var type = typeof k;
	
        if (type == "number")
          name = '"' + k + '"';
        else if (type == "string")
          name = quoteString(k);
        else
          continue; // skip non-string or number keys
	
        if (typeof o[k] == "function")
          continue; // skip pairs where the value is a function.
	if (!wal.defined(o[k])){
	  continue; // skip pairs where the value is undefined
	}
        var val = wal.json.encode(o[k], prefix+"\t");
	
        pairs.push("\n"+prefix+"\t"+name + ": " + val);
      }
      
      return "{" + pairs.join(",") + "\n"+prefix+"}";
    }
  };
  
  //
  // Decode JSON encoded string.
  //
  wal.json.decode = function(src) {
    try{
      return eval('('+src+')');
    }
    catch(e){
      var i=new Iterator(src);
      return i.parse();
    }
  }
 
  var quoteString=function(string) {
        if (string.match(_escapeable)) {
            return '"' + string.replace(_escapeable, function(a) {
                var c = _meta[a];
                if (typeof c === 'string')
                    return c;
                c = a.charCodeAt();
                return '\\u00' + Math.floor(c / 16).toString(16)
                        + (c % 16).toString(16);
            }) + '"';
        }
        return '"' + string + '"';
  };
 
  var _escapeable= /["\\\x00-\x1f\x7f-\x9f]/g;

  var _meta= {
        '\b': '\\b',
        '\t': '\\t',
        '\n': '\\n',
        '\f': '\\f',
        '\r': '\\r',
        '"': '\\"',
        '\\': '\\\\'
  };

  var Iterator=function(s){
    this.val=s;
    this.line=1;
    this.char=1;
  }
  Iterator.prototype.lineAndChar=function(){
    return 'line ' + this.line + ', char ' + this.char;
  }
  // Advance past s, which is assumed to exist at start
  // of this.val.
  Iterator.prototype.advance=function(s){
    this.val=this.val.slice(s.length);
    var m=0;
    var i=0;
    while((m=s.indexOf('\n',i))!=-1){
      ++this.line;
      this.char=0;
      i=m+1;
    }
    this.char+=s.length-i;
  }
  // Advance n chars assuming none of those are newlines
  Iterator.prototype.advanceInLine=function(n){
    this.val=this.val.slice(n);
    this.char+=n;
  }
  // Parse to first occurrance of re.
  // - if re not found, advance to end if allowEnd is true; otherwise
  //   raise exception
  // - return up to resulting point (as string)
  Iterator.prototype.parseTo=function(re, allowEnd){
    var m=this.val.search(re);
    var result='';
    if (m != -1){
      result=this.val.substr(0,m);
      this.advance(result);
    }
    else {
      if (allowEnd){
	result=this.val;
	this.advance(result);
      }
      else{
	throw re.pattern+' not found aftr '+this.lineAndChar();
      }
    }
    return result;
  }
  // Skip leading whitespace.
  Iterator.prototype.skipWhite=function(){
    this.parseTo(whiteRE, true);
    return this;
  }
  // Parse literal l which is assumed to exist at start of this.val.
  // - does not skip subsequent whitespace
  // - assumes l contains no newlines
  Iterator.prototype.parseLiteral=function(l){
    if (this.val.substr(0,l.length)!=l){
      throw l+' not found at '+this.lineAndChar();
    }
    this.advanceInLine(l.length);
    return this;
  }
  // Parse number assumed to exist at start of this.val.
  // - returns number, and advances past it and any subsequent whitespace
  Iterator.prototype.parseNumber=function(){
    var n=this.parseTo(numberRE, true);
    if (n==''){
      throw 'invalid number at ' + this.lineAndChar()
    }
    this.skipWhite();
    return Number(n);
  }
  // Parse string assumed to exist at start of this.val.
  // - returns string, and advances past it and any subsequent whitespace
  //pre: iterator positioned at opening '"'
  Iterator.prototype.parseString=function(){
    this.parseLiteral('"');
    var result=this.parseTo(parseStringRE,false);
    while(this.val.length && this.val.charAt(0) == "\\"){
      switch(this.val.charAt(0)){
      case 'b':
	result+='\b';
	this.advanceInLine(2);
	break;
      case 't':
	result+='\t';
	this.advanceInLine(2);
	break;
      case 'n':
	result+='\n';
	this.advanceInLine(2);
	break;
      case 'f':
	result+='\f';
	this.advanceInLine(2);
	break;
      case 'r':
	result+='\r';
	this.advanceInLine(2);
	break;
      case '"':
	result+='"';
	this.advanceInLine(2);
	break;
      case 'u':
	if (this.val.length < 5){
	  result+=this.val;
	  this.advance(this.val);
	}
	else{
	  result+=String.fromCharCode(parseInt(this.val.substr(2,4), 16));
	  this.advanceInLine(6);
	}
	break;
      default:
	result+=this.val.charAt(1);
	this.advanceInLine(2);
	break;
      }
      result=result+this.parseTo(parseStringRE,false);
    }
    if (this.val.charAt(0) != '"'){
      throw 'unterminated string at '+this.lineAndChar();
    }
    this.parseLiteral('"');
    this.skipWhite();
    return result;
  }
  // Parse entity assumed to exist at start of this.val
  // - returns parsed entity, and advances past it and any subsequent whitepspace
  //pre: whitespace has been skipped
  Iterator.prototype.parse=function(){
    switch(this.val.charAt(0)){
    case '"':
      return this.parseString();
    case '[':
      return this.parseArray();
    case '{':
      return this.parseObject();
    case 'n':
      this.parseLiteral('null');
      this.skipWhite();
      return null;
    case 't':
      this.parseLiteral('true');
      this.skipWhite();
      return true;
    case 'f':
      this.parseLiteral('false');
      this.skipWhite();
      return false;
    default:
      return this.parseNumber();
    }
  }
  // Parse array assumed to exist at start of this.val.
  // - returns array, and advances past it and any subsequent whitespace
  //pre: iterator positioned at opening '['
  Iterator.prototype.parseArray=function(){
    this.parseLiteral('[');
    this.skipWhite();
    var result=[],x;
    while(this.val.length && this.val.charAt(0) != ']'){
      result.push(this.parse());
      if (this.val.charAt(0)==','){
	this.parseLiteral(',');
	this.skipWhite();
      }
    }
    if (this.val.charAt(0) != ']'){
      throw 'unterminated array at '+this.lineAndChar();
    }
    this.parseLiteral(']');
    this.skipWhite();
    return result;
  }
  // Parse object assumed to exist at start of this.val.
  // - returns object, and advances past it and any subsequent whitespace
  // pre: iterator positioned at opening '{'
  Iterator.prototype.parseObject=function(){
    this.parseLiteral('{');
    this.skipWhite();
    var result={};
    while(this.val.length && this.val.charAt(0) != '}'){
      var k=this.parse();
      if (typeof(k)!='string' && typeof(k)!='number'){
	throw 'invalid key type '+typeof(k)+' at '+this.lineAndChar();
      }
      if (this.val.charAt(0) != ':'){
	throw 'missing ":" at '+this.lineAndChar();
      }
      this.parseLiteral(':');
      this.skipWhite();
      var val=this.parse();
      result[k] = val;
      if (this.val.charAt(0)==','){
	this.parseLiteral(',');
	this.skipWhite();
      }
    }
    if (this.val.charAt(0) != '}'){
      throw 'missing end } at ' + this.lineAndChar();
    }
    this.parseLiteral('}');
    this.skipWhite();
    return result;
  }

  var lineRE=/[\n]/m;
  var whiteRE = /[^ \t\r\n]/m;
  var numberRE=/[^+\-0-9.eE]/;
  var parseStringRE=/[\\"]/;

  var $load_image=false;

  wal.postToServer=function(url,data,sync,raw){
    var result={
      then_: function(){},
      error_:function(e){
	alert(e);
      },
      always_:function(){
      }
    };
    result.then=function(then){
      result.then_=then;
      return result;
    };
    result.or=function(error){
      result.error_=error;
      return result;
    };
    result.always=function(always){
      result.always_=always;
      return result;
    }
    $.ajax({ 
      type: 'POST',
      url: url,
      data: raw?data:{
	'json_params':wal.json.encode(data)
      },
      dataType: 'text',
      async: !sync,
      success: function(responseData_, status){
	var responseData;
	if (raw){
	  result.then_(responseData_);
	  result.always_();
	  return;
	}
	try{
	  responseData=wal.json.decode(responseData_);
	}
	catch(e){
	  result.error_(''+e);
	  result.always_();
	  return;
	}
        if (typeof(responseData.error) != 'undefined' && 
	    responseData.error != '') {
	  result.error_(''+responseData.error);
        }
        else if (typeof(responseData.result.error) != 'undefined' && 
	    responseData.result.error != '') {
	  result.error_(''+responseData.result.error);
        }
        else {
          if (window.console&&console.log&&responseData.msg) {
	    console.log(''+responseData.msg);
	  };
	  result.then_(responseData.result);
        }
	result.always_();
      },
      error: function(jqXHR, status, e){
	result.error_(wal.inContext(''+e, 'post data to '+url));
	result.always_();
      }
    });
    return result;
  };
  wal.rendering=function($x){
    var $overlay=$('<div class="wal-busy-cursor">&nbsp;</div>').css({
      position:'absolute',
      left:0,
      top:0,
      width:'100%',
      height:'100%'})
      .css(wal.getBackground($x));
    $x.append($overlay);
    $x.addClass('wal-rendering');
    return {
      done:function(){
	$overlay.remove();
	$x.removeClass('wal-rendering');
      }
    };
  };
  wal.showElement=function($element,topOffset,then,duration){
    var viewport={
      top:$('html,body').scrollTop()+(topOffset||0),
      left:$('html,body').scrollLeft(),
      height:$(window).height()-(topOffset||0),
      width:$(window).width()
    };
    var newWindowTop=$('html,body').scrollTop();
    var newWindowLeft=$('html,body').scrollLeft();

    var scrollVert=function(top){
      newWindowTop=top-(topOffset||0);
    };
    var scrollHoriz=function(left){
      newWindowLeft=left;
    };
    var element=wal.extend(
      {
	width:$element.outerWidth(),
	height:$element.outerHeight()
      },
      $element.offset());
    duration=duration||500;
    if (element.top<viewport.top){
      scrollVert(element.top);
    }
    else if (element.top+element.height>viewport.top+viewport.height){
      scrollVert(wal.min(
	element.top,
	element.top+element.height-viewport.height));
    }
    if (element.left<viewport.left){
      scrollHoriz(element.left);
    }
    else if (element.left+element.width>viewport.left+viewport.width){
      scrollHoriz(wal.min(
	element.left,
	element.left+element.width-viewport.width));
    }
    $('html,body').animate({scrollTop:newWindowTop,
			    scrollLeft:newWindowLeft},duration,
			   then||function(){});
  };
  wal.showError=function(e){
    alert(''+e);
  };
  // call set(val) whenever input changes
  // - returns f() that stops tracking input
  wal.trackTextInput=function($input, set/*f(val:string)*/){
    var last;
    var delayedSet;
    function set_(){
      var now=$input.val();
      if (now != last){
	last = now;
	set(now);
      }
      delayedSet();
    }
    var timer;
    delayedSet=function(){
      timer&&clearTimeout(timer);
      timer=setTimeout(set_, 100);
    }
    // change is not relyable, some browsers don't trigger in all cases
    // e.g. mouse-paste
    $input.bind('change', delayedSet);
    // note keypress is no good as IE and webkit don't send eg on backspace
    $input.bind('keyup', delayedSet);
    // note keypress is no good as IE and webkit don't send eg on backspace
    $input.bind('mouseup', delayedSet);
    // and in the end only poll is foolproof
    delayedSet();
    return function(){
      timer&&clearTimeout(timer);
      $input.unbind('mouseup', delayedSet);
      $input.unbind('keyup', delayedSet);
      $input.unbind('change', delayedSet);
    }
  }
  wal.getRadioButtonValue=function($radioButtons){
    var result;
    $radioButtons.each(function(){
      if ($(this).prop('checked')){
	result=$(this).prop('value');
      }
    });
    return result;
  };
  wal.setCookie=function(name,value){
    document.cookie=name+'='+encodeURIComponent(wal.json.encode(value));
    return value;
  };
  wal.getCookie=function(name){
    var x=document.cookie.split('; ');
    var result;
    wal.each(x,function(i,y){
      var nv=y.split('=');
      if (nv[0]==name){
	result=wal.json.decode(decodeURIComponent(nv[1]));
      }
    });
    return result;
  };
  wal.set=function(arrayOfStrings){
    result={};
    wal.each(arrayOfStrings,function(i,member){
      result[member]=true;
    });
    return result;
  };
  // return sorted copy of array_
  wal.sorted=function(array_,f){
    var result=array_.slice();
    result.sort(f);
    return result;
  };
  wal.compare=function(a,b){
    if (a<b){
      return -1;
    }
    if (a>b){
      return 1;
    }
    return 0;
  };
  //return reason why s is not an email address, or false
  //if s is an email address
  wal.isNotAnRFC2822EmailAddress=function(s){
    var at=0;
    var end=s.length;
    var parseDotAtom=function(at){
      var m=s.slice(at).match(/^[^.@"\s]+([.][^.@"\s]+)*/);
      if (m&&m.length){
	at=at+m[0].length;
	return at;
      }
      throw('invalid domain at character '+(at+1));
    };
    var parseQuoted=function(at){
      var m=s.slice(at).match(/^"([\\].|[^"\r\n])*/);
      at=at+m[0].length;
      if (s.charAt(at)!='"'){
	throw('invalid "quoted" name part at character '+(at+1))
      }
      return at+1;
    };
    var parseUnquotedLocal=function(at){
      var m=s.slice(at).match(/^[^@\s]+/);
      if (m&&m[0].length){
	at=at+m[0].length;
	return at;
      }
      throw('invalid unquoted name part at character '+(at+1));
    };
    var parseLocalPart=function(at){
      if (s.charAt(at)=='"'){
	return parseQuoted(at);
      }
      at=parseUnquotedLocal(at);
      if (s.charAt(at)!='@'){
	throw('invalid "dotted" name part at character '+(at+1));
      }
      return at;
    };
    var parseChar=function(c,at){
      if (s.charAt(at)!=c){
	throw('expected "'+c+'" at character '+(at+1)+' but got "'+
	      s.charAt(at));
      }
      return at+1;
    };
    var parseLiteralDomain=function(at){
      var m=s.slice(at).match(/^\[[^\]]+/);
      at=at+(m||[''])[0].length;
      if (s.charAt(at)!=']'){
	throw('invalid "literal" domain part at character '+(at+1));
      }
      return at+1;
    };
    var parseDomain=function(at){
      if (s.charAt(at)=='['){
	return parseLiteralDomain(at);
      }
      at=parseDotAtom(at);
      return at;
    };
    try{
      at=parseLocalPart(at);
      at=parseChar('@',at);
      at=parseDomain(at);
      if (at!=end){
	throw('invalid domain at character '+(at+1));
      }
      return false;
    }
    catch(e){
      return ''+e;
    }
  };
  wal.valuesEqual=function(a,b)
  {
    var type = typeof (a);
    if (type != typeof(b)){
      return false;
    }
    if (a === null)
      return b === null;
    
    if (type == "undefined")
      return !wal.defined(b);
    
    if (type == "number" || type == "boolean" || type == "string")
      return a==b;
    
    if (type == 'object') {
      if (a.constructor === Array) {
	if (!(b.constructor === Array)){
	  return false;
	}
	if (a.length!=b.length){
	  return false;
	}
	var i;
	for(i=0; i!=a.length; ++i){
	  if (!wal.valuesEqual(a[i],b[i])){
	    return false;
	  }
	}
	return true;
      }
      if (!wal.valuesEqual(wal.sorted(wal.keys(a)),wal.sorted(wal.keys(b)))){
	return false;
      }
      var result=true;
      wal.each(wal.keys(a),function(i,k){
	if (!wal.valuesEqual(a[k],b[k])){
	  result=false;
	}
      });
      return result;
    }
  };
}( window.wal = window.wal || {} ));
