
#ifndef SP_CONFIG_SCANNER_H
#define SP_CONFIG_SCANNER_H

typedef enum {
  SP_ARGTYPE_UNKNOWN = 0,
  SP_ARGTYPE_NONE,
  SP_ARGTYPE_EMPTY,
  SP_ARGTYPE_STR
} sp_argtype;

typedef struct {
  const char *kw; // keyword points directly to the parsed input text and as such is not null-terminated
  size_t kwlen;
  const char *arg; // optional argument / can be not null terminated
  size_t arglen;
  sp_argtype argtype;
  size_t lineno;
} sp_parsed_keyword;

zend_result sp_config_scan(const char *data, zend_result (*process_rule)(sp_parsed_keyword*));
zend_string *sp_get_arg_string(sp_parsed_keyword const *const kw);
zend_string *sp_get_textual_representation(sp_parsed_keyword const *const parsed_rule);

#endif
