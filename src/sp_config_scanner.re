#include "php_snuffleupagus.h"

#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

/*!types:re2c*/

#define cs_log_error(fmt, ...) sp_log_err("config", fmt, ##__VA_ARGS__)
#define cs_log_info(fmt, ...) sp_log_msg("config", SP_LOG_INFO, fmt, ##__VA_ARGS__)
#define cs_log_warning(fmt, ...) sp_log_warn("config", fmt, ##__VA_ARGS__)

#define MAX_CONDITIONS 100
#define MAX_KEYWORDS 16


zend_string *sp_get_arg_string(sp_parsed_keyword const *const kw) {
  if (!kw || !kw->arg) {
    return NULL;
  }

  zend_string *ret = zend_string_init(kw->arg, kw->arglen, 1);
  char *pin, *pout;
  pin = pout = ZSTR_VAL(ret);
  char const *const pend = pin + ZSTR_LEN(ret);

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

zend_string *sp_get_textual_representation(sp_parsed_keyword const *const parsed_rule) {
  // a rule is "sp.keyword...keyword(arg);\0"
  size_t len = 3; // "sp" + ";"
  for (const sp_parsed_keyword *kw = parsed_rule; kw->kw; kw++) {
    len++; // .
    len += kw->kwlen;
    if (kw->argtype == SP_ARGTYPE_EMPTY) {
      len += 2; // ()
    } else if (kw->argtype == SP_ARGTYPE_STR) {
      len += 2; // ("
      len += kw->arglen;
      len += 2; // ")
    }
  }

  zend_string *ret = zend_string_alloc(len, 1);
  char *ptr = ZSTR_VAL(ret);

  memcpy(ptr, "sp", 2);
  ptr += 2;

  for (const sp_parsed_keyword *kw = parsed_rule; kw->kw; kw++) {
    *ptr++ = '.';

    memcpy(ptr, kw->kw, kw->kwlen);
    ptr += kw->kwlen;

    if (kw->argtype == SP_ARGTYPE_EMPTY || kw->argtype == SP_ARGTYPE_STR || kw->argtype == SP_ARGTYPE_UNKNOWN) {
      *ptr++ = '(';
    }
    if (kw->argtype == SP_ARGTYPE_STR && kw->arg) {
      *ptr++ = '"';
      memcpy(ptr, kw->arg, kw->arglen);
      ptr += kw->arglen;
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
  if (cond_res_i >= MAX_CONDITIONS) { cs_log_error("condition too complex on line %zu", lineno); goto out; } \
  cond_res[cond_res_i++] = val;
#define sy_res_pop() cond_res[--cond_res_i]
#define sy_op_push(op) \
  if (cond_op_i >= MAX_CONDITIONS) { cs_log_error("condition too complex on line %zu", lineno); goto out; } \
  cond_op[cond_op_i++] = op;
#define sy_op_pop() cond_op[--cond_op_i]
#define sy_op_peek() cond_op[cond_op_i-1]

static inline int sy_op_precedence(const char op) {
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

static inline int sy_op_is_left_assoc(const char op) {
  switch (op) {
    case '!': return 0;
  }
  return 1;
}

static int sy_apply_op(const char op, const int a, const int b) {
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
  if (cond_res_i < (2 - unary)) { cs_log_error("not enough input on line %zu", lineno); goto out; } \
  int a = sy_res_pop(); \
  int b = unary ? 0 : sy_res_pop(); \
  int res = sy_apply_op(op, a, b); \
  sy_res_push(res);

#define TMPSTR(tmpstr, t2, t1) \
      char tmpstr[1024]; \
      size_t tmplen = MIN(t2-t1-2, 1023); \
      strncpy(tmpstr, t1+1, tmplen); \
      tmpstr[tmplen] = 0;


zend_result sp_config_scan(const char *data, zend_result (*process_rule)(sp_parsed_keyword*))
{
  const char *YYMARKER, *t1, *t2, *t3, *t4;

  int ret = FAILURE;
  sp_parsed_keyword parsed_rule[MAX_KEYWORDS+1];
  int kw_i = 0;

  HashTable vars;
  zend_hash_init(&vars, 10, NULL, str_dtor, 1);
  zend_hash_str_add_ptr(&vars, ZEND_STRL("PHP_VERSION_ID"), zend_string_init(ZEND_STRL(ZEND_TOSTR(PHP_VERSION_ID)), 1));

  int cond_res[MAX_CONDITIONS] = {1};
  int cond_res_i = 0;
  char cond_op[MAX_CONDITIONS] = {0};
  int cond_op_i = 0;

  int cond = yycinit;
  size_t lineno = 1;

  /*!stags:re2c format = 'const char *@@;\n'; */
  /*!re2c
    re2c:define:YYCTYPE = char;
    re2c:define:YYCURSOR = data;
    //re2c:sentinel = 0;
    re2c:yyfill:enable = 0;
    re2c:eof = -1;
    re2c:flags:tags = 1;
    re2c:api:style = free-form;
    re2c:define:YYGETCONDITION = "cond";
    re2c:define:YYSETCONDITION = "cond = @@;";

    end = "\x00";
    newline = "\r"?"\n";
    whitespace = [ \t];
    keyword = [a-zA-Z][a-zA-Z0-9_]*;
    string = ["] ("\\"["] | [^"\r\n\x00])* ["];

    <init> *                 { cs_log_error("parser error on line %zu", lineno); goto out; }
    <init> whitespace+       { goto yyc_init; }
    <init> [;#] [^\r\n\x00]* { goto yyc_init; }
    <init> newline           { lineno++; goto yyc_init; }
    <init> "sp"              { kw_i = 0;  goto yyc_rule; }
    <init> end               { ret = SUCCESS; goto out; }
    <init> "@"? "set" whitespace+ @t1 keyword @t2 whitespace+ @t3 string @t4 whitespace* ";" {
      if (!cond_res[0]) { goto yyc_init; }
      const char *key = t1;
      size_t keylen = t2 - t1;
      zend_string *tmp = zend_hash_str_find_ptr(&vars, key, keylen);
      if (tmp) {
        zend_hash_str_del(&vars, key, keylen);
      }
      tmp = zend_string_init(t3+1, t4-t3-2, 1);  // `-2` for the surrounding double quotes.
      zend_hash_str_add_ptr(&vars, key, keylen, tmp);
      goto yyc_init;
    }
    <init> "@condition" whitespace+         { cond_res_i = 0; goto yyc_cond; }
    <init> "@end_condition" whitespace* ";" { cond_res[0] = 1; cond_res_i = 0; goto yyc_init; }
    <init> ( "@log" | "@info" ) whitespace+ @t1 string @t2 ";" {
      if (!cond_res[0]) { goto yyc_init; }
      TMPSTR(tmpstr, t2, t1);
      cs_log_info("[line %zu]: %s", lineno, tmpstr);
      goto yyc_init;
    }
    <init> ( "@warn" | "@warning" ) whitespace+ @t1 string @t2 ";" {
      if (!cond_res[0]) { goto yyc_init; }
      TMPSTR(tmpstr, t2, t1);
      cs_log_warning("[line %zu]: %s", lineno, tmpstr);
      goto yyc_init;
    }
    <init> ( "@err" | "@error" ) whitespace+ @t1 string @t2 ";" {
      if (!cond_res[0]) { goto yyc_init; }
      TMPSTR(tmpstr, t2, t1);
      cs_log_error("[line %zu]: %s", lineno, tmpstr);
      goto out;
    }


    <cond> whitespace+ { goto yyc_cond; }
    <cond> newline     { lineno++; goto yyc_cond; }
    <cond> @t1 keyword @t2 "(" @t3 string? @t4 ")" {
      if (t4-t3 >= 2 && strlen("extension_loaded") == t2-t1 && strncmp("extension_loaded", t1, t2-t1) == 0) {
        int is_loaded = (zend_hash_str_find_ptr(&module_registry, t3+1, t4-t3-2) != NULL);
        sy_res_push(is_loaded);
      } else {
        cs_log_error("unknown function in condition on line %zu", lineno);
        goto out;
      }
      goto yyc_cond_op;
    }
    <cond> @t1 keyword @t2 {
      zend_string *tmp = zend_hash_str_find_ptr(&vars, t1, t2-t1);
      if (!tmp) {
        cs_log_error("unknown variable in condition on line %zu", lineno);
        goto out;
      }
      sy_res_push(atoi(ZSTR_VAL(tmp)));
      goto yyc_cond_op;
    }
    <cond> @t1 [0-9]+ @t2 { sy_res_push(atoi(t1));  goto yyc_cond_op; }
    <cond> @t1 "!"        { sy_op_push(*t1); goto yyc_cond; }
    <cond> @t1 "("        { sy_op_push(*t1); goto yyc_cond; }
    <cond_op> whitespace+ { goto yyc_cond_op; }
    <cond_op> newline     { lineno++; goto yyc_cond_op; }
    <cond_op> @t1 ( "&&" | "||" | "<" | ">" | "==" | "<=" | ">=") @t2 {
      char op1 = *t1;
      if (t2-t1 == 2) {
        switch (op1) {
          case '<': op1 = 'L'; break; // <=
          case '>': op1 = 'G'; break; // >=
        }
      }
      while (cond_op_i &&
	     sy_op_peek() != '(' &&
	       (
	         (sy_op_precedence(sy_op_peek()) > sy_op_precedence(*t1)) ||
	         (sy_op_precedence(sy_op_peek()) == sy_op_precedence(*t1) && sy_op_is_left_assoc(*t1))
	       )
	     ) {
        SY_APPLY_OP_FROM_STACK();
      }
      sy_op_push(*t1);
      goto yyc_cond;
    }
    <cond_op> ")" {
      while (cond_op_i && sy_op_peek() != '(') {
        SY_APPLY_OP_FROM_STACK();
      }
      if (cond_op_i == 0 || sy_op_peek() != '(') {
        cs_log_error("unbalanced parenthesis on line %zu", lineno); goto out;
      }
      cond_op_i--;
      goto yyc_cond_op;
    }
    <cond_op> ";" {
      while (cond_op_i) {
        if (sy_op_peek() == '(') { cs_log_error("unbalanced parenthesis on line %zu", lineno); goto out; }
        SY_APPLY_OP_FROM_STACK();
      }
      if (cond_res_i > 1) { cs_log_error("invalid condition on line %zu", lineno); goto out; }
      goto yyc_init;
    }
    <cond, cond_op> * { cs_log_error("syntax error in condition on line %zu", lineno); goto out; }

    <rule> whitespace+     {  goto yyc_rule; }
    <rule> newline / ( newline | whitespace )* "." {  lineno++; goto yyc_rule; }
    <rule> "." @t1 keyword @t2 ( "(" @t3 ( string? | keyword ) @t4 ")" )? {
      if (!cond_res[0]) { goto yyc_rule; }
      if (kw_i == MAX_KEYWORDS) {
        cs_log_error("too many keywords in rule (more than %d) on line %zu", MAX_KEYWORDS, lineno);
        goto out;
      }
      sp_parsed_keyword kw = {
        .kw = t1,
	.kwlen = t2-t1,
	.arg = t3,
	.arglen = t4-t3,
	.argtype = SP_ARGTYPE_UNKNOWN,
	.lineno = lineno
      };
      if (t3 && t4) {
        if (t3 == t4) {
          kw.argtype = SP_ARGTYPE_EMPTY;
        } else if (t4-t3 >= 2 && *t3 == '"') {
          kw.arg = t3 + 1;
          kw.arglen = t4 - t3 - 2;
          kw.argtype = SP_ARGTYPE_STR;
        } else {
          zend_string *tmp = zend_hash_str_find_ptr(&vars, t3, t4-t3);
          if (!tmp) {
            cs_log_error("unknown variable on line %zu", lineno);
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
    <rule> ";"     {
      end_of_rule:
      if (!cond_res[0]) { goto yyc_init; }
      parsed_rule[kw_i++] = (sp_parsed_keyword){0, 0, 0, 0, 0, 0};
      if (process_rule && process_rule(parsed_rule) != SUCCESS) {
        goto out;
      }
      goto yyc_init;
    }
    <rule> *       { goto end_of_rule; }
  */
out:
  zend_hash_destroy(&vars);
  return ret;
}
