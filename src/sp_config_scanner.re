#include "php_snuffleupagus.h"

/*!types:re2c*/

#define cs_error_log(fmt, ...) sp_log_err("config", fmt, ##__VA_ARGS__)

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

zend_result sp_config_scan(char *data, zend_result (*process_rule)(sp_parsed_keyword*))
{
  const char *YYCURSOR = data;
  const char *YYMARKER, *t1, *t2, *t3, *t4;
  /*!stags:re2c format = 'const char *@@;\n'; */

  const int max_keywords = 16;
  sp_parsed_keyword parsed_rule[max_keywords+1];
  int kw_i = 0;

  int cond = yycinit;
  long lineno = 1;

  /*!re2c
    re2c:define:YYCTYPE = char;
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

    <init> *       { cs_error_log("Parser error on line %d", lineno); return FAILURE; }
    <init> ws+     { goto yyc_init; }
    <init> [;#] .* { goto yyc_init; }
    <init> nl      { lineno++; goto yyc_init; }
    <init> "sp"    { kw_i = 0;  goto yyc_rule; }
    <init> end     { return SUCCESS; }

    <rule> ws+     {  goto yyc_rule; }
    <rule> nl / ( nl | ws )* "." {  lineno++; goto yyc_rule; }
    <rule> "." @t1 keyword @t2 ( "(" @t3 string? @t4 ")" )?  {
      if (kw_i == max_keywords) {
        cs_error_log("Too many keywords in rule (more than %d) on line %d", max_keywords, lineno);
        return FAILURE;
      }
      sp_parsed_keyword kw = {.kw = (char*)t1, .kwlen = t2-t1, .arg = (char*)t3, .arglen = t4-t3, .argtype = SP_ARGTYPE_UNKNOWN, .lineno = lineno};
      if (t3 && t4) {
        if (t3 == t4) {
          kw.argtype = SP_ARGTYPE_EMPTY;
        } else if (t4-t2 >= 2) {
          kw.arg = (char*)t3 + 1;
          kw.arglen = t4 - t3 - 2;
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
      parsed_rule[kw_i++] = (sp_parsed_keyword){0, 0, 0, 0, 0, 0};
      if (process_rule && process_rule(parsed_rule) != SUCCESS) {
        return FAILURE;
      }
      goto yyc_init;
    }
    <rule> *       { goto end_of_rule; }

  */
  return FAILURE;
}