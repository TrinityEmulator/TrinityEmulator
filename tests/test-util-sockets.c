/*
 * Tests for util/qemu-sockets.c
 *
 * Copyright 2018 Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "qemu/osdep.h"
#include "qemu-common.h"
#include "qemu/sockets.h"
#include "qapi/error.h"
#include "socket-helpers.h"
#include "monitor/monitor.h"

static void test_fd_is_socket_bad(void)
{
    char *tmp = g_strdup("qemu-test-util-sockets-XXXXXX");
    int fd = mkstemp(tmp);
    if (fd != 0) {
        unlink(tmp);
    }
    g_free(tmp);

    g_assert(fd >= 0);

    g_assert(!fd_is_socket(fd));
    close(fd);
}

static void test_fd_is_socket_good(void)
{
    int fd = qemu_socket(PF_INET, SOCK_STREAM, 0);

    g_assert(fd >= 0);

    g_assert(fd_is_socket(fd));
    close(fd);
}

static int mon_fd = -1;
static const char *mon_fdname;

int monitor_get_fd(Monitor *mon, const char *fdname, Error **errp)
{
    g_assert(cur_mon);
    g_assert(mon == cur_mon);
    if (mon_fd == -1 || !g_str_equal(mon_fdname, fdname)) {
        error_setg(errp, "No fd named %s", fdname);
        return -1;
    }
    return dup(mon_fd);
}

/* Syms in libqemustub.a are discarded at .o file granularity.
 * To replace monitor_get_fd() we must ensure everything in
 * stubs/monitor.c is defined, to make sure monitor.o is discarded
 * otherwise we get duplicate syms at link time.
 */
__thread Monitor *cur_mon;
int monitor_vprintf(Monitor *mon, const char *fmt, va_list ap) { abort(); }
void monitor_init_qmp(Chardev *chr, bool pretty, Error **errp) {}
void monitor_init_hmp(Chardev *chr, bool use_readline, Error **errp) {}


static void test_socket_fd_pass_name_good(void)
{
    SocketAddress addr;
    int fd;

    cur_mon = g_malloc(1); /* Fake a monitor */
    mon_fdname = "myfd";
    mon_fd = qemu_socket(AF_INET, SOCK_STREAM, 0);
    g_assert_cmpint(mon_fd, >, STDERR_FILENO);

    addr.type = SOCKET_ADDRESS_TYPE_FD;
    addr.u.fd.str = g_strdup(mon_fdname);

    fd = socket_connect(&addr, &error_abort);
    g_assert_cmpint(fd, !=, -1);
    g_assert_cmpint(fd, !=, mon_fd);
    close(fd);

    fd = socket_listen(&addr, 1, &error_abort);
    g_assert_cmpint(fd, !=, -1);
    g_assert_cmpint(fd, !=, mon_fd);
    close(fd);

    g_free(addr.u.fd.str);
    mon_fdname = NULL;
    close(mon_fd);
    mon_fd = -1;
    g_free(cur_mon);
    cur_mon = NULL;
}

static void test_socket_fd_pass_name_bad(void)
{
    SocketAddress addr;
    Error *err = NULL;
    int fd;

    cur_mon = g_malloc(1); /* Fake a monitor */
    mon_fdname = "myfd";
    mon_fd = dup(STDOUT_FILENO);
    g_assert_cmpint(mon_fd, >, STDERR_FILENO);

    addr.type = SOCKET_ADDRESS_TYPE_FD;
    addr.u.fd.str = g_strdup(mon_fdname);

    fd = socket_connect(&addr, &err);
    g_assert_cmpint(fd, ==, -1);
    error_free_or_abort(&err);

    fd = socket_listen(&addr, 1, &err);
    g_assert_cmpint(fd, ==, -1);
    error_free_or_abort(&err);

    g_free(addr.u.fd.str);
    mon_fdname = NULL;
    close(mon_fd);
    mon_fd = -1;
    g_free(cur_mon);
    cur_mon = NULL;
}

static void test_socket_fd_pass_name_nomon(void)
{
    SocketAddress addr;
    Error *err = NULL;
    int fd;

    g_assert(cur_mon == NULL);

    addr.type = SOCKET_ADDRESS_TYPE_FD;
    addr.u.fd.str = g_strdup("myfd");

    fd = socket_connect(&addr, &err);
    g_assert_cmpint(fd, ==, -1);
    error_free_or_abort(&err);

    fd = socket_listen(&addr, 1, &err);
    g_assert_cmpint(fd, ==, -1);
    error_free_or_abort(&err);

    g_free(addr.u.fd.str);
}


static void test_socket_fd_pass_num_good(void)
{
    SocketAddress addr;
    int fd, sfd;

    g_assert(cur_mon == NULL);
    sfd = qemu_socket(AF_INET, SOCK_STREAM, 0);
    g_assert_cmpint(sfd, >, STDERR_FILENO);

    addr.type = SOCKET_ADDRESS_TYPE_FD;
    addr.u.fd.str = g_strdup_printf("%d", sfd);

    fd = socket_connect(&addr, &error_abort);
    g_assert_cmpint(fd, ==, sfd);

    fd = socket_listen(&addr, 1, &error_abort);
    g_assert_cmpint(fd, ==, sfd);

    g_free(addr.u.fd.str);
    close(sfd);
}

static void test_socket_fd_pass_num_bad(void)
{
    SocketAddress addr;
    Error *err = NULL;
    int fd, sfd;

    g_assert(cur_mon == NULL);
    sfd = dup(STDOUT_FILENO);

    addr.type = SOCKET_ADDRESS_TYPE_FD;
    addr.u.fd.str = g_strdup_printf("%d", sfd);

    fd = socket_connect(&addr, &err);
    g_assert_cmpint(fd, ==, -1);
    error_free_or_abort(&err);

    fd = socket_listen(&addr, 1, &err);
    g_assert_cmpint(fd, ==, -1);
    error_free_or_abort(&err);

    g_free(addr.u.fd.str);
    close(sfd);
}

static void test_socket_fd_pass_num_nocli(void)
{
    SocketAddress addr;
    Error *err = NULL;
    int fd;

    cur_mon = g_malloc(1); /* Fake a monitor */

    addr.type = SOCKET_ADDRESS_TYPE_FD;
    addr.u.fd.str = g_strdup_printf("%d", STDOUT_FILENO);

    fd = socket_connect(&addr, &err);
    g_assert_cmpint(fd, ==, -1);
    error_free_or_abort(&err);

    fd = socket_listen(&addr, 1, &err);
    g_assert_cmpint(fd, ==, -1);
    error_free_or_abort(&err);

    g_free(addr.u.fd.str);
}

#ifdef __linux__
static gchar *abstract_sock_name;

static gpointer unix_server_thread_func(gpointer user_data)
{
    SocketAddress addr;
    Error *err = NULL;
    int fd = -1;
    int connfd = -1;
    struct sockaddr_un un;
    socklen_t len = sizeof(un);

    addr.type = SOCKET_ADDRESS_TYPE_UNIX;
    addr.u.q_unix.path = abstract_sock_name;
    addr.u.q_unix.tight = user_data != NULL;
    addr.u.q_unix.abstract = true;

    fd = socket_listen(&addr, 1, &err);
    g_assert_cmpint(fd, >=, 0);
    g_assert(fd_is_socket(fd));

    connfd = accept(fd, (struct sockaddr *)&un, &len);
    g_assert_cmpint(connfd, !=, -1);

    close(fd);

    return NULL;
}

static gpointer unix_client_thread_func(gpointer user_data)
{
    SocketAddress addr;
    Error *err = NULL;
    int fd = -1;

    addr.type = SOCKET_ADDRESS_TYPE_UNIX;
    addr.u.q_unix.path = abstract_sock_name;
    addr.u.q_unix.tight = user_data != NULL;
    addr.u.q_unix.abstract = true;

    fd = socket_connect(&addr, &err);

    g_assert_cmpint(fd, >=, 0);

    close(fd);

    return NULL;
}

static void test_socket_unix_abstract_good(void)
{
    GRand *r = g_rand_new();

    abstract_sock_name = g_strdup_printf("unix-%d-%d", getpid(),
                                         g_rand_int_range(r, 100, 1000));

    /* non tight socklen serv and cli */
    GThread *serv = g_thread_new("abstract_unix_server",
                                 unix_server_thread_func,
                                 NULL);

    sleep(1);

    GThread *cli = g_thread_new("abstract_unix_client",
                                unix_client_thread_func,
                                NULL);

    g_thread_join(cli);
    g_thread_join(serv);

    /* tight socklen serv and cli */
    serv = g_thread_new("abstract_unix_server",
                        unix_server_thread_func,
                        (gpointer)1);

    sleep(1);

    cli = g_thread_new("abstract_unix_client",
                       unix_client_thread_func,
                       (gpointer)1);

    g_thread_join(cli);
    g_thread_join(serv);

    g_free(abstract_sock_name);
}
#endif

int main(int argc, char **argv)
{
    bool has_ipv4, has_ipv6;

    socket_init();

    g_test_init(&argc, &argv, NULL);

    /* We're creating actual IPv4/6 sockets, so we should
     * check if the host running tests actually supports
     * each protocol to avoid breaking tests on machines
     * with either IPv4 or IPv6 disabled.
     */
    if (socket_check_protocol_support(&has_ipv4, &has_ipv6) < 0) {
        g_printerr("socket_check_protocol_support() failed\n");
        goto end;
    }

    if (has_ipv4) {
        g_test_add_func("/util/socket/is-socket/bad",
                        test_fd_is_socket_bad);
        g_test_add_func("/util/socket/is-socket/good",
                        test_fd_is_socket_good);
        g_test_add_func("/socket/fd-pass/name/good",
                        test_socket_fd_pass_name_good);
        g_test_add_func("/socket/fd-pass/name/bad",
                        test_socket_fd_pass_name_bad);
        g_test_add_func("/socket/fd-pass/name/nomon",
                        test_socket_fd_pass_name_nomon);
        g_test_add_func("/socket/fd-pass/num/good",
                        test_socket_fd_pass_num_good);
        g_test_add_func("/socket/fd-pass/num/bad",
                        test_socket_fd_pass_num_bad);
        g_test_add_func("/socket/fd-pass/num/nocli",
                        test_socket_fd_pass_num_nocli);
    }

#ifdef __linux__
    g_test_add_func("/util/socket/unix-abstract/good",
                    test_socket_unix_abstract_good);
#endif

end:
    return g_test_run();
}
