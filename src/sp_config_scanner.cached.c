/* Generated by re2c */
#include "php_snuffleupagus.h"

enum YYCONDTYPE {
	yycinit,
	yyccond,
	yyccond_op,
	yycrule,
};


#define cs_log_error(fmt, ...) sp_log_err("config", fmt, ##__VA_ARGS__)
#define cs_log_info(fmt, ...) sp_log_msg("config", SP_LOG_INFO, fmt, ##__VA_ARGS__)
#define cs_log_warning(fmt, ...) sp_log_warn("config", fmt, ##__VA_ARGS__)


zend_string *sp_get_arg_string(sp_parsed_keyword *kw) {
  if (!kw || !kw->arg) {
    return NULL;
  }
  zend_string *ret = zend_string_init(kw->arg, kw->arglen, 1);
  char *pin, *pout;
  pin = pout = ZSTR_VAL(ret);
  char *pend = pin + ZSTR_LEN(ret);

  while (pin < pend) {
    if (*pin == '\\') {
      pin++;
    }
    *pout = *pin;
    pin++; pout++;
  }

  if (pin != pout) {
    size_t len = pout - ZSTR_VAL(ret);
    ret = zend_string_truncate(ret, len, 1);
    ZSTR_VAL(ret)[len] = 0;
  }

  return ret;
}

zend_string *sp_get_textual_representation(sp_parsed_keyword *parsed_rule) {
  // a rule is "sp.keyword...keyword(arg);\0"
  size_t len = 3; // sp + ;
  sp_parsed_keyword *kw;
  for (kw = parsed_rule; kw->kw; kw++) {
    len++; // .
    len += kw->kwlen;
    if (kw->argtype == SP_ARGTYPE_EMPTY) {
      len += 2; // ()
    } else if (kw->argtype == SP_ARGTYPE_STR) {
        len += 4;
        len += kw->arglen;
    }
  }
  zend_string *ret = zend_string_alloc(len, 1);
  char *ptr = ZSTR_VAL(ret);
  memcpy(ptr, "sp", 2); ptr += 2;
  for (kw = parsed_rule; kw->kw; kw++) {
    *ptr++ = '.';
    memcpy(ptr, kw->kw, kw->kwlen); ptr += kw->kwlen;
    if (kw->argtype == SP_ARGTYPE_EMPTY || kw->argtype == SP_ARGTYPE_STR || kw->argtype == SP_ARGTYPE_UNKNOWN) {
      *ptr++ = '(';
    }
    if (kw->argtype == SP_ARGTYPE_STR && kw->arg) {
      *ptr++ = '"';
      memcpy(ptr, kw->arg, kw->arglen); ptr += kw->arglen;
      *ptr++ = '"';
    }
    if (kw->argtype == SP_ARGTYPE_EMPTY || kw->argtype == SP_ARGTYPE_STR || kw->argtype == SP_ARGTYPE_UNKNOWN) {
      *ptr++ = ')';
    }
  }
  *ptr++ = ';';
  *ptr = 0;
  return ret;
}

static void str_dtor(zval *zv) {
  zend_string_release_ex(Z_STR_P(zv), 1);
}

// sy_ functions and macros are helpers for the shunting yard algorithm
#define sy_res_push(val) \
  if (cond_res_i >= 100) { cs_log_error("condition too complex on line %d", lineno); goto out; } \
  cond_res[cond_res_i++] = val;
#define sy_res_pop() cond_res[--cond_res_i]
#define sy_op_push(op) \
  if (cond_op_i >= 100) { cs_log_error("condition too complex on line %d", lineno); goto out; } \
  cond_op[cond_op_i++] = op;
#define sy_op_pop() cond_op[--cond_op_i]
#define sy_op_peek() cond_op[cond_op_i-1]

static inline int sy_op_precedence(char op) {
  switch (op) {
    case '!': return 120;
    case '<':
    case '>':
    case 'L': // <=
    case 'G': // >=
              return 90;
    case '&': return 70;
    case '|': return 60;
    case '=': return 20;
  }
  return 0;
}
static inline int sy_op_is_left_assoc(char op) {
  switch (op) {
    case '!': return 0;
  }
  return 1;
}
static int sy_apply_op(char op, int a, int b) {
  switch (op) {
    case '!': return !a;
    case '&': return (b && a);
    case '|': return (b || a);
    case '<': return (b < a);
    case 'L': return (b <= a);
    case 'G': return (b >= a);
    case '>': return (b > a);
    case '=': return (b == a);
  }
  return 0;
}

#define SY_APPLY_OP_FROM_STACK() \
  char op = sy_op_pop(); \
  int unary = (op == '!'); \
  if (cond_res_i < (2 - unary)) { cs_log_error("not enough input on line %d", lineno); goto out; } \
  int a = sy_res_pop(); \
  int b = unary ? 0 : sy_res_pop(); \
  int res = sy_apply_op(op, a, b); \
  sy_res_push(res);

#define TMPSTR(tmpstr, t2, t1) \
      char tmpstr[1024]; \
      size_t tmplen = MIN(t2-t1-2, 1023); \
      strncpy(tmpstr, t1+1, tmplen); \
      tmpstr[tmplen] = 0;


zend_result sp_config_scan(char *data, zend_result (*process_rule)(sp_parsed_keyword*))
{
  const char *YYCURSOR = data;
  const char *YYMARKER, *t1, *t2, *t3, *t4;
  const char *yyt1;
const char *yyt2;
const char *yyt3;
const char *yyt4;


  int ret = FAILURE;

  const int max_keywords = 16;
  sp_parsed_keyword parsed_rule[max_keywords+1];
  int kw_i = 0;

  HashTable vars;
  zend_hash_init(&vars, 10, NULL, str_dtor, 1);
  zend_hash_str_add_ptr(&vars, ZEND_STRL("PHP_VERSION_ID"), zend_string_init(ZEND_STRL(ZEND_TOSTR(PHP_VERSION_ID)), 1));


  int cond_res[100] = {1};
  int cond_res_i = 0;
  char cond_op[100] = {0};
  int cond_op_i = 0;

  int cond = yycinit;
  long lineno = 1;

  
{
	unsigned char yych;
	unsigned int yyaccept = 0;
	if (cond < 2) {
		if (cond < 1) {
			goto yyc_init;
		} else {
			goto yyc_cond;
		}
	} else {
		if (cond < 3) {
			goto yyc_cond_op;
		} else {
			goto yyc_rule;
		}
	}
/* *********************************** */
yyc_init:
	{
		static const unsigned char yybm[] = {
			 80,  80,  80,  80,  80,  80,  80,  80, 
			 80,  88,   0,  80,  80,  16,  80,  80, 
			 80,  80,  80,  80,  80,  80,  80,  80, 
			 80,  80,  80,  80,  80,  80,  80,  80, 
			 88,  80,  16,  80,  80,  80,  80,  80, 
			 80,  80,  80,  80,  80,  80,  80,  80, 
			112, 112, 112, 112, 112, 112, 112, 112, 
			112, 112,  80,  80,  80,  80,  80,  80, 
			 80, 112, 112, 112, 112, 112, 112, 112, 
			112, 112, 112, 112, 112, 112, 112, 112, 
			112, 112, 112, 112, 112, 112, 112, 112, 
			112, 112, 112,  80, 144,  80,  80, 112, 
			 80, 112, 112, 112, 112, 112, 112, 112, 
			112, 112, 112, 112, 112, 112, 112, 112, 
			112, 112, 112, 112, 112, 112, 112, 112, 
			112, 112, 112,  80,  80,  80,  80,  80, 
			 80,  80,  80,  80,  80,  80,  80,  80, 
			 80,  80,  80,  80,  80,  80,  80,  80, 
			 80,  80,  80,  80,  80,  80,  80,  80, 
			 80,  80,  80,  80,  80,  80,  80,  80, 
			 80,  80,  80,  80,  80,  80,  80,  80, 
			 80,  80,  80,  80,  80,  80,  80,  80, 
			 80,  80,  80,  80,  80,  80,  80,  80, 
			 80,  80,  80,  80,  80,  80,  80,  80, 
			 80,  80,  80,  80,  80,  80,  80,  80, 
			 80,  80,  80,  80,  80,  80,  80,  80, 
			 80,  80,  80,  80,  80,  80,  80,  80, 
			 80,  80,  80,  80,  80,  80,  80,  80, 
			 80,  80,  80,  80,  80,  80,  80,  80, 
			 80,  80,  80,  80,  80,  80,  80,  80, 
			 80,  80,  80,  80,  80,  80,  80,  80, 
			 80,  80,  80,  80,  80,  80,  80,  80, 
		};
		yych = *YYCURSOR;
		if (yybm[0+yych] & 8) {
			goto yy6;
		}
		if (yych <= '#') {
			if (yych <= '\n') {
				if (yych <= 0x00) goto yy2;
				if (yych <= 0x08) goto yy4;
				goto yy9;
			} else {
				if (yych == '\r') goto yy11;
				if (yych <= '"') goto yy4;
				goto yy12;
			}
		} else {
			if (yych <= '?') {
				if (yych == ';') goto yy12;
				goto yy4;
			} else {
				if (yych <= '@') goto yy15;
				if (yych == 's') goto yy16;
				goto yy4;
			}
		}
yy2:
		++YYCURSOR;
		{ ret = SUCCESS; goto out; }
yy4:
		++YYCURSOR;
yy5:
		{ cs_log_error("Parser error on line %d", lineno); goto out; }
yy6:
		yych = *++YYCURSOR;
		if (yybm[0+yych] & 8) {
			goto yy6;
		}
		{ goto yyc_init; }
yy9:
		++YYCURSOR;
		{ lineno++; goto yyc_init; }
yy11:
		yych = *++YYCURSOR;
		if (yych == '\n') goto yy9;
		goto yy5;
yy12:
		yych = *++YYCURSOR;
		if (yybm[0+yych] & 16) {
			goto yy12;
		}
		{ goto yyc_init; }
yy15:
		yyaccept = 0;
		yych = *(YYMARKER = ++YYCURSOR);
		switch (yych) {
		case 'c':	goto yy17;
		case 'e':	goto yy19;
		case 'i':	goto yy20;
		case 'l':	goto yy21;
		case 's':	goto yy22;
		case 'w':	goto yy23;
		default:	goto yy5;
		}
yy16:
		yyaccept = 0;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yych == 'e') goto yy24;
		if (yych == 'p') goto yy25;
		goto yy5;
yy17:
		yych = *++YYCURSOR;
		if (yych == 'o') goto yy27;
yy18:
		YYCURSOR = YYMARKER;
		if (yyaccept <= 4) {
			if (yyaccept <= 2) {
				if (yyaccept <= 1) {
					if (yyaccept == 0) {
						goto yy5;
					} else {
						yyt2 = YYCURSOR;
						goto yy67;
					}
				} else {
					yyt2 = YYCURSOR;
					goto yy71;
				}
			} else {
				if (yyaccept == 3) {
					goto yy67;
				} else {
					goto yy71;
				}
			}
		} else {
			if (yyaccept <= 6) {
				if (yyaccept == 5) {
					yyt2 = YYCURSOR;
					goto yy86;
				} else {
					yyt4 = YYCURSOR;
					goto yy91;
				}
			} else {
				if (yyaccept == 7) {
					goto yy86;
				} else {
					goto yy91;
				}
			}
		}
yy19:
		yych = *++YYCURSOR;
		if (yych == 'n') goto yy28;
		if (yych == 'r') goto yy29;
		goto yy18;
yy20:
		yych = *++YYCURSOR;
		if (yych == 'n') goto yy30;
		goto yy18;
yy21:
		yych = *++YYCURSOR;
		if (yych == 'o') goto yy31;
		goto yy18;
yy22:
		yych = *++YYCURSOR;
		if (yych == 'e') goto yy24;
		goto yy18;
yy23:
		yych = *++YYCURSOR;
		if (yych == 'a') goto yy32;
		goto yy18;
yy24:
		yych = *++YYCURSOR;
		if (yych == 't') goto yy33;
		goto yy18;
yy25:
		++YYCURSOR;
		{ kw_i = 0;  goto yyc_rule; }
yy27:
		yych = *++YYCURSOR;
		if (yych == 'n') goto yy34;
		goto yy18;
yy28:
		yych = *++YYCURSOR;
		if (yych == 'd') goto yy35;
		goto yy18;
yy29:
		yych = *++YYCURSOR;
		if (yych == 'r') goto yy36;
		goto yy18;
yy30:
		yych = *++YYCURSOR;
		if (yych == 'f') goto yy37;
		goto yy18;
yy31:
		yych = *++YYCURSOR;
		if (yych == 'g') goto yy38;
		goto yy18;
yy32:
		yych = *++YYCURSOR;
		if (yych == 'r') goto yy39;
		goto yy18;
yy33:
		yych = *++YYCURSOR;
		if (yych == '\t') goto yy40;
		if (yych == ' ') goto yy40;
		goto yy18;
yy34:
		yych = *++YYCURSOR;
		if (yych == 'd') goto yy42;
		goto yy18;
yy35:
		yych = *++YYCURSOR;
		if (yych == '_') goto yy43;
		goto yy18;
yy36:
		yych = *++YYCURSOR;
		if (yych == '"') goto yy18;
		if (yych == 'o') goto yy46;
		goto yy45;
yy37:
		yych = *++YYCURSOR;
		if (yych != 'o') goto yy18;
yy38:
		yych = *++YYCURSOR;
		if (yych == '"') goto yy18;
		goto yy48;
yy39:
		yych = *++YYCURSOR;
		if (yych == 'n') goto yy49;
		goto yy18;
yy40:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '\t') {
				if (yych <= 0x08) goto yy18;
				goto yy40;
			} else {
				if (yych == ' ') goto yy40;
				goto yy18;
			}
		} else {
			if (yych <= '_') {
				if (yych <= 'Z') {
					yyt1 = YYCURSOR;
					goto yy50;
				}
				if (yych <= '^') goto yy18;
				yyt1 = YYCURSOR;
				goto yy50;
			} else {
				if (yych <= '`') goto yy18;
				if (yych <= 'z') {
					yyt1 = YYCURSOR;
					goto yy50;
				}
				goto yy18;
			}
		}
yy42:
		yych = *++YYCURSOR;
		if (yych == 'i') goto yy52;
		goto yy18;
yy43:
		yych = *++YYCURSOR;
		if (yych == 'c') goto yy53;
		goto yy18;
yy44:
		yych = *++YYCURSOR;
yy45:
		if (yych <= 0x1F) {
			if (yych == '\t') goto yy44;
			goto yy18;
		} else {
			if (yych <= ' ') goto yy44;
			if (yych == '"') {
				yyt1 = YYCURSOR;
				goto yy54;
			}
			goto yy18;
		}
yy46:
		yych = *++YYCURSOR;
		if (yych == 'r') goto yy56;
		goto yy18;
yy47:
		yych = *++YYCURSOR;
yy48:
		if (yych <= 0x1F) {
			if (yych == '\t') goto yy47;
			goto yy18;
		} else {
			if (yych <= ' ') goto yy47;
			if (yych == '"') {
				yyt1 = YYCURSOR;
				goto yy57;
			}
			goto yy18;
		}
yy49:
		yych = *++YYCURSOR;
		if (yych == '"') goto yy18;
		if (yych == 'i') goto yy61;
		goto yy60;
yy50:
		yych = *++YYCURSOR;
		if (yybm[0+yych] & 32) {
			goto yy50;
		}
		if (yych == '\t') {
			yyt2 = YYCURSOR;
			goto yy62;
		}
		if (yych == ' ') {
			yyt2 = YYCURSOR;
			goto yy62;
		}
		goto yy18;
yy52:
		yych = *++YYCURSOR;
		if (yych == 't') goto yy64;
		goto yy18;
yy53:
		yych = *++YYCURSOR;
		if (yych == 'o') goto yy65;
		goto yy18;
yy54:
		yych = *++YYCURSOR;
		if (yybm[0+yych] & 64) {
			goto yy54;
		}
		if (yych <= '\r') goto yy18;
		if (yych <= '"') goto yy66;
		goto yy68;
yy56:
		yych = *++YYCURSOR;
		if (yych == '"') goto yy18;
		goto yy45;
yy57:
		yych = *++YYCURSOR;
		if (yych <= '\r') {
			if (yych == '\n') goto yy18;
			if (yych <= '\f') goto yy57;
			goto yy18;
		} else {
			if (yych <= '"') {
				if (yych <= '!') goto yy57;
				goto yy70;
			} else {
				if (yych == '\\') goto yy72;
				goto yy57;
			}
		}
yy59:
		yych = *++YYCURSOR;
yy60:
		if (yych <= 0x1F) {
			if (yych == '\t') goto yy59;
			goto yy18;
		} else {
			if (yych <= ' ') goto yy59;
			if (yych == '"') {
				yyt1 = YYCURSOR;
				goto yy74;
			}
			goto yy18;
		}
yy61:
		yych = *++YYCURSOR;
		if (yych == 'n') goto yy76;
		goto yy18;
yy62:
		yych = *++YYCURSOR;
		if (yych <= 0x1F) {
			if (yych == '\t') goto yy62;
			goto yy18;
		} else {
			if (yych <= ' ') goto yy62;
			if (yych == '"') {
				yyt3 = YYCURSOR;
				goto yy77;
			}
			goto yy18;
		}
yy64:
		yych = *++YYCURSOR;
		if (yych == 'i') goto yy79;
		goto yy18;
yy65:
		yych = *++YYCURSOR;
		if (yych == 'n') goto yy80;
		goto yy18;
yy66:
		yych = *++YYCURSOR;
		yyt2 = YYCURSOR;
		if (yych == ';') goto yy81;
yy67:
		t1 = yyt1;
		t2 = yyt2;
		{
      if (!cond_res[0]) { goto yyc_init; }
      TMPSTR(tmpstr, t2, t1);
      cs_log_error("[line %d]: %s", lineno, tmpstr);
      goto out;
    }
yy68:
		yych = *++YYCURSOR;
		if (yybm[0+yych] & 64) {
			goto yy54;
		}
		if (yych <= '\r') goto yy18;
		if (yych <= '"') goto yy82;
		goto yy68;
yy70:
		yych = *++YYCURSOR;
		yyt2 = YYCURSOR;
		if (yych == ';') goto yy83;
yy71:
		t1 = yyt1;
		t2 = yyt2;
		{
      if (!cond_res[0]) { goto yyc_init; }
      TMPSTR(tmpstr, t2, t1);
      cs_log_info("[line %d]: %s", lineno, tmpstr);
      goto yyc_init;
    }
yy72:
		yych = *++YYCURSOR;
		if (yych <= '\r') {
			if (yych == '\n') goto yy18;
			if (yych <= '\f') goto yy57;
			goto yy18;
		} else {
			if (yych <= '"') {
				if (yych <= '!') goto yy57;
				goto yy84;
			} else {
				if (yych == '\\') goto yy72;
				goto yy57;
			}
		}
yy74:
		yych = *++YYCURSOR;
		if (yych <= '\r') {
			if (yych == '\n') goto yy18;
			if (yych <= '\f') goto yy74;
			goto yy18;
		} else {
			if (yych <= '"') {
				if (yych <= '!') goto yy74;
				goto yy85;
			} else {
				if (yych == '\\') goto yy87;
				goto yy74;
			}
		}
yy76:
		yych = *++YYCURSOR;
		if (yych == 'g') goto yy89;
		goto yy18;
yy77:
		yych = *++YYCURSOR;
		if (yych <= '\r') {
			if (yych == '\n') goto yy18;
			if (yych <= '\f') goto yy77;
			goto yy18;
		} else {
			if (yych <= '"') {
				if (yych <= '!') goto yy77;
				goto yy90;
			} else {
				if (yych == '\\') goto yy92;
				goto yy77;
			}
		}
yy79:
		yych = *++YYCURSOR;
		if (yych == 'o') goto yy94;
		goto yy18;
yy80:
		yych = *++YYCURSOR;
		if (yych == 'd') goto yy95;
		goto yy18;
yy81:
		++YYCURSOR;
		goto yy67;
yy82:
		yyaccept = 1;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yybm[0+yych] & 128) {
			goto yy68;
		}
		if (yych <= '\r') {
			if (yych == '\n') {
				yyt2 = YYCURSOR;
				goto yy67;
			}
			if (yych <= '\f') goto yy54;
			yyt2 = YYCURSOR;
			goto yy67;
		} else {
			if (yych <= '"') {
				if (yych <= '!') goto yy54;
				goto yy66;
			} else {
				if (yych == ';') {
					yyt2 = YYCURSOR;
					goto yy96;
				}
				goto yy54;
			}
		}
yy83:
		++YYCURSOR;
		goto yy71;
yy84:
		yyaccept = 2;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yych <= '!') {
			if (yych <= '\n') {
				if (yych <= '\t') goto yy57;
				yyt2 = YYCURSOR;
				goto yy71;
			} else {
				if (yych == '\r') {
					yyt2 = YYCURSOR;
					goto yy71;
				}
				goto yy57;
			}
		} else {
			if (yych <= ';') {
				if (yych <= '"') goto yy70;
				if (yych <= ':') goto yy57;
				yyt2 = YYCURSOR;
				goto yy97;
			} else {
				if (yych == '\\') goto yy72;
				goto yy57;
			}
		}
yy85:
		yych = *++YYCURSOR;
		yyt2 = YYCURSOR;
		if (yych == ';') goto yy98;
yy86:
		t1 = yyt1;
		t2 = yyt2;
		{
      if (!cond_res[0]) { goto yyc_init; }
      TMPSTR(tmpstr, t2, t1);
      cs_log_warning("[line %d]: %s", lineno, tmpstr);
      goto yyc_init;
    }
yy87:
		yych = *++YYCURSOR;
		if (yych <= '\r') {
			if (yych == '\n') goto yy18;
			if (yych <= '\f') goto yy74;
			goto yy18;
		} else {
			if (yych <= '"') {
				if (yych <= '!') goto yy74;
				goto yy99;
			} else {
				if (yych == '\\') goto yy87;
				goto yy74;
			}
		}
yy89:
		yych = *++YYCURSOR;
		if (yych == '"') goto yy18;
		goto yy60;
yy90:
		yych = *++YYCURSOR;
		yyt4 = YYCURSOR;
		goto yy101;
yy91:
		t1 = yyt1;
		t2 = yyt2;
		t3 = yyt3;
		t4 = yyt4;
		{
      if (!cond_res[0]) { goto yyc_init; }
      char *key = (char*)t1;
      int keylen = t2-t1;
      zend_string *tmp = zend_hash_str_find_ptr(&vars, key, keylen);
      if (tmp) {
        zend_hash_str_del(&vars, key, keylen);
      }
      tmp = zend_string_init(t3+1, t4-t3-2, 1);
      zend_hash_str_add_ptr(&vars, key, keylen, tmp);
      goto yyc_init;
    }
yy92:
		yych = *++YYCURSOR;
		if (yych <= '\r') {
			if (yych == '\n') goto yy18;
			if (yych <= '\f') goto yy77;
			goto yy18;
		} else {
			if (yych <= '"') {
				if (yych <= '!') goto yy77;
				goto yy103;
			} else {
				if (yych == '\\') goto yy92;
				goto yy77;
			}
		}
yy94:
		yych = *++YYCURSOR;
		if (yych == 'n') goto yy104;
		goto yy18;
yy95:
		yych = *++YYCURSOR;
		if (yych == 'i') goto yy105;
		goto yy18;
yy96:
		yyaccept = 3;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yybm[0+yych] & 64) {
			goto yy54;
		}
		if (yych <= '\r') goto yy67;
		if (yych <= '"') goto yy66;
		goto yy68;
yy97:
		yyaccept = 4;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yych <= '\r') {
			if (yych == '\n') goto yy71;
			if (yych <= '\f') goto yy57;
			goto yy71;
		} else {
			if (yych <= '"') {
				if (yych <= '!') goto yy57;
				goto yy70;
			} else {
				if (yych == '\\') goto yy72;
				goto yy57;
			}
		}
yy98:
		++YYCURSOR;
		goto yy86;
yy99:
		yyaccept = 5;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yych <= '!') {
			if (yych <= '\n') {
				if (yych <= '\t') goto yy74;
				yyt2 = YYCURSOR;
				goto yy86;
			} else {
				if (yych == '\r') {
					yyt2 = YYCURSOR;
					goto yy86;
				}
				goto yy74;
			}
		} else {
			if (yych <= ';') {
				if (yych <= '"') goto yy85;
				if (yych <= ':') goto yy74;
				yyt2 = YYCURSOR;
				goto yy106;
			} else {
				if (yych == '\\') goto yy87;
				goto yy74;
			}
		}
yy100:
		yych = *++YYCURSOR;
yy101:
		if (yych <= 0x1F) {
			if (yych == '\t') goto yy100;
			goto yy91;
		} else {
			if (yych <= ' ') goto yy100;
			if (yych != ';') goto yy91;
		}
		++YYCURSOR;
		goto yy91;
yy103:
		yyaccept = 6;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yych <= ' ') {
			if (yych <= '\n') {
				if (yych <= 0x08) goto yy77;
				if (yych <= '\t') {
					yyt4 = YYCURSOR;
					goto yy107;
				}
				yyt4 = YYCURSOR;
				goto yy91;
			} else {
				if (yych == '\r') {
					yyt4 = YYCURSOR;
					goto yy91;
				}
				if (yych <= 0x1F) goto yy77;
				yyt4 = YYCURSOR;
				goto yy107;
			}
		} else {
			if (yych <= ':') {
				if (yych == '"') goto yy90;
				goto yy77;
			} else {
				if (yych <= ';') {
					yyt4 = YYCURSOR;
					goto yy109;
				}
				if (yych == '\\') goto yy92;
				goto yy77;
			}
		}
yy104:
		yych = *++YYCURSOR;
		if (yych == '\t') goto yy110;
		if (yych == ' ') goto yy110;
		goto yy18;
yy105:
		yych = *++YYCURSOR;
		if (yych == 't') goto yy113;
		goto yy18;
yy106:
		yyaccept = 7;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yych <= '\r') {
			if (yych == '\n') goto yy86;
			if (yych <= '\f') goto yy74;
			goto yy86;
		} else {
			if (yych <= '"') {
				if (yych <= '!') goto yy74;
				goto yy85;
			} else {
				if (yych == '\\') goto yy87;
				goto yy74;
			}
		}
yy107:
		yyaccept = 8;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yych <= ' ') {
			if (yych <= '\n') {
				if (yych <= 0x08) goto yy77;
				if (yych <= '\t') goto yy107;
				goto yy91;
			} else {
				if (yych == '\r') goto yy91;
				if (yych <= 0x1F) goto yy77;
				goto yy107;
			}
		} else {
			if (yych <= ':') {
				if (yych == '"') goto yy90;
				goto yy77;
			} else {
				if (yych <= ';') goto yy109;
				if (yych == '\\') goto yy92;
				goto yy77;
			}
		}
yy109:
		yyaccept = 8;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yych <= '\r') {
			if (yych == '\n') goto yy91;
			if (yych <= '\f') goto yy77;
			goto yy91;
		} else {
			if (yych <= '"') {
				if (yych <= '!') goto yy77;
				goto yy90;
			} else {
				if (yych == '\\') goto yy92;
				goto yy77;
			}
		}
yy110:
		yych = *++YYCURSOR;
		if (yych == '\t') goto yy110;
		if (yych == ' ') goto yy110;
		{ cond_res_i = 0; goto yyc_cond; }
yy113:
		yych = *++YYCURSOR;
		if (yych != 'i') goto yy18;
		yych = *++YYCURSOR;
		if (yych != 'o') goto yy18;
		yych = *++YYCURSOR;
		if (yych != 'n') goto yy18;
yy116:
		yych = *++YYCURSOR;
		if (yych <= 0x1F) {
			if (yych == '\t') goto yy116;
			goto yy18;
		} else {
			if (yych <= ' ') goto yy116;
			if (yych != ';') goto yy18;
		}
		++YYCURSOR;
		{ cond_res[0] = 1; cond_res_i = 0; goto yyc_init; }
	}
/* *********************************** */
yyc_cond:
	{
		static const unsigned char yybm[] = {
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  72,   0,  64,  64,   0,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 72,  64,   0,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			112, 112, 112, 112, 112, 112, 112, 112, 
			112, 112,  64,  64,  64,  64,  64,  64, 
			 64,  96,  96,  96,  96,  96,  96,  96, 
			 96,  96,  96,  96,  96,  96,  96,  96, 
			 96,  96,  96,  96,  96,  96,  96,  96, 
			 96,  96,  96,  64, 128,  64,  64,  96, 
			 64,  96,  96,  96,  96,  96,  96,  96, 
			 96,  96,  96,  96,  96,  96,  96,  96, 
			 96,  96,  96,  96,  96,  96,  96,  96, 
			 96,  96,  96,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
		};
		yych = *YYCURSOR;
		if (yybm[0+yych] & 8) {
			goto yy124;
		}
		if (yych <= '(') {
			if (yych <= '\r') {
				if (yych <= 0x08) goto yy122;
				if (yych <= '\n') goto yy127;
				if (yych >= '\r') goto yy129;
			} else {
				if (yych <= 0x1F) goto yy122;
				if (yych <= '!') goto yy130;
				if (yych >= '(') goto yy132;
			}
		} else {
			if (yych <= 'Z') {
				if (yych <= '/') goto yy122;
				if (yych <= '9') {
					yyt1 = YYCURSOR;
					goto yy134;
				}
				if (yych >= 'A') {
					yyt1 = YYCURSOR;
					goto yy137;
				}
			} else {
				if (yych <= '_') {
					if (yych >= '_') {
						yyt1 = YYCURSOR;
						goto yy137;
					}
				} else {
					if (yych <= '`') goto yy122;
					if (yych <= 'z') {
						yyt1 = YYCURSOR;
						goto yy137;
					}
				}
			}
		}
yy122:
		++YYCURSOR;
yy123:
		{ cs_log_error("Syntax error in condition on line %d", lineno); goto out; }
yy124:
		yych = *++YYCURSOR;
		if (yybm[0+yych] & 8) {
			goto yy124;
		}
		{ goto yyc_cond; }
yy127:
		++YYCURSOR;
		{ lineno++; goto yyc_cond; }
yy129:
		yych = *++YYCURSOR;
		if (yych == '\n') goto yy127;
		goto yy123;
yy130:
		++YYCURSOR;
		t1 = YYCURSOR - 1;
		{ sy_op_push(*t1); goto yyc_cond; }
yy132:
		++YYCURSOR;
		t1 = YYCURSOR - 1;
		{ sy_op_push(*t1); goto yyc_cond; }
yy134:
		yych = *++YYCURSOR;
		if (yybm[0+yych] & 16) {
			goto yy134;
		}
		t1 = yyt1;
		t2 = YYCURSOR;
		{ sy_res_push(atoi(t1));  goto yyc_cond_op; }
yy137:
		yyaccept = 0;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yybm[0+yych] & 32) {
			goto yy137;
		}
		if (yych == '(') goto yy140;
yy139:
		t1 = yyt1;
		t2 = YYCURSOR;
		{
      zend_string *tmp = zend_hash_str_find_ptr(&vars, t1, t2-t1);
      if (!tmp) {
        cs_log_error("unknown variable in condition on line %d", lineno);
        goto out;
      }
      sy_res_push(atoi(ZSTR_VAL(tmp)));
      goto yyc_cond_op;
    }
yy140:
		yych = *++YYCURSOR;
		if (yych == '"') {
			yyt2 = YYCURSOR;
			goto yy142;
		}
		if (yych == ')') {
			yyt2 = YYCURSOR;
			goto yy144;
		}
yy141:
		YYCURSOR = YYMARKER;
		if (yyaccept == 0) {
			goto yy139;
		} else {
			goto yy145;
		}
yy142:
		yych = *++YYCURSOR;
		if (yybm[0+yych] & 64) {
			goto yy142;
		}
		if (yych <= '\r') goto yy141;
		if (yych <= '"') goto yy146;
		goto yy147;
yy144:
		++YYCURSOR;
yy145:
		t1 = yyt1;
		t3 = yyt2;
		t2 = yyt2 - 1;
		t4 = YYCURSOR - 1;
		{
      if (t4-t3 >= 2 && strlen("extension_loaded") == t2-t1 && strncmp("extension_loaded", t1, t2-t1) == 0) {
        int is_loaded = (zend_hash_str_find_ptr(&module_registry, t3+1, t4-t3-2) != NULL);
        sy_res_push(is_loaded);
      } else {
        cs_log_error("unknown function in condition on line %d", lineno);
        goto out;
      }
      goto yyc_cond_op;
    }
yy146:
		yych = *++YYCURSOR;
		if (yych == ')') goto yy144;
		goto yy141;
yy147:
		yych = *++YYCURSOR;
		if (yybm[0+yych] & 64) {
			goto yy142;
		}
		if (yych <= '\r') goto yy141;
		if (yych >= '#') goto yy147;
		yych = *++YYCURSOR;
		if (yybm[0+yych] & 128) {
			goto yy147;
		}
		if (yych <= '\r') {
			if (yych == '\n') goto yy141;
			if (yych <= '\f') goto yy142;
			goto yy141;
		} else {
			if (yych <= '"') {
				if (yych <= '!') goto yy142;
				goto yy146;
			} else {
				if (yych != ')') goto yy142;
			}
		}
		yyaccept = 1;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yybm[0+yych] & 64) {
			goto yy142;
		}
		if (yych <= '\r') goto yy145;
		if (yych <= '"') goto yy146;
		goto yy147;
	}
/* *********************************** */
yyc_cond_op:
	{
		static const unsigned char yybm[] = {
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0, 128,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			128,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
			  0,   0,   0,   0,   0,   0,   0,   0, 
		};
		yych = *YYCURSOR;
		if (yybm[0+yych] & 128) {
			goto yy155;
		}
		if (yych <= ')') {
			if (yych <= '\r') {
				if (yych <= 0x08) goto yy153;
				if (yych <= '\n') goto yy158;
				if (yych >= '\r') goto yy160;
			} else {
				if (yych == '&') {
					yyt1 = YYCURSOR;
					goto yy161;
				}
				if (yych >= ')') goto yy162;
			}
		} else {
			if (yych <= '=') {
				if (yych <= ':') goto yy153;
				if (yych <= ';') goto yy164;
				if (yych <= '<') {
					yyt1 = YYCURSOR;
					goto yy166;
				}
				yyt1 = YYCURSOR;
				goto yy168;
			} else {
				if (yych <= '>') {
					yyt1 = YYCURSOR;
					goto yy166;
				}
				if (yych == '|') {
					yyt1 = YYCURSOR;
					goto yy169;
				}
			}
		}
yy153:
		++YYCURSOR;
yy154:
		{ cs_log_error("Syntax error in condition on line %d", lineno); goto out; }
yy155:
		yych = *++YYCURSOR;
		if (yybm[0+yych] & 128) {
			goto yy155;
		}
		{ goto yyc_cond_op; }
yy158:
		++YYCURSOR;
		{ lineno++; goto yyc_cond_op; }
yy160:
		yych = *++YYCURSOR;
		if (yych == '\n') goto yy158;
		goto yy154;
yy161:
		yych = *++YYCURSOR;
		if (yych == '&') goto yy170;
		goto yy154;
yy162:
		++YYCURSOR;
		{
      while (cond_op_i && sy_op_peek() != '(') {
        SY_APPLY_OP_FROM_STACK();
      }
      if (cond_op_i == 0 || sy_op_peek() != '(') {
        cs_log_error("unbalanced parathesis on line %d", lineno); goto out;
      }
      cond_op_i--;
      goto yyc_cond_op;
    }
yy164:
		++YYCURSOR;
		{
      while (cond_op_i) {
        if (sy_op_peek() == '(') { cs_log_error("unbalanced parathesis on line %d", lineno); goto out; }
        SY_APPLY_OP_FROM_STACK();
      }
      if (cond_res_i > 1) { cs_log_error("invalid condition on line %d", lineno); goto out; }
      goto yyc_init;
    }
yy166:
		yych = *++YYCURSOR;
		if (yych == '=') goto yy170;
yy167:
		t1 = yyt1;
		t2 = YYCURSOR;
		{
      char op1 = *t1;
      if (t2-t1 == 2) {
        switch (op1) {
          case '<': op1 = 'L'; break; // <=
          case '>': op1 = 'G'; break; // >=
        }
      }
      while (cond_op_i && sy_op_peek() != '(' && ((sy_op_precedence(sy_op_peek()) > sy_op_precedence(*t1)) || (sy_op_precedence(sy_op_peek()) == sy_op_precedence(*t1) && sy_op_is_left_assoc(*t1)))) {
        SY_APPLY_OP_FROM_STACK();
      }
      sy_op_push(*t1);
      goto yyc_cond;
    }
yy168:
		yych = *++YYCURSOR;
		if (yych == '=') goto yy170;
		goto yy154;
yy169:
		yych = *++YYCURSOR;
		if (yych != '|') goto yy154;
yy170:
		++YYCURSOR;
		goto yy167;
	}
/* *********************************** */
yyc_rule:
	{
		static const unsigned char yybm[] = {
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  88,  16,  64,  64,   0,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 88,  64,   0,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 96,  96,  96,  96,  96,  96,  96,  96, 
			 96,  96,  64,  64,  64,  64,  64,  64, 
			 64,  96,  96,  96,  96,  96,  96,  96, 
			 96,  96,  96,  96,  96,  96,  96,  96, 
			 96,  96,  96,  96,  96,  96,  96,  96, 
			 96,  96,  96,  64, 128,  64,  64,  96, 
			 64,  96,  96,  96,  96,  96,  96,  96, 
			 96,  96,  96,  96,  96,  96,  96,  96, 
			 96,  96,  96,  96,  96,  96,  96,  96, 
			 96,  96,  96,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
			 64,  64,  64,  64,  64,  64,  64,  64, 
		};
		yych = *YYCURSOR;
		if (yybm[0+yych] & 8) {
			goto yy175;
		}
		if (yych <= '\r') {
			if (yych <= 0x08) goto yy173;
			if (yych <= '\n') goto yy178;
			if (yych >= '\r') goto yy179;
		} else {
			if (yych <= '.') {
				if (yych >= '.') goto yy180;
			} else {
				if (yych == ';') goto yy181;
			}
		}
yy173:
		++YYCURSOR;
yy174:
		{ goto end_of_rule; }
yy175:
		yych = *++YYCURSOR;
		if (yybm[0+yych] & 8) {
			goto yy175;
		}
		{  goto yyc_rule; }
yy178:
		yyaccept = 0;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yych <= '\r') {
			if (yych <= 0x08) goto yy174;
			if (yych <= '\n') {
				yyt1 = YYCURSOR;
				goto yy183;
			}
			if (yych <= '\f') goto yy174;
			yyt1 = YYCURSOR;
			goto yy186;
		} else {
			if (yych <= ' ') {
				if (yych <= 0x1F) goto yy174;
				yyt1 = YYCURSOR;
				goto yy183;
			} else {
				if (yych == '.') {
					yyt1 = YYCURSOR;
					goto yy187;
				}
				goto yy174;
			}
		}
yy179:
		yyaccept = 0;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yych == '\n') goto yy189;
		goto yy174;
yy180:
		yych = *++YYCURSOR;
		if (yych <= '^') {
			if (yych <= '@') goto yy174;
			if (yych <= 'Z') {
				yyt1 = YYCURSOR;
				goto yy190;
			}
			goto yy174;
		} else {
			if (yych == '`') goto yy174;
			if (yych <= 'z') {
				yyt1 = YYCURSOR;
				goto yy190;
			}
			goto yy174;
		}
yy181:
		++YYCURSOR;
		{
      end_of_rule:
      if (!cond_res[0]) { goto yyc_init; }
      parsed_rule[kw_i++] = (sp_parsed_keyword){0, 0, 0, 0, 0, 0};
      if (process_rule && process_rule(parsed_rule) != SUCCESS) {
        goto out;
      }
      goto yyc_init;
    }
yy183:
		yych = *++YYCURSOR;
		if (yybm[0+yych] & 16) {
			goto yy183;
		}
		if (yych == '\r') goto yy186;
		if (yych == '.') goto yy187;
yy185:
		YYCURSOR = YYMARKER;
		if (yyaccept <= 1) {
			if (yyaccept == 0) {
				goto yy174;
			} else {
				yyt3 = yyt4 = NULL;
				yyt2 = YYCURSOR;
				goto yy192;
			}
		} else {
			goto yy192;
		}
yy186:
		yych = *++YYCURSOR;
		if (yych == '\n') goto yy183;
		goto yy185;
yy187:
		++YYCURSOR;
		YYCURSOR = yyt1;
		{  lineno++; goto yyc_rule; }
yy189:
		yych = *++YYCURSOR;
		if (yych <= '\r') {
			if (yych <= 0x08) goto yy185;
			if (yych <= '\n') {
				yyt1 = YYCURSOR;
				goto yy183;
			}
			if (yych <= '\f') goto yy185;
			yyt1 = YYCURSOR;
			goto yy186;
		} else {
			if (yych <= ' ') {
				if (yych <= 0x1F) goto yy185;
				yyt1 = YYCURSOR;
				goto yy183;
			} else {
				if (yych == '.') {
					yyt1 = YYCURSOR;
					goto yy187;
				}
				goto yy185;
			}
		}
yy190:
		yyaccept = 1;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yybm[0+yych] & 32) {
			goto yy190;
		}
		if (yych == '(') {
			yyt2 = YYCURSOR;
			goto yy193;
		}
		yyt3 = yyt4 = NULL;
		yyt2 = YYCURSOR;
yy192:
		t1 = yyt1;
		t2 = yyt2;
		t3 = yyt3;
		t4 = yyt4;
		{
      if (!cond_res[0]) { goto yyc_rule; }
      if (kw_i == max_keywords) {
        cs_log_error("Too many keywords in rule (more than %d) on line %d", max_keywords, lineno);
        goto out;
      }
      sp_parsed_keyword kw = {.kw = (char*)t1, .kwlen = t2-t1, .arg = (char*)t3, .arglen = t4-t3, .argtype = SP_ARGTYPE_UNKNOWN, .lineno = lineno};
      if (t3 && t4) {
        if (t3 == t4) {
          kw.argtype = SP_ARGTYPE_EMPTY;
        } else if (t4-t3 >= 2 && *t3 == '"') {
          kw.arg = (char*)t3 + 1;
          kw.arglen = t4 - t3 - 2;
          kw.argtype = SP_ARGTYPE_STR;
        } else {
          zend_string *tmp = zend_hash_str_find_ptr(&vars, t3, t4-t3);
          if (!tmp) {
            cs_log_error("unknown variable on line %d", lineno);
            goto out;
          }
          kw.arg = ZSTR_VAL(tmp);
          kw.arglen = ZSTR_LEN(tmp);
          kw.argtype = SP_ARGTYPE_STR;
        }
      } else {
        kw.argtype = SP_ARGTYPE_NONE;
      }
      parsed_rule[kw_i++] = kw;
      goto yyc_rule;
    }
yy193:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '"') {
				if (yych <= '!') goto yy185;
				yyt3 = YYCURSOR;
			} else {
				if (yych == ')') {
					yyt3 = yyt4 = YYCURSOR;
					goto yy196;
				}
				goto yy185;
			}
		} else {
			if (yych <= '_') {
				if (yych <= 'Z') {
					yyt3 = YYCURSOR;
					goto yy197;
				}
				if (yych <= '^') goto yy185;
				yyt3 = YYCURSOR;
				goto yy197;
			} else {
				if (yych <= '`') goto yy185;
				if (yych <= 'z') {
					yyt3 = YYCURSOR;
					goto yy197;
				}
				goto yy185;
			}
		}
yy194:
		yych = *++YYCURSOR;
		if (yybm[0+yych] & 64) {
			goto yy194;
		}
		if (yych <= '\r') goto yy185;
		if (yych <= '"') goto yy199;
		goto yy200;
yy196:
		++YYCURSOR;
		goto yy192;
yy197:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= ')') {
				if (yych <= '(') goto yy185;
				yyt4 = YYCURSOR;
				goto yy196;
			} else {
				if (yych <= '/') goto yy185;
				if (yych <= '9') goto yy197;
				goto yy185;
			}
		} else {
			if (yych <= '_') {
				if (yych <= 'Z') goto yy197;
				if (yych <= '^') goto yy185;
				goto yy197;
			} else {
				if (yych <= '`') goto yy185;
				if (yych <= 'z') goto yy197;
				goto yy185;
			}
		}
yy199:
		yych = *++YYCURSOR;
		if (yych == ')') {
			yyt4 = YYCURSOR;
			goto yy196;
		}
		goto yy185;
yy200:
		yych = *++YYCURSOR;
		if (yybm[0+yych] & 64) {
			goto yy194;
		}
		if (yych <= '\r') goto yy185;
		if (yych >= '#') goto yy200;
		yych = *++YYCURSOR;
		if (yybm[0+yych] & 128) {
			goto yy200;
		}
		if (yych <= '\r') {
			if (yych == '\n') goto yy185;
			if (yych <= '\f') goto yy194;
			goto yy185;
		} else {
			if (yych <= '"') {
				if (yych <= '!') goto yy194;
				goto yy199;
			} else {
				if (yych != ')') goto yy194;
				yyt4 = YYCURSOR;
			}
		}
		yyaccept = 2;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yybm[0+yych] & 64) {
			goto yy194;
		}
		if (yych <= '\r') goto yy192;
		if (yych <= '"') goto yy199;
		goto yy200;
	}
}

out:
  zend_hash_destroy(&vars);
  return ret;
}