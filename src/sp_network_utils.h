#ifndef SP_NETWORK_UTILS_H
#define SP_NETWORK_UTILS_H

int get_ip_and_cidr(char *, sp_cidr *);
bool cidr_match(const char *, const sp_cidr *);
bool /* success */ get_ip_str(char *dst, size_t dst_len, sp_cidr *cidr);

#endif /*SP_NETWORK_UTILS_H*/
