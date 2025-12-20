SCANNER
  EOF		= EOFTOK
  ERR		= ERRTOK
  "Word"	= WORDTK	=> "WORD"
  "EPString"	= SCAN_HOSTWD	=> "HOSTWD"
  "ObjectID"	= SCAN_OBJTID	=> "OBJTID"
  '!'		= EXCLPT
  ';'		= SCOLON
  '?'		= QUESMK
  '%'		= PERCNT
  '<'		= LANGLE
  '>'		= RANGLE
  ':'		= COLON
  '('		= LPAREN
  ')'		= RPAREN
  '+'		= PLUS
  '='		= EQUALS
  '/'		= SLASH
