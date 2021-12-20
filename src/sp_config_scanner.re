#include "php_snuffleupagus.h"

/*!types:re2c*/

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
  /*!stags:re2c format = 'const char *@@;\n'; */

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

  /*!re2c
    re2c:define:YYCTYPE = "unsigned char";
    // re2c:define:YYCURSOR = data;
    re2c:yyfill:enable = 0;
    re2c:flags:tags = 1;
    re2c:api:style = free-form;
    re2c:define:YYGETCONDITION = "cond";
    re2c:define:YYSETCONDITION = "cond = @@;";

    end = "\x00";
    nl = "\r"?"\n";
    ws = [ \t];
    keyword = [a-zA-Z_][a-zA-Z0-9_]*;
    string = "\"" ("\\\"" | [^"\r\n])* "\"";

    <init> *       { cs_log_error("Parser error on line %d", lineno); goto out; }
    <init> ws+     { goto yyc_init; }
    <init> [;#] .* { goto yyc_init; }
    <init> nl      { lineno++; goto yyc_init; }
    <init> "sp"    { kw_i = 0;  goto yyc_rule; }
    <init> end     { ret = SUCCESS; goto out; }
    <init> "set" ws+ @t1 keyword @t2 ws+ @t3 string @t4 ws* ";"? {
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
    <init> "@condition" ws+ { goto yyc_cond; }
    <init> "@end_condition" ws* ";" { cond_res[0] = 1; goto yyc_init; }
    <init> ( "@log" | "@info" ) ws+ @t1 string @t2 {
      TMPSTR(tmpstr, t2, t1);
      cs_log_info("[line %d]: %s", lineno, tmpstr);
      goto yyc_init;
    }
    <init> ( "@warn" | "@warning" ) ws+ @t1 string @t2 {
      TMPSTR(tmpstr, t2, t1);
      cs_log_warning("[line %d]: %s", lineno, tmpstr);
      goto yyc_init;
    }
    <init> ( "@err" | "@error" ) ws+ @t1 string @t2 {
      TMPSTR(tmpstr, t2, t1);
      cs_log_error("[line %d]: %s", lineno, tmpstr);
      goto out;
    }


    <cond> ws+ { goto yyc_cond; }
    <cond> nl  { lineno++; goto yyc_cond; }
    <cond> @t1 keyword @t2 "(" @t3 string? @t4 ")" {
      if (t4-t3 >= 2 && strlen("extension_loaded") == t2-t1 && strncmp("extension_loaded", t1, t2-t1) == 0) {
        int is_loaded = (zend_hash_str_find_ptr(&module_registry, t3+1, t4-t3-2) != NULL);
        sy_res_push(is_loaded);
      } else {
        cs_log_error("unknown function in condition on line %d", lineno);
        goto out;
      }
      goto yyc_cond_op;
    }
    <cond> @t1 keyword @t2 {
      zend_string *tmp = zend_hash_str_find_ptr(&vars, t1, t2-t1);
      if (!tmp) {
        cs_log_error("unknown variable in condition on line %d", lineno);
        goto out;
      }
      sy_res_push(atoi(ZSTR_VAL(tmp)));
      goto yyc_cond_op;
    }
    <cond> @t1 [0-9]+ @t2 { sy_res_push(atoi(t1));  goto yyc_cond_op; }
    <cond> @t1 "!" { sy_op_push(*t1); goto yyc_cond; }
    <cond> @t1 "(" { sy_op_push(*t1); goto yyc_cond; }
    <cond_op> ws+ { goto yyc_cond_op; }
    <cond_op> nl  { lineno++; goto yyc_cond_op; }
    <cond_op> @t1 ( "&&" | "||" | "<" | ">" | "==" | "<=" | ">=") @t2 {
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
    <cond_op> ")" {
      while (cond_op_i && sy_op_peek() != '(') {
        SY_APPLY_OP_FROM_STACK();
      }
      if (cond_op_i == 0 || sy_op_peek() != '(') {
        cs_log_error("unbalanced parathesis on line %d", lineno); goto out;
      }
      cond_op_i--;
      goto yyc_cond_op;
    }
    <cond_op> ";" {
      while (cond_op_i) {
        if (sy_op_peek() == '(') { cs_log_error("unbalanced parathesis on line %d", lineno); goto out; }
        SY_APPLY_OP_FROM_STACK();
      }
      if (cond_res_i > 1) { cs_log_error("invalid condition on line %d", lineno); goto out; }
      goto yyc_init;
    }
    <cond, cond_op> * { cs_log_error("Syntax error in condition on line %d", lineno); goto out; }

    <rule> ws+     {  goto yyc_rule; }
    <rule> nl / ( nl | ws )* "." {  lineno++; goto yyc_rule; }
    <rule> "." @t1 keyword @t2 ( "(" @t3 ( string? | keyword ) @t4 ")" )?  {
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