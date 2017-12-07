#include "php_snuffleupagus.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const sp_token_t jrr_token[] = {
  {.type=object, .token=OBJECT_TOKEN},
  {.type=array, .token=ARRAY_TOKEN},
  {.type=array_end, .token=ARRAY_END_TOKEN},
  {.type=string_delimiter, .token=STRING_TOKEN},
  {.type=esc_string_delimiter, .token=ESC_STRING_TOKEN},
  /* Namespace seems to be useless. */
  /*{.type=namespace, .token=NAMESPACE_TOKEN},*/
  {.type=class, .token=CLASS_TOKEN},
  {.type=0, .token=NULL}
};

/*TODO: rename a lot of vars*/
/*TODO: maybe a better algorythm ?*/
static void sp_list_sort(sp_node_t *list) {
  sp_node_t *head = list;
  int change;
  void *tmp;

  do {
    change = 0;
    list = head;
    while (list) {
      if (list->next && list->next->data
	  && 0 < ((int)((sp_token_t *)list->data)->pos -
		  (int)((sp_token_t *)list->next->data)->pos)) {
	  change = 1;
	  tmp = list->next->data;
	  list->next->data = list->data;
	  list->data = tmp;
	}
      list = list->next;
    }
  } while (change);
  for (list = head; list; list = list->next) {
    list->head = head;
  }
}


static int get_all_object(const char *str, const sp_token_t token,
			  sp_node_t *tokens_list) {
  const char *cur_str = str;

  while ((cur_str = strchr(cur_str, token.token[0]))) {
    if (0 == strncmp(cur_str, token.token, strlen(token.token))) {
      sp_token_t *token_elm = malloc(sizeof(sp_token_t));
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

static void create_var(arbre_du_ghetto *sapin, const char *value,
		       int value_len, elem_type _type, char *idx) {
  arbre_du_ghetto *var_node = NULL;

  if (!sapin) {
    return ;
  }
  if (sapin->next == NULL && sapin->type == 0) {
    var_node = sapin;
  } else {
    var_node = malloc(sizeof(arbre_du_ghetto));
  }
  _type = (_type == string_delimiter) ? litteral : _type;
  var_node->value = NULL;
  var_node->next = NULL;
  var_node->idx = NULL;
  var_node->type = _type;
  /* Check if there is a var token. */
  if (value && value[0] == VARIABLE_TOKEN) {
    var_node->type = (_type == litteral) ? var : _type;
    var_node->value = strndup(&value[1], value_len - 1);
  }
  if (!var_node->value) {
    var_node->value = strndup(value, value_len);
  }
  var_node->idx = parse_var(idx);
  if (sapin != var_node) {
    while (sapin->next) {
      sapin = sapin->next;
    }
    sapin->next = var_node;
  }
}

static void free_sp_token_list(sp_node_t *tokens_list) {
  sp_node_t *token_node = tokens_list;

  for (; token_node; token_node = token_node->next) {
    sp_token_t *tmp = (sp_token_t *)token_node->data;
    free(tmp);
  }
  sp_list_free(tokens_list);
}

void free_arbre_du_ghetto(arbre_du_ghetto *sapin) {
  while (sapin) {
    arbre_du_ghetto *tmp;
    free(sapin->value);
    free_arbre_du_ghetto(sapin->idx);
    tmp = sapin;
    sapin = sapin->next;
    free(tmp);
  }
}

static arbre_du_ghetto *arbre_du_ghetto_new() {
  arbre_du_ghetto *new = malloc(sizeof(arbre_du_ghetto));
  new->next = new->idx = NULL;
  new->value = NULL;
  new->type = 0;
  return new;
}

static int check_empty_next_token(sp_token_t *token, sp_token_t *token_next,
			    const char *str) {
  if ((token_next && token_next->pos == token->pos + strlen(token->token))
      || (!token_next && token->pos == strlen(str) - strlen(token->token))) {
    return -1;
  }
  return 0;
}

static int check_error_token(sp_node_t *tokens_list, elem_type ignore,
			     int array_count, const char *str) {
  sp_token_t *token = (sp_token_t *)tokens_list->data;
  sp_token_t *token_next = NULL;

  if (tokens_list->next) {
    token_next = (sp_token_t *)tokens_list->next->data;
  }
  switch (token->type) {
    case esc_string_delimiter:
      if (ignore == esc_string_delimiter) {
	if (token_next && token_next->pos != token->pos + 1) {
	  return -1;
	} else if (!token_next && token->pos != strlen(str) - 1) {
	  return -1;
	}
      }
      break;
    case string_delimiter:
      if (ignore == string_delimiter) {
	if (token_next && token_next->pos != token->pos + 1) {
	  return -1;
	} else if (!token_next && token->pos != strlen(str) - 1) {
	  return -1;
	}
      }
      break;
    case array_end:
      if (!ignore) {
	if (array_count < 1) {
	    return -1;
	} else if (!token_next && token->pos != strlen(str) - strlen(token->token)) {
	    return -1;
	} else if (token_next) {
	  if (token_next->type == string_delimiter
	      || token_next->type == esc_string_delimiter) {
	    return -1;
	  }
	}
      }
      break;
    case object:
      if (!ignore && -1 == check_empty_next_token(token, token_next, str)) {
	return -1;
      }
      break;
    case class:
      if (!ignore && -1 == check_empty_next_token(token, token_next, str)) {
	return -1;
      }
      break;
    default:
      break;
  }
  return 0;
}

static arbre_du_ghetto *parse_tokens(const char *str, sp_node_t *tokens_list) {
  int pos = 0, array_count = 0, pos_idx_start = -1;
  elem_type ignore = 0;
  arbre_du_ghetto *sapin = arbre_du_ghetto_new();

  for (; tokens_list && tokens_list->data; tokens_list = tokens_list->next) {
    sp_token_t *token = (sp_token_t *)tokens_list->data;
    int value_len;
    char *idx = NULL;

    if (-1 == check_error_token(tokens_list, ignore, array_count, str)) {
      goto error;
    }
    if (token->type == string_delimiter || token->type == esc_string_delimiter) {
      pos = (!ignore && !array_count) ? pos + (int)strlen(token->token) : pos;
      ignore = (!ignore) ? token->type : (ignore == token->type) ? 0 : ignore;
    }
    if (ignore == 0) {
      if (token->type == array) {
	pos_idx_start = (array_count) ? pos_idx_start :
	  (int)(token->pos + strlen(token->token));
	array_count++;
      } else if (token->type == array_end) {
	array_count--;
	token->type = array;
      }
      if (array_count == 0) {
	value_len = token->pos - pos;
	if (token->type == array) {
	  value_len -= strlen(token->token);
	}
	if (pos_idx_start > 0) {
	  idx = strndup(&str[pos_idx_start], token->pos - pos_idx_start);
	  strncpy(idx, &str[pos_idx_start], token->pos - pos_idx_start);
	  idx[token->pos - pos_idx_start] = 0;
	  value_len -= token->pos - pos_idx_start;
	}
	create_var(sapin, &str[pos], value_len, token->type, idx);
	free(idx);
	pos = token->pos + strlen(token->token);
	pos_idx_start = -1;
      }
    }
  }

  if (ignore != 0 || array_count != 0) {
error:
    free_arbre_du_ghetto(sapin);
    return NULL;
  }
  if (pos != (int)strlen(str)) {
    create_var(sapin, &str[pos], strlen(str) - pos, litteral, NULL);
  }
  return sapin;
}

arbre_du_ghetto *parse_var(const char *line) {
  sp_node_t *tokens_list = NULL;
  arbre_du_ghetto *sapin = NULL;

  if (!line) {
    return NULL;
  }
  tokens_list = sp_list_new();
  for (int i = 0; jrr_token[i].token; i++) {
    get_all_object(line, jrr_token[i], tokens_list);
  }
  sp_list_sort(tokens_list);
  sapin = parse_tokens(line, tokens_list);
  free_sp_token_list(tokens_list);
  return sapin;
}
