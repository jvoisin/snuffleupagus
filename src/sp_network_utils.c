#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "php_snuffleupagus.h"

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

static inline bool cidr4_match(const struct in_addr addr,
                               const struct in_addr net, uint8_t bits);
static inline bool cidr6_match(const struct in6_addr address,
                               const struct in6_addr network, uint8_t bits);
static inline int get_ip_version(const char *ip);

void apply_mask_on_ip(char *out, const char *const remote_addr) {
  uint8_t mask4 = SNUFFLEUPAGUS_G(config).config_cookie_encryption->mask_ipv4;
  uint8_t mask6 = SNUFFLEUPAGUS_G(config).config_cookie_encryption->mask_ipv6;
  const int ip_version = get_ip_version(remote_addr);

  memset(out, 0, 128);

  if (ip_version == AF_INET) {
    struct in_addr out4;
    inet_pton(AF_INET, remote_addr, &out4);
    const long n = out4.s_addr & htonl(0xFFFFFFFFu << (32 - mask4));
    out[0] = (n >> 24) & 0xFF;
    out[1] = (n >> 16) & 0xFF;
    out[2] = (n >> 8) & 0xFF;
    out[3] = (n >> 0) & 0xFF;
  } else if (ip_version == AF_INET6) {
    inet_pton(AF_INET6, remote_addr, out);
    uint32_t *p_ip = (uint32_t *)out;
    while (32 < mask6) {
      *p_ip = 0xFFFFFFFFu;
      p_ip++;
      mask6 -= 32;
    }
    if (0 != mask6) {
      *p_ip = htonl(0xFFFFFFFFu << (32 - mask6));
    }
  } else {
    sp_log_err("ip_mask", "It seems that %s isn't a valid ip.", remote_addr);
  }
}

/* http://fxr.watson.org/fxr/source/include/net/xfrm.h?v=linux-2.6#L840 */
static inline bool cidr4_match(const struct in_addr addr,
                               const struct in_addr net, uint8_t bits) {
  if (bits == 0) {  // C99 6.5.7 (3): u32 << 32 is undefined behaviour
    return true;
  }
  return !((addr.s_addr ^ net.s_addr) & htonl(0xFFFFFFFFu << (32 - bits)));
}

static inline bool cidr6_match(const struct in6_addr address,
                               const struct in6_addr network, uint8_t bits) {
  //#ifdef LINUX
  const uint32_t *a = address.s6_addr32;
  const uint32_t *n = network.s6_addr32;
  /*
#else
  const uint32_t *a = address.__u6_addr.__u6_addr32;
  const uint32_t *n = network.__u6_addr.__u6_addr32;
#endif
*/
  int bits_whole, bits_incomplete;
  bits_whole = bits >> 5;         // number of whole u32
  bits_incomplete = bits & 0x1F;  // number of bits in incomplete u32
  if (bits_whole) {
    if (memcmp(a, n, bits_whole << 2)) {
      return false;
    }
  }
  if (bits_incomplete) {
    uint32_t mask = htonl((0xFFFFFFFFu) << (32 - bits_incomplete));
    if ((a[bits_whole] ^ n[bits_whole]) & mask) {
      return false;
    }
  }
  return true;
}

static inline int get_ip_version(const char *ip) {
  struct in_addr out4;
  struct in6_addr out6;
  int res = inet_pton(AF_INET, ip, &out4);
  if (0 == res) {
    if (1 == inet_pton(AF_INET6, ip, &out6)) {
      return AF_INET6;
    } else {
      return -1;
    }
  } else if (1 == res) {
    return AF_INET;
  } else {
    return -1;
  }
}

// TODO factorise a bit this function
bool cidr_match(const char *ip, const sp_cidr *cidr) {
  struct in_addr out4;
  struct in6_addr out6;

  switch (get_ip_version(ip)) {
    case AF_INET:
      if (AF_INET != cidr->ip_version) {
        return false;
      }
      inet_pton(AF_INET, ip, &out4);
      return cidr4_match(out4, cidr->ip.ipv4, cidr->mask);
    case AF_INET6:
      if (AF_INET6 != cidr->ip_version) {
        return false;
      }
      inet_pton(AF_INET6, ip, &out6);
      return cidr6_match(out6, cidr->ip.ipv6, cidr->mask);
    default:
      sp_log_err("cidr_match", "Weird ip (%s) family", ip);
      break;
  }
  return false;
}

int get_ip_and_cidr(char *ip, sp_cidr *cidr) {
  errno = 0;
  char *mask = strchr(ip, '/');

  if (NULL == mask) {
    sp_log_err("config",
			"'%s' isn't a valid network mask, it seems that you forgot a '/'.",
            ip);
    return -1;
  }

  if (sscanf(mask + 1, "%hhu", &(cidr->mask)) != 1) {
    sp_log_err("config", "'%s' isn't a valid network mask.", mask + 1);
    return -1;
  }

  ip[mask - ip] = '\0';  // NULL the '/' char

  cidr->ip_version = get_ip_version(ip);

  if (AF_INET == cidr->ip_version) {
    if (cidr->mask > 32) {
      sp_log_err("config", "'%d' isn't a valid ipv4 mask.", cidr->mask);
      return -1;
    }
    inet_pton(AF_INET, ip, &(cidr->ip.ipv4));
  } else if (AF_INET6 == cidr->ip_version) {
    inet_pton(AF_INET6, ip, &(cidr->ip.ipv6));
  } else {
    return -1;
  }

  ip[mask - ip] = '/';
  return 0;
}
