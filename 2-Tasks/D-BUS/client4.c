#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>

const char *const SERVER1_BUS_NAME = "in.softprayog.server1";
const char *const SERVER1_OBJECT_PATH_NAME = "/in/softprayog/server1";
const char *const SERVER1_INTERFACE_NAME = "in.softprayog.server1.interface";
const char *const SERVER1_METHOD_MULTI_NAME = "multi";

const char *const SERVER2_BUS_NAME = "in.softprayog.server2";
const char *const SERVER2_OBJECT_PATH_NAME = "/in/softprayog/server2";
const char*const SERVER2_INTERFACE_NAME = "in.softprayog.server2.interface";
const char *const SERVER2_METHOD_SUB_NAME = "sub";

void print_dbus_error(const char *prefix, DBusError *error) {
    fprintf(stderr, "%s: %s\n", prefix, error->message);
    dbus_error_free(error);
}

int main(int argc, char **argv) {
    DBusConnection *conn;
    DBusError dbus_error;
    dbus_error_init(&dbus_error);

    conn = dbus_bus_get(DBUS_BUS_SESSION, &dbus_error);
    if (dbus_error_is_set(&dbus_error)) {
        print_dbus_error("dbus_bus_get", &dbus_error);
        exit(1);
    }

    if (conn == NULL) {
        fprintf(stderr, "Failed to connect to the D-Bus session bus.\n");
        exit(1);
    }

    DBusMessage *msg;
    DBusMessageIter args;
    DBusPendingCall *pending;
    int a, b, c;
    char str[80];

    // Multi method from server1
    printf("Enter two numbers to be multiplied: ");
    scanf("%d %d", &a, &b);

    msg = dbus_message_new_method_call(SERVER1_BUS_NAME, SERVER1_OBJECT_PATH_NAME, SERVER1_INTERFACE_NAME, SERVER1_METHOD_MULTI_NAME);
    dbus_message_iter_init_append(msg, &args);
    dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &a);
    dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &b);

    if (!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {
        fprintf(stderr, "Failed to send message.\n");
        exit(1);
    }

    dbus_message_unref(msg);

    dbus_pending_call_block(pending);

    msg = dbus_pending_call_steal_reply(pending);
    if (msg == NULL) {
        fprintf(stderr, "Failed to get a reply.\n");
        exit(1);
    }

    dbus_message_iter_init(msg, &args);
    dbus_message_iter_get_basic(&args, &c);

    printf("Multi Result: %d\n", c);

    dbus_message_unref(msg);
    dbus_pending_call_unref(pending);

    // Sub method from server2
    printf("Enter two numbers to be subtracted: ");
    scanf("%d %d", &a, &b);

    msg = dbus_message_new_method_call(SERVER2_BUS_NAME, SERVER2_OBJECT_PATH_NAME, SERVER2_INTERFACE_NAME, SERVER2_METHOD_SUB_NAME);
    dbus_message_iter_init_append(msg, &args);
    dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &a);
    dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &b);

    if (!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {
        fprintf(stderr, "Failed to send message.\n");
        exit(1);
    }

    dbus_message_unref(msg);

    dbus_pending_call_block(pending);

    msg = dbus_pending_call_steal_reply(pending);
    if (msg == NULL) {
        fprintf(stderr, "Failed to get a reply.\n");
        exit(1);
    }

    dbus_message_iter_init(msg, &args);
    dbus_message_iter_get_basic(&args, &c);

    printf("Sub Result: %d\n", c);

    dbus_message_unref(msg);
    dbus_pending_call_unref(pending);

    dbus_connection_unref(conn);

    return 0;
}