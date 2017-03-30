/**
 * @Filename: vty.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 03/29/2017 03:34:23 PM
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#include <sys/utsname.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <arpa/telnet.h>

#include "vty.h"
#include "buffer.h"
#include "command.h"

extern char *default_motd;
int g_vty_timeout_val = 5;

int vty_out(struct vty *vty, const char *format, ...)
{
	va_list args;
	int len, size;
	char buf[1024];
	char *p;

    size = 1024;
	va_start(args, format);
    p = NULL;
    len = vsnprintf(buf, size, format, args);
    while (len >= size) {
        size = size * 2;
        if (p) {
            free(p);
            p = NULL;
        }
        p = malloc(size);
        if (!p) {
            return -1;
        }
        len = vsnprintf(p, size, format, args);
    }

    if (NULL == p) {
        p = buf;
    }

    write(vty->fd, p, len);
    if (p != buf) {
        free(p);
        p = NULL;
    }
	va_end(args);

	return len;
}

#if 0
int vty_config_lock(struct vty *vty)
{
    if (vty_config == 0) { 
        vty->config = 1; 
        vty_config = 1; 
    }    

    return vty->config;
}

int vty_config_unlock(struct vty *vty)
{
    if (vty_config == 1 && vty->config == 1) { 
        vty->config = 0; 
        vty_config = 0; 
    }    

    return vty->config;
}
#endif

static int vty_write(struct vty *vty, char *buf, size_t nbytes)
{
    int recode;

    if ((vty->node == AUTH_NODE) || (vty->node == AUTH_ENABLE_NODE)) {
        return -1;
    }

    recode = buffer_write(vty->obuf, buf, nbytes);
    if (recode) {
        printf("buffer write failed\n");
        return -1;
    }

    return 0;
}

static int vty_ensure(struct vty *vty, int length)
{
    char *ptr;

    if (vty->max <= length) {
        vty->max *= 2;
        ptr = realloc(vty->buf, vty->max);
        if (NULL == ptr) {
            printf("%s:vty ensure realloc failed\n", __func__);
            return -1;
        }
        vty->buf = ptr;
    }    

    return 0;
}

void vty_hello(struct vty *vty)
{
    vty_out(vty, default_motd);
}

static void vty_prompt(struct vty *vty)
{
	///struct utsname names;

    ///uname(&names);
    vty_out(vty, "accelink>");
    ///vty_out(vty, "%s", names.nodename);

    return;
}

/* Send WILL TELOPT_ECHO to remote server. */
void vty_will_echo(struct vty *vty)
{
	char cmd[] = { IAC, WILL, TELOPT_ECHO, '\0' };
	vty_out(vty, "%s", cmd);
}

/* Make suppress Go-Ahead telnet option. */
static void vty_will_suppress_go_ahead(struct vty *vty)
{
	char cmd[] = { IAC, WILL, TELOPT_SGA, '\0' };
	vty_out(vty, "%s", cmd);
}

/* Make don't use linemode over telnet. */
static void vty_dont_linemode(struct vty *vty)
{
	char cmd[] = { IAC, DONT, TELOPT_LINEMODE, '\0' };
	vty_out(vty, "%s", cmd);
}

/* Use window size. */
static void vty_do_window_size(struct vty *vty)
{
	char cmd[] = { IAC, DO, TELOPT_NAWS, '\0' };
	vty_out(vty, "%s", cmd);
}

#if __TELNET_DEBUG__
static int vty_telnet_option(struct vty *vty, unsigned char *buf, int nbytes)
#else
static int vty_telnet_option(struct vty *vty, unsigned char *buf)
#endif
{
    char *buffer;
    int length;
#if __TELNET_DEBUG__
	int i;

	for (i = 0; i < nbytes; i++) {
		switch (buf[i]) {
		case IAC:
			vty_out(vty, "IAC ");
			break;
		case WILL:
			vty_out(vty, "WILL ");
			break;
		case WONT:
			vty_out(vty, "WONT ");
			break;
		case DO:
			vty_out(vty, "DO ");
			break;
		case DONT:
			vty_out(vty, "DONT ");
			break;
		case SB:
			vty_out(vty, "SB ");
			break;
		case SE:
			vty_out(vty, "SE ");
			break;
		case TELOPT_ECHO:
			vty_out(vty, "TELOPT_ECHO %s", VTY_NEWLINE);
			break;
		case TELOPT_SGA:
			vty_out(vty, "TELOPT_SGA %s", VTY_NEWLINE);
			break;
		case TELOPT_NAWS:
			vty_out(vty, "TELOPT_NAWS %s", VTY_NEWLINE);
			break;
		default:
			vty_out(vty, "%x ", buf[i]);
			break;
		}
	}
	vty_out(vty, "%s", VTY_NEWLINE);
#endif

	switch (buf[0]) {
	case SB:
		buffer_reset(vty->sb_buffer);
		vty->iac_sb_in_progress = 1;
		break;
	case SE:
        if (vty->iac_sb_in_progress) {
            buffer = (char *)vty->sb_buffer->head->data;
            length = vty->sb_buffer->length;
            if (buffer && (TELOPT_NAWS == buffer[0]) && (length > 4)) {
                vty->width = buffer[2];
                vty->height = vty->lines >= 0 ? vty->lines : buffer[4];
            }
            vty->iac_sb_in_progress = 0;
        }
        break;
	default:
		return 1;
	}

	return 0;
}

static void vty_down_level(struct vty *vty)
{
    vty_out(vty, "%s", VTY_NEWLINE);
    ///config_exit(NULL, vty, 0, NULL);
    vty_prompt(vty);
    vty->cp = 0; 

    return;
}

static void vty_redraw_line(struct vty *vty)
{
    vty_write(vty, vty->buf, vty->length);
    vty->cp = vty->length;

    return;
}

static void vty_kill_line(struct vty *vty)
{
	int i;
	int size;
    char ch;

	size = vty->length - vty->cp;

	if (size == 0)
		return;

    ch = ' ';
	for (i = 0; i < size; i++) {
		vty_write(vty, &ch, 1);
    }

    ch = 0x08;
	for (i = 0; i < size; i++) {
		vty_write(vty, &ch, 1);
    }

	memset(&vty->buf[vty->cp], 0, size);
	vty->length = vty->cp;
    
    return;
}

static int vty_self_insert(struct vty *vty, char c)
{
	int i, recode;
	int length;
    char ch;

	recode = vty_ensure(vty, vty->length + 1);
    if (recode) {
        return -1;
    }
	length = vty->length - vty->cp;
    if (length < 0) {
        printf("Invalid vty buf length\n");
        return -1;
    }
	memmove(&vty->buf[vty->cp + 1], &vty->buf[vty->cp], length);
	vty->buf[vty->cp] = c;

	recode = vty_write(vty, &vty->buf[vty->cp], length + 1);
    if (-1 == recode) {
        printf("%s:vty write buf failed\n", __func__);
        return -1;
    }

    ch = 0x08;
	for (i = 0; i < length; i++) {
		recode = vty_write(vty, &ch, 1);
        if (recode) {
            printf("%s:vty write back failed\n", __func__);
            return -1;
        }
    }
	vty->cp++;
	vty->length++;

    return 0;
}

static int vty_self_insert_overwrite(struct vty *vty, char c)
{
    int recode;

    recode = vty_ensure(vty, vty->length + 1);
    if (recode) {
        return -1;
    }
    vty->buf[vty->cp++] = c;
    if (vty->cp > vty->length) {
        vty->length++;
    }

    if ((vty->node == AUTH_NODE) || (vty->node == AUTH_ENABLE_NODE)) {
        return -1;
    }
    recode = vty_write(vty, &c, 1);
    if (recode) {
        printf("%s:vty write char failed\n", __func__);
        return -1;
    }

    return 0;
}

/*  Insert a word into vty interface with overwrite mode. */
///static
void vty_insert_word_overwrite(struct vty *vty, char *str)
{
    int len;
    
    len = strlen(str);
    vty_write(vty, str, len);
    strcpy(&vty->buf[vty->cp], str);
    vty->cp += len;
    vty->length = vty->cp;

    return;
}

static void vty_forward_char(struct vty *vty)
{
	if (vty->cp < vty->length) {
		vty_write(vty, &vty->buf[vty->cp], 1);
		vty->cp++;
	}

    return;
}

static int vty_backward_char(struct vty *vty)
{
    char ch;
    int recode;

    ch = 0x08;
	if (vty->cp > 0) {
		vty->cp--;
		recode = vty_write(vty, &ch, 1);
        if (recode) {
            return -1;
        }
	}

    return 0;
}

static int vty_beginning_of_line(struct vty *vty)
{
    int recode;

    while (vty->cp) {
        recode = vty_backward_char(vty);
        if (recode) {
            return -1;
        }
    }

    return 0;
}

static void vty_end_of_line(struct vty *vty)
{
    while (vty->cp < vty->length) {
        vty_forward_char(vty);
    }

    return;
}

static void vty_kill_line_from_beginning(struct vty *vty)
{
	vty_beginning_of_line(vty);
	vty_kill_line(vty);

    return;
}

static void vty_delete_char(struct vty *vty)
{
	int i;
	int size;
    char ch;

	if (vty->node == AUTH_NODE || vty->node == AUTH_ENABLE_NODE) {
		return;
    }

	if (vty->length == 0) {
		vty_down_level(vty);
		return;
	}

	if (vty->cp == vty->length) {
		return;
    }

	size = vty->length - vty->cp;

	vty->length--;
	memmove(&vty->buf[vty->cp], &vty->buf[vty->cp + 1], size - 1);
	vty->buf[vty->length] = '\0';

	vty_write(vty, &vty->buf[vty->cp], size - 1);
    ch = ' ';
	vty_write(vty, &ch, 1);

    ch = 0x08;
	for (i = 0; i < size; i++) {
		vty_write(vty, &ch, 1);
    }

    return;
}

/* Delete a character before the point. */
static void vty_delete_backward_char(struct vty *vty)
{
	if (vty->cp == 0)
		return;

	vty_backward_char(vty);
	vty_delete_char(vty);

    return;
}

static void vty_history_print(struct vty *vty)
{
	int length;

	vty_kill_line_from_beginning(vty);

	length = strlen(vty->hist[vty->hp]);
	memcpy(vty->buf, vty->hist[vty->hp], length);
	vty->cp = vty->length = length;

	vty_redraw_line(vty);

    return;
}

static void vty_next_line(struct vty *vty)
{
	int try_index;

	if (vty->hp == vty->hindex)
		return;

	try_index = vty->hp;
	if (try_index == (VTY_MAXHIST - 1))
		try_index = 0;
	else
		try_index++;

	if (vty->hist[try_index] == NULL)
		return;
	else
		vty->hp = try_index;

	vty_history_print(vty);

    return;
}

static void vty_previous_line(struct vty *vty)
{
	int try_index;

	try_index = vty->hp;
	if (try_index == 0)
		try_index = VTY_MAXHIST - 1;
	else
		try_index--;

	if (vty->hist[try_index] == NULL)
		return;
	else
		vty->hp = try_index;

	vty_history_print(vty);

    return;
}

static void vty_hist_add(struct vty *vty)
{
	int index;

	if (vty->length == 0) {
		return;
    }

	index = vty->hindex ? vty->hindex - 1 : VTY_MAXHIST - 1;
	if (vty->hist[index]) {
		if (strcmp(vty->buf, vty->hist[index]) == 0) {
			vty->hp = vty->hindex;
			return;
		}
    }

	if (vty->hist[vty->hindex]) {
		free(vty->hist[vty->hindex]);
        vty->hist[vty->hindex] = NULL;
    }
	vty->hist[vty->hindex] = strdup(vty->buf);

	vty->hindex++;
	if (vty->hindex == VTY_MAXHIST) {
		vty->hindex = 0;
    }
	vty->hp = vty->hindex;

    return ;
}

static int vty_execute(struct vty *vty)
{
	int recode;

	recode = 0;
	switch (vty->node) {
	case AUTH_NODE:
	case AUTH_ENABLE_NODE:
		///vty_auth(vty, vty->buf);
		break;
	default:
        ///ret = vty_command(vty, vty->buf);
        vty_hist_add(vty);
        break;
	}

	vty->cp = vty->length = 0;
	memset(vty->buf, 0, vty->max);
	if ((vty->status != VTY_CLOSE) && (vty->status != VTY_START)
            && (vty->status != VTY_CONTINUE)) {
		vty_prompt(vty);
    }

	return recode;
}

static void vty_escape_map(struct vty *vty, char ch)
{
	switch (ch) {
	case 'A':
		vty_previous_line(vty);
		break;
	case 'B':
		vty_next_line(vty);
		break;
	case 'C':
		vty_forward_char(vty);
		break;
	case 'D':
		vty_backward_char(vty);
		break;
	default:
		break;
	}
	vty->escape = VTY_NORMAL;

    return;
}

static void vty_forward_word(struct vty *vty)
{
	while (vty->cp != vty->length && vty->buf[vty->cp] != ' ') {
		vty_forward_char(vty);
    }

	while (vty->cp != vty->length && vty->buf[vty->cp] == ' ') {
		vty_forward_char(vty);
    }

    return;
}

static void vty_backward_word(struct vty *vty)
{
	while (vty->cp > 0 && vty->buf[vty->cp - 1] == ' ') {
		vty_backward_char(vty);
    }

	while (vty->cp > 0 && vty->buf[vty->cp - 1] != ' ') {
		vty_backward_char(vty);
    }

    return;
}

static void vty_forward_kill_word(struct vty *vty)
{
	while (vty->cp != vty->length && vty->buf[vty->cp] == ' ') {
		vty_delete_char(vty);
    }
	while (vty->cp != vty->length && vty->buf[vty->cp] != ' ') {
		vty_delete_char(vty);
    }

    return;
}

static void vty_backward_kill_word(struct vty *vty)
{
    while (vty->cp > 0 && vty->buf[vty->cp - 1] == ' ') {
        vty_delete_backward_char(vty);
    }
    while (vty->cp > 0 && vty->buf[vty->cp - 1] != ' ') {
        vty_delete_backward_char(vty);
    }

    return;
}

static int vty_transpose_chars(struct vty *vty)
{
    char c1, c2;

    /*  If length is short or point is near by the beginning of line then
     *         return. */
    if ((vty->length < 2) || (vty->cp < 1)) { 
        return 0;
    }

    /*  In case of point is located at the end of the line. */
    if (vty->cp == vty->length) {
        c1 = vty->buf[vty->cp - 1];
        c2 = vty->buf[vty->cp - 2];

        vty_backward_char(vty);
        vty_backward_char(vty);
        if (vty_self_insert_overwrite(vty, c1)) {
            return -1;
        }
        if (vty_self_insert_overwrite(vty, c2)) {
            return -1;
        }
    } else {
        c1 = vty->buf[vty->cp];
        c2 = vty->buf[vty->cp - 1];

        vty_backward_char(vty);
        if (vty_self_insert_overwrite(vty, c1)) {
            return -1;
        }
        if (vty_self_insert_overwrite(vty, c2)) {
            return -1;
        }
    }

    return 0;
}

static void vty_buffer_reset(struct vty *vty)
{
    buffer_reset(vty->obuf);
    vty_prompt(vty);
    vty_redraw_line(vty);

    return;
}

static void vty_end_config(struct vty *vty)
{
	vty_out(vty, "%s", VTY_NEWLINE);

	switch (vty->node) {
	case VIEW_NODE:
	case ENABLE_NODE:
		/* Nothing to do. */
		break;
	case CONFIG_NODE:
	case INTERFACE_NODE:
	case VTY_NODE:
		///vty_config_unlock(vty);
		vty->node = ENABLE_NODE;
		break;
	default:
		break;
	}
	vty_prompt(vty);
	vty->cp = 0;

    return;
}

static void vty_stop_input(struct vty *vty)
{
    vty->cp = vty->length = 0;
    memset(vty->buf, 0, vty->max);
    vty_out(vty, "%s", VTY_NEWLINE);
    switch (vty->node) {
        case VIEW_NODE:
        case ENABLE_NODE:
        case CONFIG_NODE:
        case INTERFACE_NODE:
        case VTY_NODE:
            ///vty_config_unlock(vty);
            vty->node = ENABLE_NODE;
            break;
        default:
            break;
    }

    vty_prompt(vty);
    vty->hp = vty->hindex;

    return;
}

static void vty_complete_command(struct vty *vty)
{
    if (NULL == vty) {
        return;
    }
#if 0
	int i;
	int ret;
	char **matched = NULL;
	vector vline;

	if (vty->node == AUTH_NODE || vty->node == AUTH_ENABLE_NODE) {
		return;
    }

	vline = cmd_make_strvec(vty->buf);
	if (vline == NULL) {
		return;
    }

	/* In case of 'help \t'. */
	if (isspace((int)vty->buf[vty->length - 1]))
		vector_set(vline, '\0');

	matched = cmd_complete_command(vline, vty, &ret);

	cmd_free_strvec(vline);

	vty_out(vty, "%s", VTY_NEWLINE);
	switch (ret) {
	case CMD_ERR_AMBIGUOUS:
		vty_out(vty, "%% Ambiguous command.%s", VTY_NEWLINE);
		vty_prompt(vty);
		vty_redraw_line(vty);
		break;
	case CMD_ERR_NO_MATCH:
		/* vty_out (vty, "%% There is no matched command.%s", VTY_NEWLINE); */
		vty_prompt(vty);
		vty_redraw_line(vty);
		break;
	case CMD_COMPLETE_FULL_MATCH:
		vty_prompt(vty);
		vty_redraw_line(vty);
		vty_backward_pure_word(vty);
		vty_insert_word_overwrite(vty, matched[0]);
		vty_self_insert(vty, ' ');
		XFREE(MTYPE_TMP, matched[0]);
		break;
	case CMD_COMPLETE_MATCH:
		vty_prompt(vty);
		vty_redraw_line(vty);
		vty_backward_pure_word(vty);
		vty_insert_word_overwrite(vty, matched[0]);
		XFREE(MTYPE_TMP, matched[0]);
		vector_only_index_free(matched);
		return;
		break;
	case CMD_COMPLETE_LIST_MATCH:
		for (i = 0; matched[i] != NULL; i++) {
			if (i != 0 && ((i % 6) == 0))
				vty_out(vty, "%s", VTY_NEWLINE);
			vty_out(vty, "%-10s ", matched[i]);
			XFREE(MTYPE_TMP, matched[i]);
		}
		vty_out(vty, "%s", VTY_NEWLINE);

		vty_prompt(vty);
		vty_redraw_line(vty);
		break;
	case CMD_ERR_NOTHING_TODO:
		vty_prompt(vty);
		vty_redraw_line(vty);
		break;
	default:
		break;
	}
	if (matched) {
		vector_only_index_free(matched);
    }
#endif

    return;
}

static void vty_describe_command(struct vty *vty)
{
    if (NULL == vty) {
        return;
    }
#if 0
	int ret;
	vector vline;
	vector describe;
	unsigned int i, width, desc_width;
	struct desc *desc, *desc_cr = NULL;

	vline = cmd_make_strvec(vty->buf);

	/* In case of '> ?'. */
	if (vline == NULL) {
		vline = vector_init(1);
		vector_set(vline, '\0');
	} else if (isspace((int)vty->buf[vty->length - 1]))
		vector_set(vline, '\0');

	describe = cmd_describe_command(vline, vty, &ret);

	vty_out(vty, "%s", VTY_NEWLINE);

	/* Ambiguous error. */
	switch (ret) {
	case CMD_ERR_AMBIGUOUS:
		cmd_free_strvec(vline);
		vty_out(vty, "%% Ambiguous command.%s", VTY_NEWLINE);
		vty_prompt(vty);
		vty_redraw_line(vty);
		return;
		break;
	case CMD_ERR_NO_MATCH:
		cmd_free_strvec(vline);
		vty_out(vty, "%% There is no matched command.%s", VTY_NEWLINE);
		vty_prompt(vty);
		vty_redraw_line(vty);
		return;
		break;
	}

	/* Get width of command string. */
	width = 0;
	for (i = 0; i < vector_max(describe); i++)
		if ((desc = vector_slot(describe, i)) != NULL) {
			unsigned int len;

			if (desc->cmd[0] == '\0')
				continue;

			len = strlen(desc->cmd);
			if (desc->cmd[0] == '.')
				len--;

			if (width < len)
				width = len;
		}

	/* Get width of description string. */
	desc_width = vty->width - (width + 6);

	/* Print out description. */
	for (i = 0; i < vector_max(describe); i++)
		if ((desc = vector_slot(describe, i)) != NULL) {
			if (desc->cmd[0] == '\0')
				continue;

			if (strcmp(desc->cmd, "<cr>") == 0) {
				desc_cr = desc;
				continue;
			}

			if (!desc->str)
				vty_out(vty, "  %-s%s",
					desc->cmd[0] ==
					'.' ? desc->cmd + 1 : desc->cmd,
					VTY_NEWLINE);
			else if (desc_width >= strlen(desc->str))
				vty_out(vty, "  %-*s  %s%s", width,
					desc->cmd[0] ==
					'.' ? desc->cmd + 1 : desc->cmd,
					desc->str, VTY_NEWLINE);
			else
				vty_describe_fold(vty, width, desc_width, desc);

#if 0
			vty_out(vty, "  %-*s %s%s", width
				desc->cmd[0] == '.' ? desc->cmd + 1 : desc->cmd,
				desc->str ? desc->str : "", VTY_NEWLINE);
#endif /* 0 */
		}

	if ((desc = desc_cr)) {
		if (!desc->str)
			vty_out(vty, "  %-s%s",
				desc->cmd[0] == '.' ? desc->cmd + 1 : desc->cmd,
				VTY_NEWLINE);
		else if (desc_width >= strlen(desc->str))
			vty_out(vty, "  %-*s  %s%s", width,
				desc->cmd[0] == '.' ? desc->cmd + 1 : desc->cmd,
				desc->str, VTY_NEWLINE);
		else
			vty_describe_fold(vty, width, desc_width, desc);
	}

	cmd_free_strvec(vline);
	vector_free(describe);

	vty_prompt(vty);
	vty_redraw_line(vty);
#endif
}

struct vty *vty_new(void)
{
	struct vty *new;
    
    new = malloc(sizeof(struct vty));
    if (NULL == new) {
        return NULL;
    }
    memset(new, 0x00, sizeof(struct vty));
	new->obuf = (struct buffer *)buffer_new(100);
    if (NULL == new->obuf) {
        free(new);
        return NULL;
    }
	new->buf = malloc(BUFSIZ);
    if (NULL == new->buf) {
        free(new->obuf);
        free(new);
        return NULL;
    }
	new->max = BUFSIZ;
	new->sb_buffer = NULL;

	return new;
}

struct vty *vty_create(int vty_sock, void *addr, int family)
{
    char ip_str[64];
	struct vty *vty;
    struct sockaddr_in *sin;
    struct sockaddr_in6 *sin6;

	vty = vty_new();
	vty->fd = vty_sock;
	///vty->type = VTY_TERM;
    if (addr) {
        if (AF_INET6 == family) {
            sin6 = (struct sockaddr_in6 *)addr;
            inet_ntop(AF_INET6, (void *)(&(sin6->sin6_addr)), ip_str, 64);
            vty->address = strdup(ip_str);
            vty->port = sin6->sin6_port;
        } else {
            sin = (struct sockaddr_in *)addr;
            inet_ntop(AF_INET, (void *)(&(sin->sin_addr)), ip_str, 64);
            vty->address = strdup(ip_str);
            vty->port = sin->sin_port;
        }
    }

#if 0
    if (host.no_password_check) {
		if (host.advanced) {
			vty->node = ENABLE_NODE;
        } else {
			vty->node = VIEW_NODE;
        }
	} else {
		vty->node = AUTH_NODE;
    }
#endif
    vty->node = VIEW_NODE;

	///vty->fail = 0;
	vty->cp = 0;
    memset(vty->buf, 0, vty->max);
	vty->length = 0;
	memset(vty->hist, 0, sizeof(vty->hist));
	vty->hp = 0;
	vty->hindex = 0;

	///vector_set_index(vtyvec, vty_sock, vty);
	vty->status = VTY_NORMAL;
	vty->v_timeout = g_vty_timeout_val;
#if 0
	if (host.lines >= 0)
		vty->lines = host.lines;
	else
		vty->lines = -1;
#endif
	vty->iac = 0;
	vty->iac_sb_in_progress = 0;
	vty->sb_buffer = buffer_new(2000);

#if 0
	if (!no_password_check) {
		/* Vty is not available if password isn't set. */
		if (host.password == NULL && host.password_encrypt == NULL) {
			vty_out(vty, "Vty password is not set.%s", VTY_NEWLINE);
			vty->status = VTY_CLOSE;
			vty_destroy(vty);
			return NULL;
		}
	}
#endif
#if 1
    /* Say hello to the world. */
	vty_hello(vty);
#if 0
	if (!no_password_check)
		vty_out(vty, "%sUser Access Verification%s%s", VTY_NEWLINE,
			VTY_NEWLINE, VTY_NEWLINE);
#endif

	/* Setting up terminal. */
	vty_will_echo(vty);
	vty_will_suppress_go_ahead(vty);

	vty_dont_linemode(vty);
	vty_do_window_size(vty);
	/* vty_dont_lflow_ahead (vty); */

	vty_prompt(vty);
#endif

	return vty;
}

void vty_destroy(struct vty *vty)
{
    int i;

    if (vty) {
    
        if (!buffer_empty(vty->obuf)) {
            buffer_flush_all(vty->obuf, vty->fd);
        }
        if (vty->obuf) {
            buffer_free(vty->obuf);
        }
        for (i = 0; i < VTY_MAXHIST; i++) {
            if (vty->hist[i]) {
                free(vty->hist[i]);
                vty->hist[i] = NULL;
            }
        }
        ///vector_unset(vtyvec, vty->fd);
        if (vty->fd > 0) {
            close(vty->fd);
            vty->fd = 0;
        }
        if (vty->address) {
            free(vty->address);
            vty->address = NULL;
        }

        free(vty);
        vty = NULL;
    }

    return;
}

int vty_flush(struct vty *vty)
{
	int erase;
	int dont_more;

    printf("vty status:%d\n", vty->status);
	if (vty->status == VTY_START || vty->status == VTY_CONTINUE) {
		if (vty->status == VTY_CONTINUE && vty->output_func) {
			erase = 1;
		} else {
			erase = 0;
        }

		if (vty->output_func == NULL) {
			dont_more = 1;
        } else {
			dont_more = 0;
        }

		if (vty->lines == 0) {
			erase = 0;
			dont_more = 1;
		}

		buffer_flush_vty_all(vty->obuf, vty->fd, erase, dont_more);

		if (vty->status == VTY_CLOSE) {
			vty_destroy(vty);
			return 0;
		}

		if (vty->output_func == NULL) {
			vty->status = VTY_NORMAL;
			vty_prompt(vty);
			///vty_event(VTY_WRITE, vty_sock, vty);
		} else {
			vty->status = VTY_MORE;
        }

#if 0
		if (vty->lines == 0) {
			if (vty->output_func == NULL)
				vty_event(VTY_READ, vty_sock, vty);
			else {
				if (vty->output_func)
					(*vty->output_func) (vty, 0);
				vty_event(VTY_WRITE, vty_sock, vty);
			}
		}
#endif
	} else {
		if ((vty->status == VTY_MORE) || (vty->status == VTY_MORELINE)) {
			erase = 1;
        } else {
			erase = 0;
        }

#if 1
		if (vty->lines == 0) {
			buffer_flush_window(vty->obuf, vty->fd, vty->width, 25, 0, 1);
        } else if (vty->status == VTY_MORELINE) {
			buffer_flush_window(vty->obuf, vty->fd, vty->width, 1, erase, 0);
        } else {
			buffer_flush_window(vty->obuf, vty->fd, vty->width,
                    ((vty->lines >= 0) ? vty->lines : vty->height),
                    erase, 0);
        }
#endif

		if (buffer_empty(vty->obuf)) {
			if (vty->status == VTY_CLOSE) {
				vty_destroy(vty);
            } else {
				vty->status = VTY_NORMAL;
#if 0
				if (vty->lines == 0) {
					vty_event(VTY_READ, vty_sock, vty);
                }
#endif
			}
		} else {
			vty->status = VTY_MORE;
#if 0
			if (vty->lines == 0) {
				vty_event(VTY_WRITE, vty_sock, vty);
            }
#endif
		}
	}

	return 0;
}

int vty_read(struct vty *vty)
{
	int recode;
	int i, nbytes, sock;
	unsigned char buf[BUFSIZ];

    if (NULL == vty) {
        return -1;
    }
	sock = vty->fd;

	nbytes = read(vty->fd, buf, BUFSIZ);
	if (nbytes <= 0) {
		vty->status = VTY_CLOSE;
    }

    ///printf("read nbytes:%d\n", nbytes);
	///for (i = 0; i < nbytes; i++) {
    ///    printf("%02x ", buf[i]);
    ///    if (i % 16 == 15) {
    ///        printf("\n");
    ///    }
    ///}
    ///printf("\n");
	for (i = 0; i < nbytes; i++) {
        printf("%02x ", buf[i]);
		if (buf[i] == IAC) {
			if (!vty->iac) {
				vty->iac = 1;
				continue;
			} else {
				vty->iac = 0;
			}
		}

		if (vty->iac_sb_in_progress && !vty->iac) {
            ///printf("put buffer\n");
			buffer_putc(vty->sb_buffer, buf[i]);
			continue;
		}

		if (vty->iac) {
#if __TELNET_DEBUG__
			recode = vty_telnet_option(vty, buf + i, nbytes - i);
#else
			recode = vty_telnet_option(vty, buf + i);
#endif
			vty->iac = 0;
			i += recode;
            ///printf("recode:%d\n", recode);
			continue;
		}

        printf("start vty more, %d\n", vty->status);
		if (vty->status == VTY_MORE) {
			switch (buf[i]) {
			case CONTROL('C'):
			case 'q':
			case 'Q':
				if (vty->output_func) {
					(*vty->output_func)(vty, 1);
                }
				vty_buffer_reset(vty);
				break;
			case '\n':
			case '\r':
				vty->status = VTY_MORELINE;
				if (vty->output_func) {
					(*vty->output_func)(vty, 0);
                }
				break;
			default:
				if (vty->output_func) {
					(*vty->output_func)(vty, 0);
                }
				break;
			}
			continue;
		}

		/* Escape character. */
		if (vty->escape == VTY_ESCAPE) {
			vty_escape_map(vty, buf[i]);
			continue;
		}

		/* Pre-escape status. */
		if (vty->escape == VTY_PRE_ESCAPE) {
			switch (buf[i]) {
			case '[':
				vty->escape = VTY_ESCAPE;
				break;
			case 'b':
				vty_backward_word(vty);
				vty->escape = VTY_NORMAL;
				break;
			case 'f':
				vty_forward_word(vty);
				vty->escape = VTY_NORMAL;
				break;
			case 'd':
				vty_forward_kill_word(vty);
				vty->escape = VTY_NORMAL;
				break;
			case CONTROL('H'):
			case 0x7f:
				vty_backward_kill_word(vty);
				vty->escape = VTY_NORMAL;
				break;
			default:
				vty->escape = VTY_NORMAL;
				break;
			}
			continue;
		}

		switch (buf[i]) {
		case CONTROL('A'):
			vty_beginning_of_line(vty);
			break;
		case CONTROL('B'):
			vty_backward_char(vty);
			break;
		case CONTROL('C'):
			vty_stop_input(vty);
			break;
		case CONTROL('D'):
			vty_delete_char(vty);
			break;
		case CONTROL('E'):
			vty_end_of_line(vty);
			break;
		case CONTROL('F'):
			vty_forward_char(vty);
			break;
		case CONTROL('H'):
		case 0x7f:
			vty_delete_backward_char(vty);
			break;
		case CONTROL('K'):
			vty_kill_line(vty);
			break;
		case CONTROL('N'):
			vty_next_line(vty);
			break;
		case CONTROL('P'):
			vty_previous_line(vty);
			break;
		case CONTROL('T'):
			vty_transpose_chars(vty);
			break;
		case CONTROL('U'):
			vty_kill_line_from_beginning(vty);
			break;
		case CONTROL('W'):
			vty_backward_kill_word(vty);
			break;
		case CONTROL('Z'):
			vty_end_config(vty);
			break;
		case '\n':
		case '\r':
			vty_out(vty, "%s", VTY_NEWLINE);
			vty_execute(vty);
			break;
		case '\t':
			vty_complete_command(vty);
			break;
		case '?':
			if ((vty->node == AUTH_NODE)
			    || (vty->node == AUTH_ENABLE_NODE)) {
				vty_self_insert(vty, buf[i]);
            } else {
				vty_describe_command(vty);
            }
			break;
		case '\033':
			if (i + 1 < nbytes && buf[i + 1] == '[') {
				vty->escape = VTY_ESCAPE;
				i++;
			} else {
				vty->escape = VTY_PRE_ESCAPE;
            }
			break;
		default:
			if ((buf[i] > 31) && (buf[i] < 127)) {
				vty_self_insert(vty, buf[i]);
            }
			break;
		}
	}

	/* Check status. */
	if (vty->status == VTY_CLOSE) {
		vty_destroy(vty);
	 } else {
        //printf("vty flush, status:%d\n", vty->status);
		vty_flush(vty);///vty_event(VTY_WRITE, vty_sock, vty);
		///vty_event(VTY_READ, vty_sock, vty);
	}
	return 0;
}
