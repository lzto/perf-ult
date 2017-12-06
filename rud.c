/*
 * standalone program to setup perf subsystem and receive perf data
 * 2017 Tong Zhang<ztong@vt.edu>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <linux/perf_event.h>

#define PERF_TYPE_RAW_PEBS 8

#include "hperf.h"
#include "pebs.h"

static long
perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
			   int cpu, int group_fd, unsigned long flags)
{
   int ret;

   ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
				  group_fd, flags);
   return ret;
}

void* m;
void* aux_pebs;
void* aux_pt;

/*
 * ring buffer stuff to receive data
 */
void setup_mmap(struct perf_event_attr * attr, int fd)
{
    fprintf(stderr, "setup_mmap: fd %d\n", fd);
    int ret;
    int output = fd;
    int regular_ring_size = 129 * 4096;
    //create mmap
    m = mmap(NULL, regular_ring_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (m == MAP_FAILED)
    {
        fprintf(stderr,"mmap failed : %d\n", errno);
        return;
    }
    fprintf(stderr,"mmap m=%p + %d\n", m, regular_ring_size);

    //must set this before do aux mapping
    struct hacked_perf_event_mmap_page *pc = m;
    pc->aux_offset_pt = regular_ring_size;
    pc->aux_size_pt = 64*4096;
    pc->aux_offset_pebs = regular_ring_size + 64 * 4096;
    pc->aux_size_pebs = 64* 4096;

    //create aux mmap
    aux_pt = mmap(NULL, 128*4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 129*4096);
    if (aux_pt == MAP_FAILED)
    {
        fprintf(stderr, "mmap aux failed : %d\n", errno);
        return;
    }
    aux_pebs = ((unsigned char*)aux_pt) + 64*4096;
    fprintf(stderr,"mmap aux_pt=%p, aux_pebs=%p\n",
            aux_pt, aux_pebs);
    //notify driver the memory region to use
    //ret = ioctl(fd, PERF_EVENT_IOC_SET_OUTPUT, &output);
    //if (ret!=0)
    //{
    //    fprintf(stderr,"ret=%d\n",ret);
    //}
}

void dump(int fd)
{
	//read(fd, &count, sizeof(long long));
	//printf("Used %lld instructions\n", count);
    fprintf(stderr,"-------------------\n");
    for(int i=0;i<64;i++)
    {
        unsigned char *p = &(((unsigned char*)(aux_pebs))[4096*i]);
        for(int j=0;j<4096;j++)
        {
            fprintf(stderr,"0x%02x ", p[j]);
            if ((j+1)%32==0)
                fprintf(stderr,"\n");
        }
        fprintf(stderr,"\n");
    }
}

void dump2(int fd)
{
    for (int i=0;i<(4096*64/(sizeof(struct pebs_v3)));i++)
    {
        struct pebs_v3* p =
            (struct pebs_v3*)
            &(((unsigned char*)(aux_pebs))[(sizeof(struct pebs_v3))*i]);
        fprintf(stderr,"TSC:0x%llx, IP:0x%llx\n", p->tsc, p->v2.v1.ip);
    }
}

int main(int argc, char** argv)
{
    long long count;
    struct perf_event_attr attr;
    memset(&attr, 0, sizeof(struct perf_event_attr));
    //the index(type) for /sys/devices/intel_pebs
    attr.type = PERF_TYPE_RAW_PEBS;
    attr.size = sizeof(struct perf_event_attr);
    attr.sample_type = 
            PERF_SAMPLE_IP | PERF_SAMPLE_TID | PERF_SAMPLE_TIME |
            PERF_SAMPLE_CPU | PERF_SAMPLE_IDENTIFIER;
    /*
     * put PEBS event config here
     * see /sys/devices/intel_pebs/format/* 
     * and linux/tools/perf/design.txt for more details
     */
    attr.config = 0xC4 + 0x2A00;

    attr.sample_period = 103;
    attr.read_format = PERF_FORMAT_ID;

    attr.exclude_user = 0;
    attr.exclude_kernel = 1;
    attr.exclude_hv = 1;
    attr.exclude_host = 0;
    attr.exclude_guest = 0;
    attr.exclude_idle = 1;
    attr.disabled = 1;
    attr.precise_ip = 3;//no skid at all
    attr.mmap = 0;
    attr.comm = 0;
    attr.mmap_data = 0;


    int fd = perf_event_open(&attr,0,-1,-1,0);
    if (fd==-1)
    {
        fprintf(stderr, "Error opening leader %llx\n", attr.config);
        exit(EXIT_FAILURE);
    }

    setup_mmap(&attr, fd);

    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
	printf("Measuring instruction count for this printf\n");
	/* Place target code here instead of printf */
    for(int i=0;i<10000;i++)
    {
    }

	ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    
    dump2(fd);

	close(fd);
    return 0;
}


