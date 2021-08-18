#ifndef BASE_MACRO_DEFINES
#define BASE_MACRO_DEFINES

//连接两个符号
#define CONCAT_EXPANSION_IMPLE(A,B) A ## B
#define CONCAT_EXPANSION(A,B) CONCAT_EXPANSION_IMPLE(A,B)

//将符号变为字符串
#define STRING_EXPANSION_IMPLE(X) # X
#define STRING_EXPANSION(X) STRING_EXPANSION_IMPLE(X)

//不做任何事，只是指引宏进行二段展开
#define NONE_EXPANSION_IMPLE(...)	__VA_ARGS__
#define NONE_EXPANSION(...)	NONE_EXPANSION_IMPLE(__VA_ARGS__)

//相当于invoke，将进行调用形式的二段展开
#define TWO_STAGE_EXPANSION(MACRO,...) MACRO(__VA_ARGS__)

#endif