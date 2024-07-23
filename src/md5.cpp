
#include <iostream>
#include <openssl/md5.h>
#include <fstream>
#include <string>
#include <iomanip>
#include <string.h>


std::string get_str_md5(std::string assign_str)
{

	// const char data[] = assign_str.c_str();
    unsigned char md5[MD5_DIGEST_LENGTH];
 
    // 第一种方式
    MD5(reinterpret_cast<const unsigned char*>(assign_str.c_str()), assign_str.size(), md5); //直接产生字符串的MD5
 
    // 第二种方式
    // MD5_CTX ctx;
    // MD5_Init(&ctx); //初始化MD5上下文结构
    // MD5_Update(&ctx, data, strlen(data)); //刷新MD5，将文件连续数据分片放入进行MD5刷新
    // MD5_Final(md5, &ctx); //产生最终的MD5数据
 
    std::string md5_hex;
    const char map[] = "0123456789abcdef";
    for (size_t i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        // std::cout << int(md5[i]) << " ";
        md5_hex += map[md5[i] / 16];
        md5_hex += map[md5[i] % 16];
    }
    // std::cout << "\n" << md5_hex << std::endl;
    return md5_hex;
}


std::string get_file_md5(std::string lpcstrFilePath)
{
    char *pbymd5Buff [100]= {0};
	unsigned char achbyBuff[17] = {0};
	unsigned char *pbyBuff = achbyBuff;
	
    FILE *inFile = fopen (lpcstrFilePath.c_str(), "rb");
	MD5_CTX mdContext;
	int bytes;
	unsigned char data[1024];
	
	if (inFile == NULL) {
		printf( "%s can't be opened.\n", lpcstrFilePath.c_str());
        throw "file can't be opened : " ;
	}
	
	MD5_Init (&mdContext);
	while ((bytes = fread (data, 1, 1024, inFile)) != 0)
		MD5_Update (&mdContext, data, bytes);
	MD5_Final (pbyBuff,&mdContext);
	
    char szBuf[1024] = {0};
	for(int i = 0; i < MD5_DIGEST_LENGTH; i++) 
	{
		sprintf(&szBuf[i*2], "%02x", pbyBuff[i]);
	}
	
    strcpy((char*)pbymd5Buff, (char*)szBuf);

    std::string md5_str = szBuf;
	fclose (inFile);
    return szBuf;
}
