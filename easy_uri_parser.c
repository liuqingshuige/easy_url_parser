/*
 * URL解析
 * Copyright FreeCode. All Rights Reserved.
 * MIT License (https://opensource.org/licenses/MIT)
 * 2024 by liuqingshuige
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "easy_uri_parser.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/*
 * 在指定长度的字符串中查找指定字符
 * return：成功返回指向指定字符的指针，失败返回NULL
 */
static const char *strnchr(const char *str, char c, size_t n)
{
	size_t i = 0;
	const char *ptr = str;
	while (ptr && (*ptr) && (i < n))
	{
		if (*ptr == c)
			return ptr;
		ptr++;
		i++;
	}
	return NULL;
}

/*
 * 查找指定字符
 */
static const unsigned char *struchr(const unsigned char *s, unsigned char c)
{
	while (*s)
	{
		if (*s == c)
			return s;
		s++;
	}
	return NULL;
}

/*
 * 把str中的所有strsub替换为strreplace, 不要出现回文串
 * 确保str的空间足够，因为strreplace可能会比strsub长
 */
static int strreplace(char *str, const char *strsub, const char *strreplace)
{
	if (!str || !strsub || !strreplace)
		return -1;

	if (0==strcmp(strsub, strreplace)) // 一样的，不用替换了
		return 0;

	int len = strlen(str); // Show you the power of strcpy and strcat
	int slen = strlen(strsub);

	if (slen > len) // 子串比主串还长，不用替换了
		return -1;

	int rlen = strlen(strreplace);
	char *p = strstr(str, strsub); // str = "123abcjjf567"  strsub="abc"  strreplace = "xyzwq"
	char *temp = 0; // str = "123jjf567abc"  strsub="abc"  strreplace = "xyzwq"
	char *tbuf = (char *)malloc(len);
	if (!tbuf)
		return -1;
	while (p)
	{
		if (*(p+slen+1) == 0) // strsub在末尾，后面没有了，退出
		{
			strcpy(p, strreplace);
			break;
		}
		temp = p + slen; // str = "123abcjjf567", now temp points to "jjf567"
		strcpy(tbuf, temp); // 避免被破坏
		strcpy(p, strreplace); // 替换
		strcat(str, tbuf); // 将后面的字符串拼接回来
		temp = p + rlen; // 跳过strreplace，继续往下找
		p = strstr(temp, strsub);
	}
	free(tbuf);
	return 0;
}

/*
 * 十六进制转十进制
 */
static unsigned char Hex2Dec(char hex)
{
	if (IS_DIGIT(hex))
		return hex - '0';
	else if (IS_HEX_LOWER(hex))
		return hex - 'a' + 10;
	else if (IS_HEX_UPPER(hex))
		return hex - 'A' + 10;
	return (unsigned char)hex;
}

/*
 * 将URL中的十六进制表示的字符转换回来
 * url：待转换的URL
 * inSize：url长度，单位字节
 * buf：保存转换结果
 * bufSize：buf大小，单位字节
 * return：成功返回转换后的URL长度，失败返回-1
 * eg：http://1.2.3.4:5800/2024-05-10_17-55-18_01ZHUJI_Call_%E4%B8%80%E9%94%AE0241.mp4
 */
static int DecodeEscape(const char *url, size_t inSize, char *buf, size_t bufSize)
{
	if (!url || !buf)
		return -1;

	const char *in_start = url;
	const char *in_end = url + ((inSize > 0) ? inSize : strlen(url));
	char *out_start = buf;
	char *out_end = buf + bufSize;

	while (in_start < in_end && out_start < out_end)
	{
		if ((*in_start == '%') // %xx?
			&& ((in_start+2) < in_end)
			&& (IS_DIGIT(in_start[1]) || IS_HEX(in_start[1]))
			&& (IS_DIGIT(in_start[2]) || IS_HEX(in_start[2]))
			)
		{
			*out_start = (Hex2Dec(in_start[1]) << 4) | Hex2Dec(in_start[2]);
			in_start += 2;
		}
		else
		{
			if (*in_start == '+')
				*out_start = ' ';
			else
				*out_start = *in_start;
		}

		in_start++;
		out_start++;
	}
	*out_start = '\0';

	return out_start - buf;
}

/*
 * 解析URL
 * url：待解析的URL
 * result：解析结果
 * return：0 on success，others on fail
 * eg：http://admin:admin@1.2.3.4:5800/21ZHUJI_Call_%E4%B8%80%E9%94%AE0241.mp4
 */
int EasyUrlParse(const char *url, EasyUrlResult *result)
{
	if (!url || !result)
		return -1;
#if 1
	char decurl[1024];

	DecodeEscape(url, strlen(url), decurl, sizeof(decurl));
	strreplace(decurl, "\\", "/");

	const char *pos = decurl, *end = decurl + strlen(decurl);
#else
	const char *pos = url, *end = url + strlen(url);
#endif
	size_t len = 0;
	const char *ptr, *tmp;

	memset(result, 0, sizeof(EasyUrlResult));
	// scheme://
	ptr = strstr(pos, "://");
	if (!ptr)
		return -1;

	len = ptr - pos;
	if (len > sizeof(result->scheme))
		return -1;
	strncpy(result->scheme, pos, len);

	// user:pwd@host:port/path
	pos = ptr + 3;
	ptr = strchr(pos, '/'); // 查找path分割符/
	if (!ptr)
		ptr = end;

	tmp = ptr;
	ptr = strnchr(pos, '@', ptr - pos); // 查找分割符@
	if (ptr) // 说明存在user
	{
		// pos指向user，ptr指向@
		const char *pwd = strnchr(pos, ':', ptr - pos);
		if (pwd) // 存在pwd，pwd指向:
		{
			len = pwd - pos; // user长度
			if (len > sizeof(result->username))
				return -1;
			strncpy(result->username, pos, len);
			
			pwd = pwd + 1; // pwd指向pwd
			len = ptr - pwd;
			if (len > sizeof(result->password))
				return -1;
			strncpy(result->password, pwd, len);
			
			pos = ptr + 1; // 指向@的下一位
		}
		else // 不存在pwd
		{
			len = ptr - pos; // user长度
			if (len > sizeof(result->username))
				return -1;
			strncpy(result->username, pos, len);
			
			pos = ptr + 1; // 指向@的下一位
		}
	}

	// host:port/path
	// pos指向host
	ptr = tmp; // 还原ptr指向path分割符/或者结束符
	ptr = strnchr(pos, ':', ptr - pos); // 查找分割符:
	if (ptr) // 存在port
	{
		// pos指向host，ptr指向:
		len = ptr - pos; // user长度
		if (len > sizeof(result->host))
			return -1;
		strncpy(result->host, pos, len);
		
		int port = 0;
		ptr = ptr + 1; // ptr指向port
		while (ptr < tmp) // tmp指向path分割符/或者结束符
		{
			if (!IS_DIGIT(*ptr))
				break;

			port = port * 10 + Hex2Dec(*ptr);
			ptr++;
		}
		result->port = port;
		pos = ptr; // pos指向path分割符/或者结束符，即pos == tmp，否则端口号有错误
		if (pos != tmp) // 比如：host:80err80/path
			return -1;
	}
	else // 不存在port
	{
		// pos指向host
		len = tmp - pos; // host长度
		if (len > sizeof(result->host))
			return -1;
		strncpy(result->host, pos, len);

		// 设置默认的port
		result->port = 80; // default port
		if (!strncasecmp(url, "https://", 8)
			|| !strncasecmp(url, "wss://", 6))
			result->port = 443;
		else if (!strncasecmp(url, "rtsp://", 7)
			|| !strncasecmp(url, "rtsps://", 8))
			result->port = 554;
		else if (!strncasecmp(url, "rtmp://", 7)
			|| !strncasecmp(url, "rtmps://", 8))
			result->port = 1935;
		else if (!strncasecmp(url, "ssh://", 6))
			result->port = 22;
		else if (!strncasecmp(url, "ftp://", 6))
			result->port = 21;
		pos = tmp; // pos指向path分割符/或者结束符
	}

	// /path?s=abc#fragment
	// pos指向path分割符/或者结束符
	if (pos == end)
	{
		result->path[0] = '/';
		return 0;
	}

	// http://username:password@host:1258/path?param1=value1&param2=value2#fragment
	// http://43.139.119.208:1612/#/home
	// pos指向path分割符/
	int type = 0; // 0：path，1：query，2：fragment
	int np = 0, nq = 0, nf = 0;
	ptr = pos;
	while (ptr < end)
	{
		if (*ptr == '?' && type != 1)
		{
			type = 1;
			ptr++;
			continue;
		}

		if (*ptr == '#' && type != 2)
		{
			type = 2;
			ptr++;
			continue;
		}

		if (type == 0)
		{
			result->path[np++] = *ptr;
		}
		else if (type == 1)
		{
			result->query[nq++] = *ptr;
		}
		else if (type == 2)
		{
			result->fragment[nf++] = *ptr;
		}
		ptr++;
	}

	// query param
	// param1=value1&param2=value2
	if (nq >= 3) // the shortest：s=b
	{
		int n = 0;

		type = 0; // 0：key，1：value
		ptr = result->query;
		end = ptr + nq;

		result->query_map[n].key = ptr;
		while (ptr < end)
		{
			if (*ptr == '=' && type == 0)
			{
				type = 1;
				ptr++;
				result->query_map[n].value = ptr;
			}

			if (*ptr == '&' && type == 1)
			{
				type = 0;
				n++;
				if (n >= (ARRAY_SIZE(result->query_map) - 1))
					break;
				ptr++;
				result->query_map[n].key = ptr;
			}

			if (type == 0)
			{
				result->query_map[n].klen++;
			}
			else if (type == 1)
			{
				result->query_map[n].vlen++;
			}

			ptr++;
		}
		result->query_size = n + 1;
	}
	return 0;
}

/*
 * 是否是保留和未保留字符
 */
static int IsHttpReserved(unsigned char ch)
{
	const unsigned char *exp = "!*'();:@&=+$,/?#[]-_.~";
	return (IS_ALPHANUM(ch) || struchr(exp, ch));
}

/*
 * HTTP-URL编码
 * inUrl：待编码的URL
 * inSize：inUrl长度，单位字节
 * outUrl：保存编码后的URL
 * outSize：outUrl大小，单位字节
 * return：编码后的URL长度
 */
int HttpUrlEncode(const char *inUrl, size_t inSize, char *outUrl, size_t outSize)
{
	if (!inUrl || !outUrl)
		return 0;

	char hex[] = "0123456789abcdef";
	const char *in_begin = inUrl;
	const char *in_end = inUrl + ((inSize > 0) ? inSize : strlen(inUrl));
	char *out_begin = outUrl;
	char *out_end = outUrl + outSize;

	while (in_begin < in_end && out_begin < out_end)
	{
		unsigned char ch = (unsigned char)*in_begin;
		if (IsHttpReserved(ch))
		{
			*out_begin = (char)ch;
			out_begin++;
		}
		else
		{
			out_begin[0] = '%';
			out_begin[1] = hex[(ch >> 4) & 0xf];
			out_begin[2] = hex[ch & 0xf];
			out_begin += 3;
		}
		in_begin++; // next
	}
	return out_begin - outUrl;
}

/*
 * HTTP-URL解码
 * inUrl：待解码的URL
 * inSize：inUrl长度，单位字节
 * outUrl：保存解码后的URL
 * outSize：outUrl大小，单位字节
 * return：解码后的URL长度
 */
int HttpUrlDecode(const char *inUrl, size_t inSize, char *outUrl, size_t outSize)
{
	return DecodeEscape(inUrl, inSize, outUrl, outSize);
}



