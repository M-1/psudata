// Build with: gcc psudata.cc -o psudata -framework IOKit

// original code from https://apple.stackexchange.com/a/283403 by Luyer, modified by M-1

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

float get_value(io_connect_t * ioc, uint32_t key) {
  smc_t req, resp;
  size_t sz = sizeof(resp);

  memset(&req, 0, sizeof(smc_t));
  memset(&resp, 0, sizeof(smc_t));
  req.cmd = 5;
  req.size = 4;
  req.key = key;
  IOConnectCallStructMethod(*ioc, 2, &req, sizeof(req), &resp, &sz);
  return resp.val;
}

int main(void) {
  mach_port_t mp;
  CFMutableDictionaryRef d;
  io_iterator_t it;
  io_object_t svc;
  io_connect_t ioc;
  float v, i, system_total;

  IOMasterPort(MACH_PORT_NULL, &mp);
  d = IOServiceMatching("AppleSMC");
  IOServiceGetMatchingServices(mp, d, &it);
  svc = IOIteratorNext(it);
  IOObjectRelease(it);
  IOServiceOpen(svc, mach_task_self(), 0, &ioc);
  IOObjectRelease(svc);

  // SMC keys/"codes" can be found at https://logi.wiki/index.php/SMC_Sensor_Codes
  v = get_value(&ioc, ('V' << 24) + ('D' << 16) + ('0' << 8) + 'R');
  i = get_value(&ioc, ('I' << 24) + ('D' << 16) + ('0' << 8) + 'R');
  system_total = get_value(&ioc, ('P' << 24) + ('S' << 16) + ('T' << 8) + 'R');

  IOServiceClose(ioc);

  printf("DC In: %fV %fA %fW\n", v, i, v*i);
  printf("System Total: %fW\n", system_total);
  return 0;
}
