#ifndef SP_NETWORK_UTILS_H
#define SP_NETWORK_UTILS_H

int get_ip_and_cidr(char *, sp_cidr *);
bool cidr_match(const char *, const sp_cidr *);
void apply_mask_on_ip(char *, const char * const);

#endif /*SP_NETWORK_UTILS_H*/
