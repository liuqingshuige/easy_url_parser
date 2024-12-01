/*
 * URL解析
 * Copyright FreeCode. All Rights Reserved.
 * MIT License (https://opensource.org/licenses/MIT)
 * 2024 by liuqingshuige
 */
#ifndef __FREE_EASY_URL_PARSER_H__
#define __FREE_EASY_URL_PARSER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define IS_ALPHA(c) ( ((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z') )
#define IS_DIGIT(c) ((c) >= '0' && (c) <= '9')
#define IS_ALPHANUM(c) (IS_ALPHA(c) || IS_DIGIT(c))
#define IS_HEX(c) ( ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F') )
#define IS_HEX_LOWER(c) ((c) >= 'a' && (c) <= 'f')
#define IS_HEX_UPPER(c) ((c) >= 'A' && (c) <= 'F')

/*
 * query参数结果
 */
typedef struct QueryResult
{
	const char *key;
	int klen;
	const char *value;
	int vlen;
}QueryResult;

/*
 * URL解析结果
 */
typedef struct EasyUrlResult
{
	char scheme[32];
	char username[64];
	char password[64];
	char host[256];
	int port;
	char path[256];
	char query[512];
	int query_size; // query_map[]有效元素个数
	QueryResult query_map[32];
	char fragment[512];
}EasyUrlResult;


/*
 * 解析URL
 * url：待解析的URL
 * result：解析结果
 * return：0 on success，others on fail
 */
int EasyUrlParse(const char *url, EasyUrlResult *result);

/*
 *根据RFC 3986标准，URL由以下几部分组成：

 *保留字符（Reserved Characters）：
 *这些字符在URI中有特殊含义，用于分隔不同的组件。
 *保留字符分为两类：通用分隔符（gen-delims）和子分隔符（sub-delims）。

 *通用分隔符（gen-delims）：: / ? # [ ] @
 *子分隔符（sub-delims）：! $ & ' ( ) * + , ; =
 *这些保留字符的总和为：! * ' ( ) ; : @ & = + $ , / ? # [ ]

 *未保留字符（Unreserved Characters）：
 *这些字符可以在URI中直接使用，不需要进行编码。
 *它们包括英文字母（a-zA-Z）、数字（0-9）以及四个特殊字符-_.~。

 *组成：根据RFC 3986，URL的语法可以表示为：
 *URI = scheme ":" hier-part [ "?" query ] [ "#" fragment ]。
 *其中，scheme（方案）和hier-part（层次部分）是必须有的，但hier-part可以是空的。
 *如果authority（授权部分）存在，则path（路径）要么为空要么以/字符为起始。
 *如果authority不存在，则path不能以//为开头。
*/

/*
 * HTTP-URL编码
 * inUrl：待编码的URL
 * inSize：inUrl长度，单位字节
 * outUrl：保存编码后的URL
 * outSize：outUrl大小，单位字节
 * return：编码后的URL长度
 */
int HttpUrlEncode(const char *inUrl, size_t inSize, char *outUrl, size_t outSize);

/*
 * HTTP-URL解码
 * inUrl：待解码的URL
 * inSize：inUrl长度，单位字节
 * outUrl：保存解码后的URL
 * outSize：outUrl大小，单位字节
 * return：解码后的URL长度
 */
int HttpUrlDecode(const char *inUrl, size_t inSize, char *outUrl, size_t outSize);

#ifdef __cplusplus
}
#endif

#endif
