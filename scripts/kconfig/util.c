#include <string.h>
#include "lkc.h"

/* 文件已存在于列表中？如果不加 */
struct file *file_lookup(const char *name)
{
	struct file *file;
	const char *file_name = sym_expand_string_value(name);

	for (file = file_list; file; file = file->next) {
		if (!strcmp(name, file->name)) {
			free((void *)file_name);
			return file;
		}
	}

	file = malloc(sizeof(*file));
	memset(file, 0, sizeof(*file));
	file->name = file_name;
	file->next = file_list;
	file_list = file;

	return file;
}

/* 按kbuild所用编写依赖项文件以跟踪依赖项 */
int file_write_dep(const char *name)
{
	FILE *out;
	struct expr *e;
	struct file *file;
	struct symbol *sym, *env_sym;
	
	if (!name) {
		name = ".kconfig.d";
	}

	out = fopen("..config.tmp", "w");
	if (!out) {
		return 1;
	}

	fprintf(out, "deps_config := \\\n");
	for (file = file_list; file; file = file->next) {
		if (file->next) {
			fprintf(out, "\t%s \\\n", file->name);
		} else {
			fprintf(out, "\t%s\n", file->name);
		}
	}
	fprintf(out, "\n%s: \\\n\t$(deps_config)\n\n", conf_get_autoconfig_name());

	expr_list_for_each_sym(sym_env_list, e, sym) {
		const char *value;
		struct property *prop;

		prop = sym_get_env_prop(sym);
		env_sym = prop_get_symbol(prop);
		if (!env_sym) {
			continue;
		}

		value = getenv(env_sym->name);
		if (!value) {
			value = "";
		}

		fprintf(out, "ifneq \"$(%s)\" \"%s\"\n", env_sym->name, value);
		fprintf(out, "%s: FORCE\n", conf_get_autoconfig_name());
		fprintf(out, "endif\n");
	}

	fprintf(out, "\n$(deps_config): ;\n");
	fclose(out);
	rename("..config.tmp", name);

	return 0;
}


/* 分配初始可增长字符串 */
struct gstr str_new(void)
{
	struct gstr gs;

	gs.s = malloc(sizeof(char) * 64);
	gs.len = 64;
	gs.max_width = 0;
	strcpy(gs.s, "\0");

	return gs;
}

/* 分配和分配可增长字符串 */
struct gstr str_assign(const char *s)
{
	struct gstr gs;

	gs.s = strdup(s);
	gs.len = strlen(s) + 1;
	gs.max_width = 0;

	return gs;
}

/* 释放可增长字符串存储 */
void str_free(struct gstr *gs)
{
	if (gs->s) {
		free(gs->s);
	}

	gs->s = NULL;
	gs->len = 0;
}

/* 附加到可增长字符串 */
void str_append(struct gstr *gs, const char *s)
{
	size_t l;

	if (s) {
		l = strlen(gs->s) + strlen(s) + 1;
		if (l > gs->len) {
			gs->s   = realloc(gs->s, l);
			gs->len = l;
		}
		strcat(gs->s, s);
	}
}

/* 将printf格式的字符串追加到可增长字符串 */
void str_printf(struct gstr *gs, const char *fmt, ...)
{
	va_list ap;
	char s[10000];

	va_start(ap, fmt);
	vsnprintf(s, sizeof(s), fmt, ap);
	str_append(gs, s);
	va_end(ap);
}

/* 检索可增长字符串的值 */
const char *str_get(struct gstr *gs)
{
	return gs->s;
}
