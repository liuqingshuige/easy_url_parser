#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "easy_uri_parser.h"


int main(int argc, char **argv)
{
	const char *inbuf = "http://www.baidu.com/?que='中'\"__\"# 国&nihao=(-@$*也生均~.mp2[123])?";
	char ttemp[512] = {0};
	char obuf[256] = {0};

	int ret = HttpUrlEncode(inbuf, strlen(inbuf), ttemp, sizeof(ttemp));
	printf("encode ret: %d\n%s\n\n", ret, ttemp);

	ret = HttpUrlDecode(ttemp, 0, obuf, sizeof(obuf));
	printf("decode ret: %d\n%s\n\n", ret, obuf);

	printf("=====url parse=====\n");
	const char *url1 = "http://username:password@www.baidu.com:8569/path?param1=value1&param2=value2#fragment";
	const char *url2 = "http://admin:1234@43.139.119.208:1612/#/home";
	EasyUrlResult result;

#if 1
	ret = EasyUrlParse(url1, &result);
	printf("parse url: %s\nret: %d\n", url1, ret);
	printf("scheme: %s\nuser: %s\npwd: %s\nhost: %s\nport: %d\npath: %s\nquery: %s\nfragment: %s\n\n", 
		result.scheme, result.username, result.password, result.host, result.port,
		result.path, result.query, result.fragment);
		
	ret = EasyUrlParse(url2, &result);
	printf("parse url: %s\nret: %d\n", url2, ret);
	printf("scheme: %s\nuser: %s\npwd: %s\nhost: %s\nport: %d\npath: %s\nquery: %s\nfragment: %s\n\n", 
		result.scheme, result.username, result.password, result.host, result.port,
		result.path, result.query, result.fragment);
#endif
	if (argc == 2)
	{
		ret = EasyUrlParse(argv[1], &result);
		printf("parse url: %s\nret: %d\n", argv[1], ret);
		printf("scheme: %s\nuser: %s\npwd: %s\nhost: %s\nport: %d\npath: %s\nquery: %s\nfragment: %s\n\n", 
			result.scheme, result.username, result.password, result.host, result.port,
			result.path, result.query, result.fragment);
			
		if (result.query_size)
		{
			int i = 0;
			for (; i<result.query_size; i++)
			{
				printf("key: %.*s, value: %.*s\n",
					result.query_map[i].klen, result.query_map[i].key,
					result.query_map[i].vlen, result.query_map[i].value);
			}
		}
	}

	return 0;
}




