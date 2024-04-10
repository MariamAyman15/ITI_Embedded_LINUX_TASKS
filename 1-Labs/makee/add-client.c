/*
 *
 *     add-client.c: client program, takes two numbers as input,
 *                   sends to server for addition or subtraction,
 *                   gets result from server,
 *                   prints the result on the screen
 *
 */

#include <stdio.h>     //provides input/output operations such as printf and scanf.
#include <stdlib.h>    //provides general utility functions such as memory allocation and random number generation
#include <unistd.h>    //provides access to various POSIX operating system API functions such as fork, exec, and sleep.
#include <string.h>    //provides string manipulation functions such as strlen, strcpy, and strcat
#include <stdbool.h>   //provides the Boolean data type and related macros such as true and false.
#include <ctype.h>     //provides functions for testing and converting characters such as isalpha, isdigit, and toupper.

#include <dbus/dbus.h> //header file for the D-Bus library, 


const char *const INTERFACE_NAME = "in.softprayog.dbus_example";
const char *const SERVER_BUS_NAME = "in.softprayog.add_server";
const char *const CLIENT_BUS_NAME = "in.softprayog.add_client";
const char *const SERVER_OBJECT_PATH_NAME = "/in/softprayog/adder";
const char *const CLIENT_OBJECT_PATH_NAME = "/in/softprayog/add_client";
const char *const ADD_METHOD_NAME = "add_numbers";
const char *const SUBTRACT_METHOD_NAME = "subtract_numbers";

DBusError dbus_error;
void print_dbus_error (char *str);

int main (int argc, char **argv)
{
    DBusConnection *conn;
    int ret;
    char input[80];
    int num1, num2, result;

    dbus_error_init (&dbus_error);
    conn = dbus_bus_get (DBUS_BUS_SESSION, &dbus_error);

    if (dbus_error_is_set (&dbus_error))
        print_dbus_error ("dbus_bus_get");

    if (!conn) 
        exit (1);

    printf ("Please type 'add' or 'subtract' followed by two numbers: ");

    while (fgets (input, 78, stdin) != NULL) {

        // Get a well known name
        while (1) {
            ret = dbus_bus_request_name (conn, CLIENT_BUS_NAME, 0, &dbus_error);
            if (ret == DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) 
               break;

            if (ret == DBUS_REQUEST_NAME_REPLY_IN_QUEUE) {
               fprintf (stderr, "Waiting for the bus ... \n");
               sleep (1);
               continue;
            }

            if (dbus_error_is_set (&dbus_error))
               print_dbus_error ("dbus_bus_get");
        }
        
        DBusMessage *request;

        if (strcmp(input, "add\n") == 0) {
            request = dbus_message_new_method_call(SERVER_BUS_NAME, SERVER_OBJECT_PATH_NAME, INTERFACE_NAME, ADD_METHOD_NAME);
        } else if (strcmp(input, "subtract\n") == 0) {
            request = dbus_message_new_method_call(SERVER_BUS_NAME, SERVER_OBJECT_PATH_NAME, INTERFACE_NAME, SUBTRACT_METHOD_NAME);
        } else {
            printf("Invalid input, please try again.\n");
            continue;
        }

        DBusMessageIter iter;
        dbus_message_iter_init_append (request, &iter);
        char *ptr = input + strlen(input);
        if (sscanf(input, "%s %d %d", ptr, &num1, &num2) != 3) {
            fprintf (stderr, "Error in sscanf\n");
            exit (1);
        }

        if (!dbus_message_iter_append_basic (&iter, DBUS_TYPE_INT32, &num1) ||
            !dbus_message_iter_append_basic (&iter, DBUS_TYPE_INT32, &num2)) {
            fprintf (stderr, "Error in dbus_message_iter_append_basic\n");
            exit (1);
        }

        DBusPendingCall *pending_return;

        if (!dbus_connection_send_with_reply (conn, request, &pending_return, -1)) {
            fprintf (stderr, "Error in dbus_connection_send_with_reply\n");
            exit (1);
        }

        if (pending_return == NULL) {
            fprintf (stderr, "pending return is NULL");
            exit (1);
        }

        dbus_connection_flush (conn);
        dbus_message_unref (request);	
        dbus_pending_call_block (pending_return);

        DBusMessage *reply;

        if ((reply = dbus_pending_call_steal_reply (pending_return)) == NULL) {
            fprintf (stderr, "Error in dbus_pending_call_steal_reply");
            exit (1);
        }

        dbus_pending_call_unref	(pending_return);

        if (strcmp(METHOD_NAME, ADD_METHOD_NAME) == 0) {
           if (dbus_message_get_args(reply, &dbus_error, DBUS_TYPE_INT32, &result, DBUS_TYPE_INVALID)) {
                printf("Result: %d\n", result);
            }
        } else if (strcmp(METHOD_NAME, SUBTRACT_METHOD_NAME) == 0) {
            if (dbus_message_get_args(reply, &dbus_error, DBUS_TYPE_INT32, &result, DBUS_TYPE_INVALID)) {
                printf("Result: %d\n", result);
            }
        } else {
            fprintf(stderr, "Did not get arguments in reply\n");
            exit(1);
        }

        printf ("Please type 'add' or 'subtract' followed by two numbers: ");
    }

    dbus_connection_close (conn);
    return 0;
}

void print_dbus_error (char *str)
{
    fprintf (stderr, "%s: %s\n", str, dbus_error.message);
    dbus_error_free (&dbus_error);
}