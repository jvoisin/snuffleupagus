#include "php_snuffleupagus.h"

static inline bool cidr4_match(const struct in_addr addr,
                               const struct in_addr net, uint8_t bits);
static inline bool cidr6_match(const struct in6_addr address,
                               const struct in6_addr network, uint8_t bits);
static inline int get_ip_version(const char *ip);

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
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || \
    defined(__APPLE__)
  const uint32_t *a = address.__u6_addr.__u6_addr32;
  const uint32_t *n = network.__u6_addr.__u6_addr32;
#else
  const uint32_t *a = address.s6_addr32;
  const uint32_t *n = network.s6_addr32;
#endif

  int bits_whole = bits >> 5;         // number of whole u32
  int bits_incomplete = bits & 0x1F;  // number of bits in incomplete u32

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
  if ((0 == res) && (1 == inet_pton(AF_INET6, ip, &out6))) {
    return AF_INET6;
  } else if (1 == res) {
    return AF_INET;
  } else {
    return -1;
  }
}

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
  }
  return false;
}

int get_ip_and_cidr(char *ip, sp_cidr *cidr) {
  char *mask = strchr(ip, '/');

  if (NULL == mask) {
    sp_log_err("config", "'%s' isn't a valid network mask, it seems that you forgot a '/'.", ip);
    return -1;
  }

  int masklen = strlen(mask+1);
  int imask = atoi(mask+1);
  if (masklen < 1 || masklen > 3 || !isdigit(*(mask+1)) || (masklen >= 2 && !isdigit(*(mask+2)))  || (masklen == 3 && !isdigit(*(mask+3))) || imask < 0 || imask > 128) {
    sp_log_err("config", "'%s' isn't a valid network mask.", mask + 1);
    return -1;
  }
  cidr->mask = (uint8_t)imask;

  ip[mask - ip] = '\0';  // NULL the '/' char

  cidr->ip_version = get_ip_version(ip);

  assert(cidr->ip_version == AF_INET6 || cidr->ip_version == AF_INET);

  if (AF_INET == cidr->ip_version) {
    if (cidr->mask > 32) {
      sp_log_err("config", "'%d' isn't a valid ipv4 mask.", cidr->mask);
      return -1;
    }
    inet_pton(AF_INET, ip, &(cidr->ip.ipv4));
  } else if (AF_INET6 == cidr->ip_version) {
    inet_pton(AF_INET6, ip, &(cidr->ip.ipv6));
  }

  ip[mask - ip] = '/';
  if (cidr->ip_version < 0) {
    sp_log_err("config", "Weird ip (%s) family", ip);
    return -1;
  }

  return 0;
}

bool /* success */ get_ip_str(char *dst, size_t dst_len, sp_cidr *cidr) {
  size_t ipstr_len = 0;
  void *ip = NULL;
  switch (cidr->ip_version) {
    case AF_INET:
      ipstr_len = INET_ADDRSTRLEN;
      ip = &cidr->ip.ipv4;
      break;
    case AF_INET6:
      ipstr_len = INET6_ADDRSTRLEN;
      ip = &cidr->ip.ipv6;
      break;
    default:
      return false;
  }

  if (dst_len < ipstr_len + 1 + 3 + 1) {
    return false;
  }
  if (!inet_ntop(cidr->ip_version, ip, dst, ipstr_len)) {
    return false;
  }
  ipstr_len = strlen(dst);
  snprintf(dst + ipstr_len, dst_len - ipstr_len, "/%d", cidr->mask);
  return true;
}