INCLUDE "gmc/nod.h"

SCANNER
  EOF		= EOFTOK
  ERR		= ERRTOK
  "Word"	= WORDTK	=> "WORD"
  "EPString"	= SCAN_HOSTWD	=> "SCAN_HOSTWD"
  "ObjectID"	= SCAN_OBJTID	=> "SCAN_OBJTID"
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
