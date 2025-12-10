#ifndef TP_YYNODTYP
#define TP_YYNODTYP

typedef enum NodTyp {
  PROGRM=1,
  NOD_Include=2,
  SCANNR=3,
  EOFLDF=4,
  ERRLDF=5,
  NONLDF=6,
  LEAFDF=7,
  NODES=8,
  RULES=9,
  RULE=10,
  ALTLST=11,
  ALTRNT=12,
  SEQ=13,
  LIST=14,
  PLUS=15,
  STAR=16,
  QUESTION=17,
  OPTNAL=18,
  OUTNOD=19,
  NAME=20,
  DSTRNG=21,
  KSTRNG=22
} tp_NodTyp;

#endif
