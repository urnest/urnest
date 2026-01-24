#ifndef TP_INPKIND
#define TP_INPKIND

enum InpKind {
  IK_Simple =1,	/* depends on input obj */
  IK_Trans =2,	/* depends on all elements of input obj */
  IK_Name =3,	/* depends on the name of the input object */
  IK_TransName =4,	/* depends on the names of all elements */
  IK_Pntr =5,	/* IK_Simple, but Pntrs are not deref'ed */
  IK_AnyOK =6	/* IK_Trans, but can have error status */
};

#endif
