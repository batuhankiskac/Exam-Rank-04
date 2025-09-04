#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct node {
	enum {
		ADD,
		MULTI,
		VAL
	}   type;
	int val;
	struct node *l;
	struct node *r;
}   node;

node	*parse_expr(char **s);

node    *new_node(node n)
{
	node *ret = calloc(1, sizeof(node));
	if (!ret)
		return (NULL);
	*ret = n;
	return (ret);
}

void    destroy_tree(node *n)
{
	if (!n)
		return ;
	if (n->type != VAL)
	{
		destroy_tree(n->l);
		destroy_tree(n->r);
	}
	free(n);
}

void    unexpected(char c)
{
	if (c)
		printf("Unexpected token '%c'\n", c);
	else
		printf("Unexpected end of file\n");
}

int accept(char **s, char c)
{
	if (**s == c)
	{
		(*s)++;
		return (1);
	}
	return (0);
}

int expect(char **s, char c)
{
	if (accept(s, c))
		return (1);
	unexpected(**s);
	return (0);
}

static node *parse_factor(char **s) {
	if (accept(s, '(')) {
		node *expr_node = parse_expr(s);
		if (!expect(s, ')')) {
			destroy_tree(expr_node);
			return (NULL);
		}
		return (expr_node);
	} else if (isdigit(**s)) {
		node n = {.type = VAL, .val = **s - '0', .l = NULL, .r = NULL};
		(*s)++;
		return (new_node(n));
	} else {
		unexpected(**s);
		return (NULL);
	}
}

static node *parse_term(char **s) {
	node *left = parse_factor(s);
	if (!left)
		return (NULL);
	while (accept(s, '*')) {
		node *right = parse_factor(s);
		if (!right) {
			destroy_tree(left);
			return (NULL);
		}
		node n = {.type = MULTI, .l = left, .r = right};
		left = new_node(n);
	}
	return (left);
}

node    *parse_expr(char **s) {
	node *left = parse_term(s);
	if (!left)
		return (NULL);
	while (accept(s, '+')) {
		node *right = parse_term(s);
		if (!right) {
			destroy_tree(left);
			return (NULL);
		}
		node n = {.type = ADD, .l = left, .r = right};
		left = new_node(n);
	}
	return (left);
}

int eval_tree(node *tree)
{
	switch (tree->type)
	{
		case ADD:
			return (eval_tree(tree->l) + eval_tree(tree->r));
		case MULTI:
			return (eval_tree(tree->l) * eval_tree(tree->r));
		case VAL:
			return (tree->val);
	}
	return (0);
}

int main(int argc, char **argv)
{
	if (argc != 2)
		return (1);

	char *input = argv[1];
	node *tree = parse_expr(&input);
	if (!tree || *input != '\0') {
		if (tree == NULL && *input != '\0')
			unexpected(*input);
		else if (tree != NULL && *input != '\0')
			unexpected(*input);
		if (tree)
			destroy_tree(tree);
		return (1);
	}
	printf("%d\n", eval_tree(tree));
	destroy_tree(tree);
	return (0);
}
