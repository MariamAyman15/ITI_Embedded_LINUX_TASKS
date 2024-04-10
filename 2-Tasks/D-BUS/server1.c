//sub and sum
#include <stdio.h>
#include <stdlib.h>
#include <dbus/dbus.h>

const char *const BUS_NAME = "in.softprayog.server1";
const char *const OBJECT_PATH_NAME = "/in/softprayog/server1";
const char *const INTERFACE_NAME = "in.softprayog.server1.interface";
const char *const METHOD_SUM_NAME = "sum";
const char *const METHOD_SUB_NAME = "sub";

DBusError dbus_error;

void print_dbus_error(const char *prefix) {
    fprintf(stderr, "%s: %s\n", prefix, dbus_error.message);
    dbus_error_free(&dbus_error);
}

DBusHandlerResult handle_message(DBusConnection *conn, DBusMessage *msg, void *data) {
    DBusMessage *reply;
    DBusMessageIter args;
    dbus_uint32_t serial = 0;
    int a, b;
    char str[80];

    if (dbus_message_is_method_call(msg, INTERFACE_NAME, METHOD_SUM_NAME)) {
        dbus_message_iter_init(msg, &args);
        dbus_message_iter_get_basic(&args, &a);
        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &b);

        snprintf(str, sizeof(str), "%d", a + b);
        reply = dbus_message_new_method_return(msg);
        dbus_message_iter_init_append(reply, &args);
        dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &str);
    } else if (dbus_message_is_method_call(msg, INTERFACE_NAME, METHOD_SUB_NAME)) {
        dbus_message_iter_init(msg, &args);
        dbus_message_iter_get_basic(&args, &a);
        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &b);

        snprintf(str, sizeof(str), "%d", a - b);
        reply = dbus_message_new_method_return(msg);
        dbus_message_iter_init_append(reply, &args);
        dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &str);
    } else {
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    if (!dbus_connection_send(conn, reply, &serial)) {
        fprintf(stderr, "Failed to send message.\n");
        exit(1);
    }

    dbus_connection_flush(conn);
    dbus_message_unref(reply);

    return DBUS_HANDLER_RESULT_HANDLED;
}

int main(int argc, char **argv) {
    DBusConnection *conn;
    DBusError dbus_error;
    dbus_error_init(&dbus_error);

    conn = dbus_bus_get(DBUS_BUS_SESSION, &dbus_error);
    if (dbus_error_is_set(&dbus_error)) {
        print_dbus_error("dbus_bus_get");
        exit(1);
    }

    if (conn == NULL) {
        fprintf(stderr, "Failed to connect to the D-Bus session bus.\n");
        exit(1);
    }

    int ret;
    ret = dbus_bus_request_name(conn, BUS_NAME, DBUS_NAME_FLAG_REPLACE_EXISTING, &dbus_error);
    if (dbus_error_is_set(&dbus_error)) {
        print_dbus_error("dbus_bus_request_name");
        exit(1);
    }

    if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        fprintf(stderr, "Failed to request name on D-Bus: %d\n", ret);
        exit(1);
    }

    DBusObjectPathVTable vtable = { .message_function = handle_message };
    dbus_connection_register_object_path(conn, OBJECT_PATH_NAME, &vtable, NULL);

    while (true) {
        dbus_connection_read_write(conn, -1);
    }

    return 0;
}