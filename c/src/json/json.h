/**
 * Rapid-prototyping protection schemes with IEC 61850
 *
 * Copyright (c) 2014 Steven Blair
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#if JSON_INTERFACE == 1

#ifndef JSONRPC_H_
#define JSONRPC_H_

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <windows.h>	// TODO cross-platform version?
#include "dataModelIndex.h"
#include "ctypes.h"

#define WEB_SERVER_SELECT_MAX_TIME		5	// ms
#define JSON_OUTPUT_PRETTIFY			0
#define JSON_USE_HTTP_AUTH				0

#if JSON_OUTPUT_PRETTIFY == 1
#define ACSI_RESPONSE_MAX_SIZE			64000
#else
#define ACSI_RESPONSE_MAX_SIZE			16000
#endif
#define ACSI_RESPONSE_SIZE_WARNING		128
#define ACSI_GET_DEFINITION				"definition"
#define ACSI_GET_DIRECTORY				"directory"
#define ACSI_OK							"ok"
#define ACSI_NOT_POSSIBLE				"not possible"
#define ACSI_NOT_FOUND					"404"
#define ACSI_BUFFER_OVERRUN				"buffer overrun; increase ACSI_RESPONSE_MAX_SIZE"
#define ACSI_REQUEST_CHECK_LENGTH(len)	if (len > ACSI_RESPONSE_MAX_SIZE - ACSI_RESPONSE_SIZE_WARNING) {return -1;}
#define ACSI_REQUEST_CHECK_LENGTH2(len)	if (len > ACSI_RESPONSE_MAX_SIZE - ACSI_RESPONSE_SIZE_WARNING) {return len;}


Item *getIED(char *iedObjectRef);
Item *getLD(char *iedObjectRef, char *objectRef);
Item *getLN(char *iedObjectRef, char *LDObjectRef, char *objectRef);

/**
 * Find an item within the specified Logical Node object hierarchy.
 *
 * Provide a list of item names (where num is the number), e.g., getItem(ln, 3, "C1", "LN0", "Mod").
 */
Item *getItem(Item *ln, int num, ...);

/**
 * Converts data object reference (DataObjectRef), e.g., "myLD/MMXU1.PhV.phsA", to the database Item, if found. Returns NULL otherwise.
 */
Item *getItemFromPath(char *iedObjectRef, char *objectRefPath);

/**
 * Sets the value of leaf data items. Returns the number of bytes written if successful, or 0 otherwise.
 */
int setItem(Item *item, char *input, int input_len);

/**
 * Prints leaf data items to the specified buffer. The buffer must be large enough. Returns the number of characters printed.
 */
int itemToJSON(char *buf, Item *item);

/**
 * Prints hierarchy of items without whitespace, starting from the root, to the specified buffer.
 *
 * The buffer must be large enough. Returns the number of characters printed.
 */
int itemTreeToJSON(char *buf, Item *root);

/**
 * Prints hierarchy of items with whitespace, starting from the root, to the specified buffer.
 *
 * The buffer must be large enough. Returns the number of characters printed.
 */
int itemTreeToJSONPretty(char *buf, Item *root);

/**
 * Prints a self-descriptive hierarchy of items with whitespace, starting from the root, to the specified buffer.
 *
 * The buffer must be large enough. Returns the number of characters printed.
 */
int itemDescriptionTreeToJSONPretty(char *buf, Item *root, unsigned char deep);

/**
 * Prints a self-descriptive hierarchy of items without whitespace, starting from the root, to the specified buffer.
 *
 * The buffer must be large enough. Returns the number of characters printed.
 */
int itemDescriptionTreeToJSON(char *buf, Item *root, unsigned char deep);

/**
 * Starts JSON HTTP-based interface.
 */
void start_json_interface();

/**
 * Send an HTTP request to a server.
 */
char *send_http_request(int port, int *len, char *method, char *url);

/**
 * Send an HTTP POST request to a server.
 */
char *send_http_request_post(int port, int *len, char *url, char *value);


unsigned char isClient(ACSIClient *client, char ip[48], int port) {
	if (client == NULL) {
		return FALSE;
	}

	if (strcmp(client->ip, ip) == 0 && client->port == port) {
		return TRUE;
	}

	return FALSE;
}

ACSIClient *findClient(ACSIClient *client_list, char ip[48], int port) {
	ACSIClient *client = client_list;

	while (client != NULL) {
		if (isClient(client, ip, port)) {
			return client;
		}

		client = client->next;
	}

	return NULL;
}

ACSIClient *addClient(ACSIClient *client_list, char ip[48], int port) {
	if (client_list == NULL) {
		client_list = (ACSIClient *) calloc(1, sizeof(ACSIClient));
		memcpy(client_list->ip, ip, 48);
		client_list->port = port;
		return client_list;
	}

	ACSIClient *found = findClient(client_list, ip, port);

	if (found == NULL) {
		ACSIClient *end = client_list;
		while (end->next != NULL) {
			end = end->next;
		}

		end->next = (ACSIClient *) calloc(1, sizeof(ACSIClient));
		memcpy(end->next->ip, ip, 48);
		end->next->port = port;
		return end->next;

		return end;
	}
	else {
		return found;
	}
}



#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif

#endif /* JSONRPC_H_ */

#endif
