from typing import TypeVar,Literal,Generic,Callable,cast
from dataclasses import dataclass
from abc import ABC,abstractmethod
import re

Rest=str
EatWhite=Callable[[Rest],Rest]
V = TypeVar('V')
Parser=Callable[[Rest,EatWhite],None|tuple[V,Rest]]
PR=None|tuple[V,Rest]

def eat_multi_line_whitespace(
        rest:Rest,r=re.compile(r'[ \t\n]*',flags=re.M),rl=re.compile(r'[^\n]*')
) -> Rest:
    while rest:
        match rest[0]:
            case ' ' | '\t' | '\n':
                rest=rest[r.match(rest).end():]
            case '#':
                rest=rest[rl.match(rest).end():]
            case '\\':
                if rest.startswith('\\\n'):
                    rest=rest[2:]
                else:
                    return rest
            case _:
                return rest
        pass
    return rest

            
def eat_same_line_whitespace(rest:Rest,r=re.compile(r'[ \t]*'),rl=re.compile(r'[^\n]*')) -> Rest:
    while rest:
        match rest[0]:
            case ' ' | '\t':
                rest=rest[r.match(rest).end():]
            case '#':
                return rest[rl.match(rest).end():]
            case '\\':
                if rest.startswith('\\\n'):
                    rest=rest[2:]
                else:
                    return rest
            case _:
                return rest
        pass
    return rest


def parse_rest_of_line(rest:Rest,eat_white:EatWhite) -> PR[str]:
    try:
        i=rest.index('\n')
    except ValueError:
        return rest,''
    else:
        return rest[:i],eat_multi_line_whitespace(rest[i:])
    pass

def parse_open_bracket(rest:Rest,eat_white:EatWhite)->PR[Literal['(']]:
    if rest.startswith('('):
        return '(',eat_white(rest[1:])
    return None

def parse_close_bracket(rest:Rest,eat_white:EatWhite)->PR[Literal[')']]:
    if rest.startswith(')'):
        return ')',eat_white(rest[1:])
    return None

U = TypeVar('U')
W = TypeVar('W')

@dataclass
class NonEmptyListParser(Generic[V,U]):
    parse_item: Parser[V]
    parse_separator: Parser[U]
    def parse(self,rest:str,eat_white:EatWhite) -> PR[list[V]]:
        i=self.parse_item(rest,eat_white)
        if i is not None:
            v,rest=i
            result:list[V] = [ v ]
            while rest:
                j=self.parse_separator(rest,eat_white)
                if j is None:
                    return result,rest
                else:
                    i=self.parse_item(j[1],eat_white)
                    if i is None:
                        return None
                    v,rest=i
                    result.append(v)
                    pass
                pass
            return result,rest
        return None
    pass

@dataclass
class ListParser(Generic[V,U,W]):
    '''optional trailing separator, e.g. 1,2,3,] or 1,2,3]; assumes opener has already been parsed'''
    parse_item:Parser[V]
    parse_separator:Parser[U]
    parse_terminator:Parser[W]
    def parse(self,rest:Rest,eat_white:EatWhite) -> PR[list[V]]:
        '''eat_white here is only used after the terminator because lists can always be multi-line'''
        result:list[V]=[]
        while rest:
            t=self.parse_terminator(rest,eat_white)
            if t is not None:
                return result,t[1]
            i=self.parse_item(rest,eat_multi_line_whitespace)
            if i is None:
                return None
            v,rest=i
            result.append(v)
            t=self.parse_terminator(rest,eat_white)
            if t is not None:
                return result,t[1]
            s=self.parse_separator(rest,eat_multi_line_whitespace)
            if s is None:
                return None
            rest=s[1]
        return None
    pass

@dataclass
class KeywordParser:
    k: str
    r=re.compile(r'[a-zA-Z0-9_]')
    def parse(self,rest:Rest,eat_white:EatWhite)->PR[str]:
        if rest.startswith(self.k) and not self.r.match(rest[len(self.k):]):
            return self.k,eat_white(rest[len(self.k):])
        return None
    pass

@dataclass(frozen=True)
class Identifier:
    value:str
    r=re.compile(r'[a-zA-Z_][_a-zA-Z0-9]*')
    def __post__init__(self):
        assert self.r.match(self.value),(self.r.pattern,self.value)
        pass
    def __str__(self):
        return self.value
    pass

def parse_identifier(
        rest:Rest,
        eat_white:EatWhite
) -> PR[Identifier]:
    match Identifier.r.match(rest):
        case re.Match() as m:
            return Identifier(rest[:m.end()]),eat_white(rest[m.end():])
    return None

    
@dataclass(frozen=True)
class AbsName:
    parts:tuple[Identifier, ...]
    def __str__(self):
        return '.'.join([str(part) for part in self.parts])
    pass

def parse_dot(rest:Rest,eat_white:EatWhite)->PR[Literal['.']]:
    if rest.startswith('.'):
        return '.',eat_white(rest[1:])
    return None

def parse_abs_name(rest:Rest,eat_white:EatWhite,
                   p=NonEmptyListParser[Identifier,Literal['.']](parse_identifier,parse_dot).parse)->PR[AbsName]:
    r=p(rest,eat_white)
    if r is not None:
        parts,rest=r
        return AbsName(tuple(parts)),rest
    return None

@dataclass(frozen=True)
class RelName:
    levels_up: int  # note import .x -> levels_up == 0
    parts:tuple[Identifier, ...]
    def __post__init(self):
        assert self.levels_up>=0,self
        pass
    def __str__(self):
        return '.'.join(['']*(1+self.levels_up) +
                        [str(part) for part in self.parts])
    pass

def parse_rel_name(rest:Rest,eat_white:EatWhite)->PR[RelName]:
    match parse_dot(rest,eat_white):
        case '.',rest:
            levels_up=0
            while True:
                match parse_dot(rest,eat_white):
                    case None:
                        break
                    case '.',rest:
                        levels_up+=1
                        pass
                pass
            match parse_abs_name(rest,eat_white):
                case AbsName() as absname, str(rest):
                    return RelName(levels_up,absname.parts),rest
            pass
    return None

def parse_name(rest:Rest,eat_white:EatWhite)->PR[AbsName|RelName]:
    name:AbsName|RelName
    match parse_abs_name(rest,eat_white):
        case AbsName() as name,str() as rest:
            return name,rest
    match parse_rel_name(rest,eat_white):
        case RelName() as name,str(rest):
            return name,rest
    return None

parse_keyword_as=KeywordParser('as').parse

def parse_plain_clause(rest:Rest,eat_white:EatWhite) -> PR[tuple[AbsName|RelName,Identifier|None]]:
    name:AbsName|RelName
    match parse_name(rest,eat_white):
        case AbsName()|RelName() as name,str(rest):
            match parse_keyword_as(rest,eat_white):
                case None:
                    return (name,None),rest
                case _,rest:
                    match parse_identifier(rest,eat_white):
                        case Identifier() as identifier,str(rest):
                            return (name,identifier),rest
                    pass
            pass
    return None

def parse_from_clause(rest:Rest,eat_white:EatWhite) -> PR[tuple[Identifier,Identifier|None]]:
    match parse_identifier(rest,eat_white):
        case Identifier() as name,str(rest):
            match parse_dot(rest,eat_white):
                case '.',str(rest):
                    return None
            match parse_keyword_as(rest,eat_white):
                case None:
                    return (name,None),rest
                case _,rest:
                    match parse_identifier(rest,eat_white):
                        case Identifier() as identifier,str(rest):
                            match parse_dot(rest,eat_white):
                                case '.',str(rest):
                                    return None
                            return (name,identifier),rest
                    pass
            pass
    return None

parse_keyword_import=KeywordParser('import').parse

def parse_comma(rest:Rest,eat_white:EatWhite)->PR[Literal[',']]:
    if rest.startswith(','):
        return ',',eat_white(rest[1:])
    return None

@dataclass
class PlainImport:
    clauses: dict[AbsName|RelName, Identifier|None]
    pass

def parse_plain_import(
        rest:Rest,eat_white:EatWhite,
        parse_bracketed_clauses=ListParser(parse_plain_clause,parse_comma,parse_close_bracket).parse,
        parse_unbracketed_clauses=NonEmptyListParser(parse_plain_clause,parse_comma).parse
)->PR[PlainImport]:
    match parse_keyword_import(rest,eat_white):
        case _,rest:
            match parse_open_bracket(rest,eat_same_line_whitespace):
                case _,rest:
                    match parse_bracketed_clauses(rest,eat_multi_line_whitespace):
                        case clauses,rest:
                            if rest and not rest.startswith('\n'):
                                return None
                            return PlainImport(dict(clauses)),rest
                case None:
                    match parse_unbracketed_clauses(rest,eat_white):
                        case clauses,rest:
                            if rest and not rest.startswith('\n'):
                                return None
                            return PlainImport(dict(clauses)),rest
            pass
    return None

@dataclass
class FromImport:
    module: AbsName|RelName
    clauses: dict[Identifier,Identifier|None]  # x [as y] -> x: y/None
    pass

parse_keyword_from=KeywordParser('from').parse

def parse_from_import(
        rest:Rest,eat_white:EatWhite,
        parse_bracketed_clauses=ListParser(parse_from_clause,parse_comma,parse_close_bracket).parse,
        parse_unbracketed_clauses=NonEmptyListParser(parse_from_clause,parse_comma).parse
)->PR[FromImport]:
    match parse_keyword_from(rest,eat_white):
        case _,rest:
            match parse_name(rest,eat_white):
                case AbsName()|RelName() as module,str(rest):
                    match parse_keyword_import(rest,eat_white):
                        case _,rest:
                            match parse_open_bracket(rest,eat_multi_line_whitespace):
                                case _,rest:
                                    match parse_bracketed_clauses(rest,eat_same_line_whitespace):
                                        case clauses,rest:
                                            if rest and not rest.startswith('\n'):
                                                return None
                                            return FromImport(module,dict(clauses)),rest
                                case None:
                                    match parse_unbracketed_clauses(rest,eat_white):
                                        case clauses,rest:
                                            if rest and not rest.startswith('\n'):
                                                return None
                                            return FromImport(module,dict(clauses)),rest
                    pass
            pass
    return None

def eat_logical_line(rest:Rest, r=re.compile('[^\\\\]*')) -> Rest:
    '''remove logical line from start of rest'''
    '''- includes continations via lines ending in backslash'''
    while rest:
        match r.match(rest.split('\n')[0]):
            case None:
                if rest.startswith('\n'):
                    return rest[1:]
            case re.Match() as m:
                rest=rest[m.end():]
                if rest.startswith('\\\n'):
                    rest=rest[2:]
                elif rest.startswith('\n'):
                    return rest[1:]
                else:
                    rest=rest[1:]
                pass
        pass
    return rest
        
def parse_python_source_code_for_imports(source_code:str) -> tuple[list[PlainImport],
                                                                   list[FromImport]]:
    plain_imports:list[PlainImport] = []
    from_imports:list[FromImport] = []
    at=eat_multi_line_whitespace(source_code)
    while(at):
        rest=eat_same_line_whitespace(at)
        match parse_from_import(rest,eat_same_line_whitespace):
            case FromImport() as f, str(rest):
                from_imports.append(f)
            case None:
                match parse_plain_import(rest,eat_same_line_whitespace):
                    case PlainImport() as p, str(rest):
                        plain_imports.append(p)
                    case None:
                        rest=eat_logical_line(rest)
        rest=eat_multi_line_whitespace(rest)
        assert len(rest)<len(at), rest[:50]
        at=rest
    return plain_imports, from_imports

def format_plain_imported_modules(plain_import: PlainImport) -> list[str]:
    '''format plain import {plain_import} to its import module list'''
    '''e.g. -> [ 'a.b.c', '..e.f' ]'''
    return [str(name) for name in plain_import.clauses.keys()]

def format_from_imported_modules(from_import: FromImport) -> list[str]:
    '''format from import {from_import} to its import module list'''
    '''e.g. -> [ 'a.b.c', '..e.f' ]'''
    return [f'{from_import.module}.{name}' for name in from_import.clauses.keys()]
pass
