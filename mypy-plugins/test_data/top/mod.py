#
# Copyright (c) 2023 Trevor Taylor
# 
# Permission to use, copy, modify, and/or distribute this software for
# any purpose with or without fee is hereby granted, provided that all
# copyright notices and this permission notice appear in all copies.
# 
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
from dataclasses import dataclass
from typing import Literal, TypeVar, Generic
from xju.json_codec import codec

AT = tuple[int, str]

@dataclass
class BT:
    x: AT
    y: Literal[7, 8]

T = TypeVar("T")
TT = tuple[int, str, T]
X = TT[float]
U = TypeVar("U")
class G1(Generic[T, U]):
    x: T
    y: U

P = G1[int, U]
Q = P[float]

#codec(BT).encode(BT( (1, 'fred'), 8))  # NameExpr -> TypeInfo("top.mod.BT")
                                        #    no type_vars
                                        #    names x and y, each is a Var

#codec(X).encode( (1, 'fred', 3.6) )    # NameExpr -> TypeAlias with no tvars .target is a TupleType
                                        # i.e. intermediate TT has been resolved completely

#codec(TT[float]).encode( (1, 'fred', 3.6) )  # IndexExpr:
                                              #   base NameExpr -> TypeAlias TupleType[int,str,T'1]
                                              #   index NameExpr -> TypeInfo("float")

#codec(G1[int, str]).encode( G1(7, 'jock') )   # IndexExpr:
                                              #   base NameExpr -> TypeInfo("G1")
                                              #     type_vars T, U
                                              #     names x Var .type T`1, y Var .type U`1
                                              #   index TupleExpr .items:
                                              #     NameExpr -> TypeInfo("int")
                                              #     NameExpr -> TypeInfo("str")

#codec(P[float]).encode(G1(8, 8.8))            # IndexExpr:
                                              #   base NameExpr -> TypeAlias with
                                              #      alias_tvars [TypeVarType(U`1)],
                                              #      target Instance("G1", [int, U`1]
                                              #   index NameExpr -> TypeInfo("float")

#codec(Q).encode(G1(8, 8.8))                   # NameExpr "Q" -> TypeAlias
                                               #     alias_tvars []
                                               #     target TypeAliasType
                                               #       alias TypeAlias
                                               #         alias_tvars [TypeVarType(U`1)],
                                               #         target Instance("G1", [int, U`1])
                                               #       args [Instance(float, []]
#codec(P[X]).encode(G1(9, (1, 'fred', 3.8)))    # IndexExpr:
                                               #   base NameExpr "P" -> TypeAlias
                                               #       alias_tvars  [TypeVarType(U`1)],
                                               #       target Instance(G1, [int, U`1])
                                               #   index NameExpr "X" -> TypeAlias
                                               #            alias_tvars []
                                               #            target TupleType[int, str, float]
# codec(P[TT[float]]).encode(G1(9, (1, 'fred', 3.8)))  # IndexExpr
                                                     #    base NameExpr -> TypeAlias
                                                     #         alias_tvars [U`1]
                                                     #         target G1[int, U`1]
                                                     #    index IndexExpr
                                                     #         base NameExpr -> TypeAlias
                                                     #           TupleType[int, str, T`1]
                                                     #         index NameExpr -> Instance(float, [])

V = TypeVar("V")
class G2(Generic[V]):
    x: G1[V, int]

codec(G2[float]).encode(G2(G1(9, 8.3)))   # IndexExpr
                                          #   base NameExpr -> TypeInfo('G2', type_vars[V`1])
                                          #       names x Var .type Instance(G1, [V`1, int])
                                          #   index NameExpr float


# should not be any errors, because codec(dict) should become
# Codec(dict[str, JsonType])
D=dict
d = codec(D).decode({'fred':5, 'jock': 'nine'})
dd: JsonType = d['fred']
