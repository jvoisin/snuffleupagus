#include "php_snuffleupagus.h"

static int parse_str_tokens(const char *str, const sp_token_t token,
			    sp_node_t *tokens_list) {
  const char *cur_str = str;

  while ((cur_str = strchr(cur_str, token.token[0]))) {
    if (0 == strncmp(cur_str, token.token, strlen(token.token))) {
      sp_token_t *token_elm = pecalloc(sizeof(sp_token_t), 1, 1);
      token_elm->pos = cur_str - str;
      token_elm->token = token.token;
      token_elm->type = token.type;
      sp_list_insert(tokens_list, token_elm);
      cur_str += strlen(token.token);
    } else {
      cur_str++;
    }
  }
  return 0;
}

static bool is_var_name_valid(const char *name) {
  static pcre *regexp_const = NULL;
  static pcre *regexp_var = NULL;
  const char *pcre_error;
  int pcre_error_offset;

  if (!name) {
    return false;
  }
  if (NULL == regexp_var || NULL == regexp_const) {
    regexp_var = sp_pcre_compile(REGEXP_VAR, PCRE_CASELESS, &pcre_error,
				 &pcre_error_offset, NULL);
    regexp_const = sp_pcre_compile(REGEXP_CONST, PCRE_CASELESS, &pcre_error,
				   &pcre_error_offset, NULL);
  }
  if (NULL == regexp_var || NULL == regexp_const) {
    sp_log_err("config", "Could not compile regexp.");
    return false;
  }
  if (0 > sp_pcre_exec(regexp_var, NULL, name, strlen(name), 0, 0, NULL, 0)
      && 0 > sp_pcre_exec(regexp_const, NULL, name, strlen(name), 0, 0, NULL, 0)) {
    return false;
  }
  return true;
}

static int create_var(sp_tree *tree, const char *restrict value,
		      size_t value_len, elem_type _type, const char *restrict idx) {
  sp_tree *var_node = NULL;

  if (!tree) {
    return -1;
  }
  if (tree->next == NULL && tree->type == UNDEFINED) {
    var_node = tree;
  } else {
    var_node = pecalloc(sizeof(sp_tree), 1, 1);
  }

  var_node->value = NULL;
  var_node->next = NULL;
  var_node->idx = NULL;
  var_node->type = _type;
  /* We consider `$` as part of the variable name, to tell them appart from
   * php's constant
   */
  if (value && value[0] == VARIABLE_TOKEN && _type == CONSTANT) {
    var_node->type = VAR;
  }
  if (!(var_node->value = pestrndup(value, value_len, 1))) {
    sp_log_err("config", "Can't allocate a strndup");
    return -1;
  }
  if (var_node->type != STRING_DELIMITER && !is_var_name_valid(var_node->value)) {
    sp_log_err("config", "Invalid var name: %s.", var_node->value);
    return -1;
  }
  var_node->idx = parse_var(idx);

  if (tree != var_node) {
    while (tree->next) {
      tree = tree->next;
    }
    tree->next = var_node;
  }
  return 0;
}

int cmp_tokens(sp_node_t *list1, sp_node_t *list2) {
  return (((sp_token_t *)list1->data)->pos
	  - ((sp_token_t *)list2->data)->pos);
}

static int is_next_token_empty(sp_token_t *token, sp_token_t *token_next,
			       const char * restrict str) {
  if ((token_next && token_next->pos == token->pos + strlen(token->token))
      || (!token_next && token->pos == strlen(str) - strlen(token->token))) {
    return -1;
  }
  return 0;
}

static int is_token_valid(sp_node_t *tokens_list, elem_type ignore,
			  int array_count, const char * restrict str,
			  size_t pos) {
  sp_token_t *token = (sp_token_t *)tokens_list->data;
  sp_token_t *token_next = NULL;

  if (tokens_list->next) {
    token_next = (sp_token_t *)tokens_list->next->data;
  }
  switch (token->type) {
    case ESC_STRING_DELIMITER:
    case STRING_DELIMITER:
      if (ignore == token->type) {
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
      if (!ignore) {
	if (array_count < 1) {
	  return -1;
	} else if (token_next) {
	  if (token_next->type == STRING_DELIMITER
	      || token_next->type == ESC_STRING_DELIMITER) {
	    return -1;
	  }
	} else if (token->pos != strlen(str) - strlen(token->token)) {
	  return -1;
	}
      }
      break;
    case OBJECT:
      if (!ignore && -1 == is_next_token_empty(token, token_next, str)) {
	return -1;
      }
      if (pos == 0 && *str != VARIABLE_TOKEN) {
	return -1;
      }
      break;
    case CLASS:
      if (!ignore && -1 == is_next_token_empty(token, token_next, str)) {
	return -1;
      }
      break;
    default:
      break;
  }
  return 0;
}

static sp_tree *parse_tokens(const char * restrict str,
			     sp_node_t *tokens_list) {
  size_t pos = 0;
  int array_count = 0, pos_idx_start = -1;
  elem_type ignore = 0;
  sp_tree *tree = sp_tree_new();

  for (; tokens_list && tokens_list->data; tokens_list = tokens_list->next) {
    sp_token_t *token = (sp_token_t *)tokens_list->data;
    size_t value_len;
    char *idx = NULL;

    if (-1 == is_token_valid(tokens_list, ignore, array_count, str, pos)) {
      sp_log_err("config", "Invalid `%s` position.", token->token);
      goto error;
    }
    if (token->type == STRING_DELIMITER || token->type == ESC_STRING_DELIMITER) {
      pos = (!ignore && !array_count) ? pos + strlen(token->token) : pos;
      ignore = (!ignore) ? token->type : (ignore == token->type) ? 0 : ignore;
      token->type = STRING_DELIMITER;
    }
    if (ignore == 0) {
      if (token->type == ARRAY) {
        pos_idx_start = (array_count) ? pos_idx_start : (int)(token->pos + strlen(token->token));
        array_count++;
      } else if (token->type == ARRAY_END) {
        array_count--;
        token->type = ARRAY;
      }
      if (array_count == 0) {
        value_len = token->pos - pos;
        if (token->type == ARRAY) {
          value_len -= strlen(token->token);
        }
        if (pos_idx_start > 0) {
          idx = estrndup(&(str[pos_idx_start]), token->pos - pos_idx_start);
          value_len -= token->pos - pos_idx_start;
        }
        if (create_var(tree, &str[pos], value_len, token->type, idx)) {
          goto error;
        }
        efree(idx);
        pos = token->pos + strlen(token->token);
        pos_idx_start = -1;
      }
    }
  }

  if (ignore != 0 || array_count != 0) {
error:
    sp_tree_free(tree);
    return NULL;
  }
  if (pos != strlen(str)
      && create_var(tree, &str[pos], strlen(str) - pos, CONSTANT, NULL)) {
    goto error;
  }
  return tree;
}

sp_tree *parse_var(const char *line) {
  sp_node_t *tokens_list = NULL;
  sp_tree *tree = NULL;
  const sp_token_t delimiter_list[] = {
    {.type=OBJECT, .token=OBJECT_TOKEN},
    {.type=ARRAY, .token=ARRAY_TOKEN},
    {.type=ARRAY_END, .token=ARRAY_END_TOKEN},
    {.type=STRING_DELIMITER, .token=STRING_TOKEN},
    {.type=ESC_STRING_DELIMITER, .token=ESC_STRING_TOKEN},
    {.type=CLASS, .token=CLASS_TOKEN}
  };


  if (!line) {
    return NULL;
  }
  tokens_list = sp_list_new();
  for (unsigned int i = 0; i < sizeof(delimiter_list) / sizeof(sp_token_t); i++) {
    parse_str_tokens(line, delimiter_list[i], tokens_list);
  }
  tokens_list = sp_list_sort(tokens_list, cmp_tokens);
  tree = parse_tokens(line, tokens_list);
  sp_list_free(tokens_list);
  // Check if tree is empty.
  if (tree && tree->next == NULL && tree->type == UNDEFINED) {
    tree->type = CONSTANT;
    tree->value = pestrdup("", 1);
  }
  return tree;
}
