// Build with: gcc psudata.cc -o psudata -framework IOKit
#include <stdio.h>
#include <string.h>
#include <IOKit/IOKitLib.h>

typedef struct {
  uint32_t key;
  char ignored[24];
  uint32_t size;
  char ignored2[10];
  char cmd;
  char ignored3[5];
  float val;
  char ignored4[28];
} smc_t;

int main(void) {
  mach_port_t mp;
  CFMutableDictionaryRef d;
  io_iterator_t it;
  io_object_t svc;
  io_connect_t ioc;
  smc_t req, resp;
  size_t sz = sizeof(resp);
  uint32_t vk  = ('V' << 24) + ('D' << 16) + ('0' << 8) + 'R';
  uint32_t ik  = ('I' << 24) + ('D' << 16) + ('0' << 8) + 'R';
  float v, i;

  IOMasterPort(MACH_PORT_NULL, &mp);
  d = IOServiceMatching("AppleSMC");
  IOServiceGetMatchingServices(mp, d, &it);
  svc = IOIteratorNext(it);
  IOObjectRelease(it);
  IOServiceOpen(svc, mach_task_self(), 0, &ioc);
  IOObjectRelease(svc);
  memset(&req, 0, sizeof(smc_t));
  memset(&resp, 0, sizeof(smc_t));
  req.cmd = 5;
  req.size = 4;
  req.key = vk;
  IOConnectCallStructMethod(ioc, 2, &req, sizeof(req), &resp, &sz);
  v = resp.val;
  req.key = ik;
  IOConnectCallStructMethod(ioc, 2, &req, sizeof(req), &resp, &sz);
  i = resp.val;
  IOServiceClose(ioc);

  printf("%fV %fA %fW\n", v, i, v*i);
  return 0;
}
