#include "argo.h"

static int g_error = 0;
static json parse_json(FILE *stream);

static char *parse_raw_string(FILE *stream) {
	char *str = NULL;
	int len = 0;
	char c;

	if (!expect(stream, '"')) {
		g_error = 1;
		return (NULL);
	}
	while (peek(stream) != '"' && peek(stream) != EOF) {
		c = getc(stream);
		if (c == '\\') {
			char next_c = peek(stream);
			if (next_c == '"' || next_c == '\\') {
				c = getc(stream);
			} else {
				g_error = 1;
				free(str);
				return (NULL);
			}
		}
		len++;
		str = realloc(str, len + 1);
		str[len - 1] = c;
		str[len] = '\0';
	}
	if (!expect(stream, '"')) {
		g_error = 1;
		free(str);
		return (NULL);
	}
	if (str == NULL) {
		str = malloc(1);
		str[0] = '\0';
	}
	return (str);
}

static json parse_map(FILE *stream) {
	json j = {.type = MAP, .map.data = NULL, .map.size = 0};
	pair p = {.key = NULL};

	if (!expect(stream, '{')) {
		g_error = 1;
		goto cleanup;
	}
	if (peek(stream) != '}') {
		do {
			p.key = parse_raw_string(stream);
			if (g_error)
				goto cleanup;
			if (!expect(stream, ':')) {
				g_error = 1;
				goto cleanup;
			}
			p.value = parse_json(stream);
			if (g_error)
				goto cleanup;
			j.map.size++;
			j.map.data = realloc(j.map.data, j.map.size * sizeof(pair));
			j.map.data[j.map.size - 1] = p;
			p.key = NULL;
		} while (accept(stream, ','));
	}
	if (!expect(stream, '}')) {
		g_error = 1;
		goto cleanup;
	}
	return (j);

cleanup:
	free(p.key);
	free_json(j);
	memset(&j, 0, sizeof(json));
	return (j);
}

static json parse_json(FILE *stream) {
	json j;
	memset(&j, 0, sizeof(json));
	int c = peek(stream);

	if (c == '"') {
		j.type = STRING;
		j.string = parse_raw_string(stream);
	} else if (c == '{') {
		return (parse_map(stream));
	} else if (isdigit(c) || c == '-') {
		j.type = INTEGER;
		if (fscanf(stream, "%d", &j.integer) != 1)
			g_error = 1;
	} else {
		g_error = 1;
	}
	return (j);
}

int		argo(json *dst, FILE *stream) {
	g_error = 0;
	*dst = parse_json(stream);

	if (g_error || peek(stream) != EOF) {
		unexpected(stream);
		return (-1);
	}
	return (1);
}
