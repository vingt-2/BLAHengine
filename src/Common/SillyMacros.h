#define LPAREN (
#define RPAREN )
#define COMMA ,
#define SBRACL [
#define SBRACR ]

#define str(a) xstr(a)
#define xstr(a) #a

#define IDT(x) x
#define CAT(A, B) _CAT(A,B)
#define _CAT(A, B) A ## B
#define EXPAND(...) __VA_ARGS__

#define EAT(a)

// Make an enumarator macro
#define EN_1(macro, count, X) macro((-1) * (1 - count))
#define EN_2(macro, count, X, ...) macro((-1) * (2 - count)), EN_1(macro, count, __VA_ARGS__)
#define EN_3(macro, count, X, ...) macro((-1) * (3 - count)), EN_2(macro, count,__VA_ARGS__)
#define EN_4(macro, count, X, ...) macro((-1) * (4 - count)), EN_3(macro, count,__VA_ARGS__)
#define EN_5(macro, count, X, ...) macro((-1) * (5 - count)), EN_4(macro, count,__VA_ARGS__)
#define EN_6(macro, count, X, ...) macro((-1) * (6 - count)), EN_5(macro, count,__VA_ARGS__)
#define EN_7(macro, count, X, ...) macro((-1) * (7 - count)), EN_6(macro, count,__VA_ARGS__)
#define EN_8(macro, count, X, ...) macro((-1) * (8 - count)), EN_7(macro, count,__VA_ARGS__)
#define EN_9(macro, count, X, ...) macro((-1) * (9 - count)), EN_8(macro, count,__VA_ARGS__)
#define EN_10(macro, count, X, ...) macro((-1) * (10 - (count)), EN_9(macro, count, __VA_ARGS__)
#define EN_11(macro, count, X, ...) macro((-1) * (11 - (count)), EN_10(macro, count, __VA_ARGS__)
#define EN_12(macro, count, X, ...) macro((-1) * (12 - (count)), EN_11(macro, count, __VA_ARGS__)
#define EN_13(macro, count, X, ...) macro((-1) * (13 - (count)), EN_12(macro, count, __VA_ARGS__)
#define EN_14(macro, count, X, ...) macro((-1) * (14 - (count)), EN_13(macro, count, __VA_ARGS__)
#define EN_15(macro, count, X, ...) macro((-1) * (15 - (count)), EN_14(macro, count, __VA_ARGS__)
//... repeat as needed

// Enumerator with prefix macro
#define EWPM_1(pre_macro, en_macro, count, X) pre_macro(X) en_macro((-1) * (1 - count))
#define EWPM_2(pre_macro, en_macro, count, X, ...) pre_macro(X) en_macro((-1) * (2 - count)), EWPM_1(pre_macro, en_macro, count, __VA_ARGS__)
#define EWPM_3(pre_macro, en_macro, count, X, ...) pre_macro(X) en_macro((-1) * (3 - count)), EWPM_2(pre_macro, en_macro, count,__VA_ARGS__)
#define EWPM_4(pre_macro, en_macro, count, X, ...) pre_macro(X) en_macro((-1) * (4 - count)), EWPM_3(pre_macro, en_macro, count,__VA_ARGS__)
#define EWPM_5(pre_macro, en_macro, count, X, ...) pre_macro(X) en_macro((-1) * (5 - count)), EWPM_4(pre_macro, en_macro, count,__VA_ARGS__)
#define EWPM_6(pre_macro, en_macro, count, X, ...) pre_macro(X) en_macro((-1) * (6 - count)), EWPM_5(pre_macro, en_macro, count,__VA_ARGS__)
#define EWPM_7(pre_macro, en_macro, count, X, ...) pre_macro(X) en_macro((-1) * (7 - count)), EWPM_6(pre_macro, en_macro, count,__VA_ARGS__)
#define EWPM_8(pre_macro, en_macro, count, X, ...) pre_macro(X) en_macro((-1) * (8 - count)), EWPM_7(pre_macro, en_macro, count,__VA_ARGS__)
#define EWPM_9(pre_macro, en_macro, count, X, ...) pre_macro(X) en_macro((-1) * (9 - count)), EWPM_8(pre_macro, en_macro, count,__VA_ARGS__)
#define EWPM_10(pre_macro, en_macro, count, X, ...) pre_macro(X) en_macro((-1) * (10 - (count)), EWPM_9(pre_macro, en_macro, __VA_ARGS__)
#define EWPM_11(pre_macro, en_macro, count, X, ...) pre_macro(X) en_macro((-1) * (11 - (count)), EWPM_10(pre_macro, en_macro, __VA_ARGS__)
#define EWPM_12(pre_macro, en_macro, count, X, ...) pre_macro(X) en_macro((-1) * (12 - (count)), EWPM_11(pre_macro, en_macro, __VA_ARGS__)
#define EWPM_13(pre_macro, en_macro, count, X, ...) pre_macro(X) en_macro((-1) * (13 - (count)), EWPM_12(pre_macro, en_macro, __VA_ARGS__)
#define EWPM_14(pre_macro, en_macro, count, X, ...) pre_macro(X) en_macro((-1) * (14 - (count)), EWPM_13(pre_macro, en_macro, __VA_ARGS__)
#define EWPM_15(pre_macro, en_macro, count, X, ...) pre_macro(X) en_macro((-1) * (15 - (count)), EWPM_14(pre_macro, en_macro, __VA_ARGS__)
//... repeat as needed

// Should pass the separator as argument to FE ... (too lazy now)
#define SEPARATOR ,

// Make a FOREACH macro
#define FE_1(WHAT, X) WHAT(X)
#define FE_2(WHAT, X, ...) WHAT(X) , FE_1(WHAT, __VA_ARGS__)
#define FE_3(WHAT, X, ...) WHAT(X) , FE_2(WHAT, __VA_ARGS__)
#define FE_4(WHAT, X, ...) WHAT(X) , FE_3(WHAT, __VA_ARGS__)
#define FE_5(WHAT, X, ...) WHAT(X) , FE_4(WHAT, __VA_ARGS__)
#define FE_6(WHAT, X, ...) WHAT(X) , FE_4(WHAT, __VA_ARGS__)
#define FE_7(WHAT, X, ...) WHAT(X) , FE_4(WHAT, __VA_ARGS__)
#define FE_8(WHAT, X, ...) WHAT(X) , FE_4(WHAT, __VA_ARGS__)
#define FE_9(WHAT, X, ...) WHAT(X) , FE_4(WHAT, __VA_ARGS__)
#define FE_10(WHAT, X, ...) WHAT(X) , FE_4(WHAT, __VA_ARGS__)
#define FE_11(WHAT, X, ...) WHAT(X) , FE_4(WHAT, __VA_ARGS__)
#define FE_12(WHAT, X, ...) WHAT(X) , FE_4(WHAT, __VA_ARGS__)
#define FE_13(WHAT, X, ...) WHAT(X) , FE_4(WHAT, __VA_ARGS__)
#define FE_14(WHAT, X, ...) WHAT(X) , FE_4(WHAT, __VA_ARGS__)
#define FE_15(WHAT, X, ...) WHAT(X) , FE_4(WHAT, __VA_ARGS__)
//... repeat as needed

#define GET_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,NAME,...) NAME
#define EXPAND_GET_MACRO(args) GET_MACRO args
#define FOR_EACH(action,...) \
  EXPAND_GET_MACRO((__VA_ARGS__,FE_15,FE_14,FE_13,FE_12,FE_11,FE_10,FE_9,FE_8,FE_7,FE_6,FE_5,FE_4,FE_3,FE_2,FE_1))(action,__VA_ARGS__)

#define ENUM_MACRO(macro, ...) EXPAND_GET_MACRO((__VA_ARGS__,EN_15,EN_14,EN_13,EN_12,EN_11,EN_10,EN_9,EN_8,EN_7,EN_6,EN_5,EN_4,EN_3,EN_2,EN_1))(macro, SIZE(__VA_ARGS__), __VA_ARGS__)

#define ENUM_WITH_PREFIX_MACRO(pre_macro, en_macro, ...) EXPAND_GET_MACRO((__VA_ARGS__, EWPM_15, EWPM_14, EWPM_13, EWPM_12, EWPM_11, EWPM_10, EWPM_9, EWPM_8, EWPM_7, EWPM_6, EWPM_5, EWPM_4, EWPM_3, EWPM_2, EWPM_1))(pre_macro, en_macro, SIZE(__VA_ARGS__), __VA_ARGS__)

#define GET_COUNT( _1, _2, _3, _4, _5, _6, _8, _9, _10, _11, _12, _13, _14, _15,  COUNT, ...) COUNT
#define EXPAND_GET_COUNT(args) GET_COUNT args
#define SIZE( ... ) EXPAND_GET_COUNT((__VA_ARGS__, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))

#define EAT_VAR_NAME(a) EXPAND(CAT(EAT_VAR_NAME_, a) RPAREN)
#define EAT_VAR_NAME_blaU32 blaU32 EAT LPAREN
#define EAT_VAR_NAME_blaF32 blaF32 EAT LPAREN
#define EAT_VAR_NAME_blaF64 blaF64 EAT LPAREN
#define EAT_VAR_NAME_int int EAT LPAREN
#define EAT_VAR_NAME_float float EAT LPAREN

#define EAT_VAR_TYPE(a) EXPAND(CAT(EAT_VAR_TYPE_, a))
#define EAT_VAR_TYPE_int

#define COLLECT_TYPES(...) FOR_EACH(EAT_VAR_NAME, __VA_ARGS__)
#define COLLECT_NAMES(...) FOR_EACH(EAT_VAR_TYPE, __VA_ARGS__)

#define GET_ARGUMENT_I(i) arguments[i])
#define TO_STRING_MACRO( i ) blaFromString< EAT_VAR_NAME(i)> LPAREN

#define IIF(c) CAT ( IIF_ , c )
#define IIF_0(t, ...) __VA_ARGS__
#define IIF_1(t, ...) t

#define IS_EMPTY(...) GET_MACRO(dummy, ##__VA_ARGS__, 0, 0 , 0 ,0 ,0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 1)

#define IS_EMPTY_TEST(...) GET_MACRO(dummy, ##__VA_ARGS__, 0, 0 , 0 ,0 ,0 , 0, 0, 0, 0, 0, 0, 4, 3, 2, 1)