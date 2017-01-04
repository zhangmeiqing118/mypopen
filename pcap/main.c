/**
 * @Filename: psend.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 06/16/2016 01:17:31 PM
 */

#include "aclk_sdk.h"
#include "aclk.h"

#include "aclk_pcap.h"

#define ACLK_DPI_MAX_PACKET_LEN     2000
#define ACLK_DPI_DATA_OFFSET        256
#define ACLK_DPI_MAX_BUF_LEN        (ACLK_DPI_MAX_PACKET_LEN + ACLK_DPI_DATA_OFFSET)

CVMX_SHARED int g_log_level;

struct option g_long_opts[] = { 
    { "pcap_file", 1, NULL, 'c'},
    { "help",      0, NULL, 'h'},
    { NULL, 0, NULL, 0 } 
};

void usage(char *progname)
{
    printf("Usage : %s [OPTION...]\n\n\
            -f, --pcap_file    pcap file name\n\
            -h, --help         show this help and exit\n\
            \n\
            Report bugs to bugs@accelink.com\n", progname);

    return;
}

int main(int argc, char *argv[])
{
    FILE *fp;
    int ssize, opt;
    char sdata[ACLK_DPI_MAX_BUF_LEN];
    char *pcap_file, *progname;
    cvmx_wqe_t *wqe;


    progname = strrchr(argv[0], '/');
    if (progname) {
        ++progname;
    } else {
        progname = argv[0];
    }
    pcap_file = NULL;
    while (1) {
        opt = getopt_long(argc, argv, "f:h", g_long_opts, 0);
        if (EOF == opt) {
            break;
        }
        switch (opt) {
            case 'f':
                pcap_file = optarg;
                break;
            case 'h':
            default:
                usage(progname);
                return 0;
        }
    }
    if (NULL == pcap_file) {
        usage(progname);
        return -1;
    }

    fp = aclk_pcap_open(pcap_file);
    if (NULL == fp) {
        return -1;
    }

    wqe = (cvmx_wqe_t *)sdata;
    wqe->packet_ptr = sdata + ACLK_DPI_DATA_OFFSET;
    do {
        ssize = aclk_pcap_read(fp, wqe->packet_ptr, ACLK_DPI_MAX_PACKET_LEN);
        if (0 == ssize) {
            break;
        } else if (-1 == ssize) {
            printf("aclk pacp read error\n");
            break;
        }
        wqe->word1.len = ssize;
       
        ///process packet
        aclk_dpi_process_packet(wqe);

    } while(1);

    fclose(fp);

    ///
    aclk_dpi_result_print();

    return 0;
}
