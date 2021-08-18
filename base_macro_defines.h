#ifndef BASE_MACRO_DEFINES
#define BASE_MACRO_DEFINES

//������������
#define CONCAT_EXPANSION_IMPLE(A,B) A ## B
#define CONCAT_EXPANSION(A,B) CONCAT_EXPANSION_IMPLE(A,B)

//�����ű�Ϊ�ַ���
#define STRING_EXPANSION_IMPLE(X) # X
#define STRING_EXPANSION(X) STRING_EXPANSION_IMPLE(X)

//�����κ��£�ֻ��ָ������ж���չ��
#define NONE_EXPANSION_IMPLE(...)	__VA_ARGS__
#define NONE_EXPANSION(...)	NONE_EXPANSION_IMPLE(__VA_ARGS__)

//�൱��invoke�������е�����ʽ�Ķ���չ��
#define TWO_STAGE_EXPANSION(MACRO,...) MACRO(__VA_ARGS__)

#endif