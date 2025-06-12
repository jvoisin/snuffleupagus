#include "php_snuffleupagus.h"

static sp_list_node *parse_str_tokens(const char *str,
                                      const sp_conf_token token,
                                      sp_list_node *tokens_list) {
  const char *cur_str = str;

  while ((cur_str = strchr(cur_str, token.text_repr[0]))) {
    if (0 == strncmp(cur_str, token.text_repr, strlen(token.text_repr))) {
      sp_conf_token *token_elm = pecalloc(1, sizeof(sp_conf_token), 1);
      token_elm->pos = cur_str - str;
      token_elm->text_repr = token.text_repr;
      token_elm->type = token.type;
      tokens_list = sp_list_insert(tokens_list, token_elm);
      cur_str += strlen(token.text_repr);
    } else {
      cur_str++;
    }
  }
  return tokens_list;
}

static bool is_var_name_valid(const char *const name) {

  assert(name);

  if (NULL == SPG(global_regexp_var) || NULL == SPG(global_regexp_const)) {
    SPG(global_regexp_var) = sp_pcre_compile(REGEXP_VAR);
    SPG(global_regexp_const) = sp_pcre_compile(REGEXP_CONST);
  }
  if (NULL == SPG(global_regexp_var) || NULL == SPG(global_regexp_const)) {
    return false;  // LCOV_EXCL_LINE
  }
  if ((false == sp_is_regexp_matching_len(SPG(global_regexp_var), VAR_AND_LEN(name))) &&
      (false == sp_is_regexp_matching_len(SPG(global_regexp_const), VAR_AND_LEN(name)))) {
    return false;
  }
  return true;
}

static int create_var(sp_tree *tree, const char *restrict value,
                      size_t value_len, elem_type _type,
                      const char *restrict idx) {
  sp_tree *var_node = NULL;
  bool free_node_on_error = false;
  int err = 0;
  assert(tree);

  if (tree->next == NULL && tree->type == UNDEFINED) {
    var_node = tree;
  } else {
    var_node = pecalloc(1, sizeof(sp_tree), 1);
    free_node_on_error = true;
  }

  var_node->value = NULL;
  var_node->next = NULL;
  var_node->idx = NULL;
  var_node->type = _type;
  /* We consider `$` as part of the variable name, to tell them apart from
   * php's constant
   */
  if (value && value[0] == VARIABLE_TOKEN && _type == CONSTANT) {
    var_node->type = VAR;
  }
  if (!(var_node->value = pestrndup(value, value_len, 1))) {
    // LCOV_EXCL_START
    sp_log_err("config", "Can't allocate a strndup");
    err = -1; goto err;
    // LCOV_EXCL_STOP
  }
  if (var_node->type != INTERPRETED_STRING &&
      !is_var_name_valid(var_node->value)) {
    sp_log_err("config", "Invalid var name: %s.", var_node->value);
    err = -1; goto err;
  }

  var_node->idx = sp_parse_var(idx);

  if (tree != var_node) {
    while (tree->next) {
      tree = tree->next;
    }
    tree->next = var_node;
  }

  if (err) {
err:
    if (free_node_on_error) {
      sp_tree_free(var_node);
    } else {
      var_node->next = var_node->idx = NULL;
      var_node->value = NULL;
      var_node->type = UNDEFINED;
    }
  }
  
  return err;
}

static int cmp_tokens(sp_list_node const *const list1,
               sp_list_node const *const list2) {
  return (((sp_conf_token *)list1->data)->pos -
          ((sp_conf_token *)list2->data)->pos);
}

static int is_next_token_empty(sp_conf_token *token, sp_conf_token *token_next,
                               const char *restrict str) {
  if ((token_next &&
       token_next->pos == token->pos + strlen(token->text_repr)) ||
      (!token_next && token->pos == strlen(str) - strlen(token->text_repr))) {
    return -1;
  }
  return 0;
}

static int is_token_valid(sp_list_node *tokens_list, elem_type quote,
                          int array_count, const char *restrict str,
                          size_t pos) {
  sp_conf_token *token = (sp_conf_token *)tokens_list->data;
  sp_conf_token *token_next = NULL;

  if (tokens_list->next) {
    token_next = (sp_conf_token *)tokens_list->next->data;
  }
  switch (token->type) {
    case LITERAL_STRING:
    case INTERPRETED_STRING:
      if (quote == token->type) {
        if (token_next) {
          if (token_next->pos != token->pos + 1) {
            return -1;
          }
        } else if (token->pos != strlen(str) - 1) {
          return -1;
        }
      }
      break;
    case ARRAY_END:
      if (UNDEFINED == quote) {
        if (array_count < 1) {
          return -1;
        } else if (token_next) {
          if (token_next->type == INTERPRETED_STRING ||
              token_next->type == LITERAL_STRING) {
            return -1;
          }
        } else if (token->pos != strlen(str) - strlen(token->text_repr)) {
          return -1;
        }
      }
      break;
    case OBJECT:
      if (UNDEFINED == quote &&
          -1 == is_next_token_empty(token, token_next, str)) {
        return -1;
      }
      if (pos == 0 && *str != VARIABLE_TOKEN) {
        return -1;
      }
      break;
    case CLASS:
      if (UNDEFINED == quote &&
          -1 == is_next_token_empty(token, token_next, str)) {
        return -1;
      }
      break;
    default:
      break;
  }
  return 0;
}

static sp_tree *parse_tokens(const char *restrict str,
                             sp_list_node *tokens_list) {
  size_t pos = 0;
  int array_count = 0, pos_idx_start = -1;
  elem_type quote = UNDEFINED;
  sp_tree *tree = sp_tree_new();

  for (; tokens_list && tokens_list->data; tokens_list = tokens_list->next) {
    sp_conf_token *token = (sp_conf_token *)tokens_list->data;
    size_t value_len;
    char *idx = NULL;

    if (-1 == is_token_valid(tokens_list, quote, array_count, str, pos)) {
      sp_log_err("config", "Invalid `%s` position.", token->text_repr);
      goto error;
    }
    if (token->type == INTERPRETED_STRING || token->type == LITERAL_STRING) {
      pos = (UNDEFINED == quote && !array_count)
                ? pos + strlen(token->text_repr)
                : pos;
      quote = (UNDEFINED == quote)     ? token->type
              : (quote == token->type) ? 0
                                       : quote;
      token->type = INTERPRETED_STRING;
    }
    if (UNDEFINED == quote) {
      if (token->type == ARRAY) {
        pos_idx_start = (array_count)
                            ? pos_idx_start
                            : (int)(token->pos + strlen(token->text_repr));
        array_count++;
      } else if (token->type == ARRAY_END) {
        array_count--;
        token->type = ARRAY;
      }
      if (array_count == 0) {
        value_len = token->pos - pos;
        if (token->type == ARRAY) {
          value_len -= strlen(token->text_repr);
        }
        if (pos_idx_start > 0) {
          idx = estrndup(&(str[pos_idx_start]), token->pos - pos_idx_start);
          value_len -= token->pos - pos_idx_start;
        }
        if (create_var(tree, &str[pos], value_len, token->type, idx)) {
          goto error;
        }
        efree(idx);
        pos = token->pos + strlen(token->text_repr);
        pos_idx_start = -1;
      }
    }
  }

  if (array_count != 0) {
    sp_log_err("config", "You forgot to close a bracket.");
    goto error;
  }
  if (quote != UNDEFINED) {
    sp_log_err("config", "Missing a closing quote.");
  error:
    sp_tree_free(tree);
    return NULL;
  }
  if (pos != strlen(str) &&
      create_var(tree, &str[pos], strlen(str) - pos, CONSTANT, NULL)) {
    goto error;
  }
  return tree;
}

sp_tree *sp_parse_var(const char *line) {
  static const sp_conf_token delimiter_list[] = {
      {.type = OBJECT, .text_repr = OBJECT_TOKEN},
      {.type = ARRAY, .text_repr = ARRAY_TOKEN},
      {.type = ARRAY_END, .text_repr = ARRAY_END_TOKEN},
      {.type = INTERPRETED_STRING, .text_repr = STRING_TOKEN},
      {.type = LITERAL_STRING, .text_repr = ESC_STRING_TOKEN},
      {.type = CLASS, .text_repr = CLASS_TOKEN}};
  sp_list_node *tokens_list = NULL;
  sp_tree *tree = NULL;

  if (!line) {
    return NULL;
  }
  for (unsigned int i = 0; i < sizeof(delimiter_list) / sizeof(sp_conf_token);
       i++) {
    tokens_list = parse_str_tokens(line, delimiter_list[i], tokens_list);
  }
  tokens_list = sp_list_sort(tokens_list, cmp_tokens);
  tree = parse_tokens(line, tokens_list);
  sp_list_free2(tokens_list);
  // Check if tree is empty.
  if (tree && tree->next == NULL && tree->type == UNDEFINED) {
    tree->type = CONSTANT;
    tree->value = pestrdup("", 1);
  }
  return tree;
}
