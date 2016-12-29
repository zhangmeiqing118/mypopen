/**
 * @Filename: aclk_pcap.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 12/29/2016 02:37:41 PM
 */
#include <stdio.h>

FILE *aclk_pcap_open(const char *filename)
{
    FILE *fp;
    pcap_file_header_t file_head;

    fp = fopen(filename, "r");
    if (NULL == fp) {
        printf("pcap file %s open error\n", filename);
        return NULL;
    }
    
    if (1 != fread(&file_head, sizeof(pcap_file_header_t), 1, fp)) {
        printf("%s[%d]:Invalid pcap file(%s)\n", __func__, __LINE__, filename);
        fclose(fp);
        return NULL;
    }
    
    file_head.magic = le32toh(file_head.magic);
#ifdef __DEBUG__
    file_head.version_major = le16toh(file_head.version_major);
    file_head.version_minor = le16toh(file_head.version_minor);
    file_head.thiszone = le32toh(file_head.thiszone);
    file_head.sigfigs = le32toh(file_head.sigfigs);
    file_head.snaplen = le32toh(file_head.snaplen);
    file_head.linktype = le32toh(file_head.linktype);
    
    printf("magic:0x%x\n", file_head.magic);
    printf("major:0x%x\n", file_head.version_major);
    printf("minor:0x%x\n", file_head.version_minor);
    printf("thiszone:0x%x\n", file_head.thiszone);
    printf("sigfigs:%u\n", file_head.sigfigs);
    printf("snaplen:%u\n", file_head.snaplen);
    printf("linktype:%u\n", file_head.linktype);
#endif   

    if (ACLK_PCAP_MAGIC_NUM != file_head.magic) {
        printf("%s[%d]:Invalid pcap file(%s)\n", __func__, __LINE__, filename);
        fclose(fp);
        return NULL;
    }

    return fp;
}

int aclk_pcap_read(FILE *fp, char *data, int len)
{
    int rlen;
    pcap_packet_header_t packet_head;

    if (1 != fread(&pcap_head, sizeof(pcap_header_t), 1, fp)) {
        printf("%s[%d]:read pcap over\n", __func__, __LINE__);
        return 0;
    }
    rlen = le32toh(pcap_head.len);

#ifdef __DEBUG__
    printf("time:%d:%d\n", pcap_head.ts.tv_sec, pcap_head.ts.tv_usec);
    tv_sec = le32toh(pcap_head.ts.tv_sec);
    tmp = localtime(&tv_sec);
    strftime(outstr, 128, "%B %d %Y %r", tmp);
    printf("time:%s\n", outstr);
    printf("snaplen:%u, cap len:%u\n", le32toh(pcap_head.capture_len), le32toh(pcap_head.len));
#endif

    if (rlen > len) {
        printf("%s[%d]: read buf is smaller than packet size\n", __func__, __LINE__);
        return -1;
    }
    if (1 != fread(data, rlen, 1, fp)) {
        printf("%s[%d]:read pcap header error\n", __func__, __LINE__);
        return -1;
    }

    return rlen;
}
