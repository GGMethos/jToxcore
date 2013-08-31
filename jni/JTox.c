/* im_tox_jtoxcore_JTox.c
 *
 *  Copyright (C) 2013 Tox project All Rights Reserved.
 *
 *  This file is part of jToxcore
 *
 *  jToxcore is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  jToxcore is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with jToxcore.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <tox/tox.h>

#include "im_tox_jtoxcore_JTox.h"
#include "callbacks.h"

#define ADDR_SIZE_HEX (TOX_FRIEND_ADDRESS_SIZE * 2 + 1)

/**
 * Begin Utilities section
 */

/**
 * Convert a given binary address to a human-readable hexadecimal string
 */
void addr_to_hex(uint8_t *addr, char *buf) {
	int i;

	for (i = 0; i < TOX_FRIEND_ADDRESS_SIZE; i++) {
		char xx[3];
		snprintf(xx, sizeof(xx), "%02X", addr[i] & 0xff);
		strcat(buf, xx);
	}
}

/**
 * Convert a given human-readable hexadecimal string into binary
 */
void hex_to_addr(const char *hex, uint8_t *buf) {
	size_t len = strlen(hex);
	uint8_t *val = malloc(len);
	char *pos = malloc(strlen(hex) + 1);
	pos = strdup(hex);
	int i;

	for (i = 0; i < len; ++i, pos += 2)
		sscanf(pos, "%2hhx", &buf[i]);
}

/**
 * End Utilities section
 */

/**
 * Begin maintenance section
 */
JNIEXPORT jlong JNICALL Java_im_tox_jtoxcore_JTox_tox_1new(JNIEnv * env,
		jclass clazz) {
	tox_jni_globals_t *globals = malloc(sizeof(tox_jni_globals_t));
	globals->tox = tox_new();
	globals->frqc = 0;
	globals->frmc = 0;
	globals->ac = 0;
	globals->nc = 0;
	globals->smc = 0;
	globals->usc = 0;
	globals->rrc = 0;
	globals->csc = 0;
	return ((jlong) globals);
}

JNIEXPORT void JNICALL Java_im_tox_jtoxcore_JTox_tox_1bootstrap(JNIEnv * env,
		jobject obj, jlong messenger, jbyteArray ip, jint port, jstring address) {
	tox_IP_Port ipport;
	tox_IP _ip;

	jbyte *ip_array = (*env)->GetByteArrayElements(env, ip, 0);
	jsize n = (*env)->GetArrayLength(env, ip);
	int i;

	for (i = 0; i < n; ++i) {
		_ip.c[i] = ip_array[i];
	}

	(*env)->ReleaseByteArrayElements(env, ip, ip_array, 0);
	ipport.ip = _ip;
	ipport.port = htons((uint16_t) port);

	const char *_address = (*env)->GetStringUTFChars(env, address, 0);
	uint8_t __address[TOX_FRIEND_ADDRESS_SIZE];
	hex_to_addr(_address, __address);

	tox_bootstrap(((tox_jni_globals_t *) messenger)->tox, ipport, __address);

	(*env)->ReleaseStringUTFChars(env, address, _address);
}

JNIEXPORT void JNICALL Java_im_tox_jtoxcore_JTox_tox_1do(JNIEnv * env,
		jobject obj, jlong messenger) {
	tox_do(((tox_jni_globals_t *) messenger)->tox);
}

JNIEXPORT jint JNICALL Java_im_tox_jtoxcore_JTox_tox_1isconnected(JNIEnv * env,
		jobject obj, jlong messenger) {
	return tox_isconnected(((tox_jni_globals_t *) messenger)->tox);
}

JNIEXPORT void JNICALL Java_im_tox_jtoxcore_JTox_tox_1kill(JNIEnv * env,
		jobject jobj, jlong messenger) {
	tox_jni_globals_t *globals = (tox_jni_globals_t *) messenger;

	if (globals->frqc) {
		if (globals->frqc->jobj) {
			(*env)->DeleteGlobalRef(env, globals->frqc->jobj);
		}
		free(globals->frqc);
	}

	if (globals->frmc) {
		if (globals->frmc->jobj) {
			(*env)->DeleteGlobalRef(env, globals->frmc->jobj);
		}
		free(globals->frmc);
	}

	if (globals->ac) {
		if (globals->ac->jobj) {
			(*env)->DeleteGlobalRef(env, globals->ac->jobj);
		}
		free(globals->ac);
	}

	if (globals->nc) {
		if (globals->nc->jobj) {
			(*env)->DeleteGlobalRef(env, globals->nc->jobj);
		}
		free(globals->nc);
	}

	if (globals->smc) {
		if (globals->smc->jobj) {
			(*env)->DeleteGlobalRef(env, globals->smc->jobj);
		}
		free(globals->smc);
	}

	if (globals->usc) {
		if (globals->usc->jobj) {
			(*env)->DeleteGlobalRef(env, globals->usc->jobj);
		}
		free(globals->usc);
	}

	if (globals->rrc) {
		if (globals->rrc->jobj) {
			(*env)->DeleteGlobalRef(env, globals->rrc->jobj);
		}
		free(globals->rrc);
	}

	if (globals->csc) {
		if (globals->csc->jobj) {
			(*env)->DeleteGlobalRef(env, globals->csc->jobj);
		}
		free(globals->csc);
	}

	tox_kill(globals->tox);

	free(globals);
}

/**
 * End maintenance section
 */

/**
 * Begin general section
 */

JNIEXPORT jint JNICALL Java_im_tox_jtoxcore_JTox_tox_1addfriend(JNIEnv * env,
		jobject obj, jlong messenger, jstring address, jstring data) {
	const uint8_t *_address = (*env)->GetStringUTFChars(env, address, 0);
	const uint8_t *_data = (*env)->GetStringUTFChars(env, data, 0);

	uint8_t __address[TOX_FRIEND_ADDRESS_SIZE];
	hex_to_addr(_address, __address);
	uint8_t *__data = malloc(strlen(_data) + 1);

	jsize length = (*env)->GetStringUTFLength(env, data);

	int errcode = tox_addfriend(((tox_jni_globals_t *) messenger)->tox,
			__address, __data, length);

	free(__data);
	(*env)->ReleaseStringUTFChars(env, address, _address);
	(*env)->ReleaseStringUTFChars(env, data, _data);

	return errcode;
}

JNIEXPORT jint JNICALL Java_im_tox_jtoxcore_JTox_tox_1addfriend_1norequest(
		JNIEnv * env, jobject obj, jlong messenger, jstring address) {
	const char *_address = (*env)->GetStringUTFChars(env, address, 0);
	uint8_t __address[TOX_FRIEND_ADDRESS_SIZE];
	hex_to_addr(_address, __address);

	int errcode = tox_addfriend_norequest(
			((tox_jni_globals_t *) messenger)->tox, __address);

	(*env)->ReleaseStringUTFChars(env, address, _address);

	return errcode;
}

JNIEXPORT jstring JNICALL Java_im_tox_jtoxcore_JTox_tox_1getaddress(
		JNIEnv * env, jobject obj, jlong messenger) {
	uint8_t addr[TOX_FRIEND_ADDRESS_SIZE];
	tox_getaddress(((tox_jni_globals_t *) messenger)->tox, addr);
	char id[ADDR_SIZE_HEX] = { 0 };
	addr_to_hex(addr, id);

	jstring result = (*env)->NewStringUTF(env, id);
	return result;
}

JNIEXPORT jint JNICALL Java_im_tox_jtoxcore_JTox_tox_1getfriend_1id(
		JNIEnv * env, jobject obj, jlong messenger, jstring address) {
	const uint8_t *_address = (*env)->GetStringUTFChars(env, address, 0);

	uint8_t __address[TOX_FRIEND_ADDRESS_SIZE];
	hex_to_addr(_address, __address);

	int errcode = tox_getfriend_id(((tox_jni_globals_t *) messenger)->tox,
			__address);

	(*env)->ReleaseStringUTFChars(env, address, _address);

	return errcode;
}

JNIEXPORT jstring JNICALL Java_im_tox_jtoxcore_JTox_tox_1getclient_1id(
		JNIEnv * env, jobject obj, jlong messenger, jint friendnumber) {
	uint8_t address[TOX_FRIEND_ADDRESS_SIZE];

	if (tox_getclient_id(((tox_jni_globals_t *) messenger)->tox, friendnumber,
			address) == -1) {
		return 0;
	} else {
		char _address[ADDR_SIZE_HEX] = { 0 };
		addr_to_hex(address, _address);
		jstring result = (*env)->NewStringUTF(env, _address);
		return result;
	}
}

/**
 * End general section
 */

/**
 * Begin Callback Section
 */

JNIEXPORT void JNICALL Java_im_tox_jtoxcore_JTox_tox_1onfriendrequest(
		JNIEnv * env, jobject obj, jlong messenger, jobject callback) {

	friendrequest_callback_t *data = malloc(sizeof(friendrequest_callback_t));
	data->env = env;
	data->jobj = (*env)->NewGlobalRef(env, callback);
	(*env)->DeleteLocalRef(env, callback);
	((tox_jni_globals_t *) messenger)->frqc = data;
	tox_callback_friendrequest(((tox_jni_globals_t *) messenger)->tox,
			(void *) callback_friendrequest, data);
}

static void callback_friendrequest(uint8_t *pubkey, uint8_t *message,
		uint16_t length, void *ptr) {
	friendrequest_callback_t *data = ptr;
	jclass class = (*data->env)->GetObjectClass(data->env, data->jobj);
	jmethodID meth = (*data->env)->GetMethodID(data->env, class, "execute",
			"(Ljava/lang/String;Ljava/lang/String;)V");
	char buf[ADDR_SIZE_HEX] = { 0 };
	addr_to_hex(pubkey, buf);
	jstring _pubkey = (*data->env)->NewStringUTF(data->env, buf);
	jstring _message = (*data->env)->NewStringUTF(data->env, message);
	(*data->env)->CallVoidMethod(data->env, data->jobj, meth, _pubkey,
			_message);
}

JNIEXPORT void JNICALL Java_im_tox_jtoxcore_JTox_tox_1onfriendmessage(
		JNIEnv * env, jobject obj, jlong messenger, jobject callback) {
	friendmessage_callback_t *data = malloc(sizeof(friendmessage_callback_t));
	data->env = env;
	data->jobj = (*env)->NewGlobalRef(env, callback);
	(*env)->DeleteLocalRef(env, callback);
	((tox_jni_globals_t *) messenger)->frmc = data;
	tox_callback_friendmessage(((tox_jni_globals_t *) messenger)->tox,
			(void *) callback_friendmessage, data);

}

static void callback_friendmessage(Tox * tox, int friendnumber,
		uint8_t *message, uint16_t length, void *ptr) {
	friendmessage_callback_t *data = ptr;
	jclass class = (*data->env)->GetObjectClass(data->env, data->jobj);
	jmethodID meth = (*data->env)->GetMethodID(data->env, class, "execute",
			"(ILjava/lang/String;)V");
	jstring _message = (*data->env)->NewStringUTF(data->env, message);
	(*data->env)->CallVoidMethod(data->env, data->jobj, meth, friendnumber,
			_message);
}

JNIEXPORT void JNICALL Java_im_tox_jtoxcore_JTox_tox_1onaction(JNIEnv * env,
		jobject obj, jlong messenger, jobject callback) {
	action_callback_t *data = malloc(sizeof(action_callback_t));
	data->env = env;
	data->jobj = (*env)->NewGlobalRef(env, callback);
	(*env)->DeleteLocalRef(env, callback);
	((tox_jni_globals_t *) messenger)->ac = data;
	tox_callback_action(((tox_jni_globals_t *) messenger)->tox,
			(void *) callback_action, data);
}

static void callback_action(Tox * tox, int friendnumber, uint8_t *action,
		uint16_t length, void *ptr) {
	action_callback_t *data = ptr;
	jclass class = (*data->env)->GetObjectClass(data->env, data->jobj);
	jmethodID meth = (*data->env)->GetMethodID(data->env, class, "execute",
			"(ILjava/lang/String;)V");
	jstring _action = (*data->env)->NewStringUTF(data->env, action);
	(*data->env)->CallVoidMethod(data->env, data->jobj, meth, friendnumber,
			_action);
}

JNIEXPORT void JNICALL Java_im_tox_jtoxcore_JTox_tox_1onnamechange(JNIEnv * env,
		jobject obj, jlong messenger, jobject callback) {
	namechange_callback_t *data = malloc(sizeof(namechange_callback_t));
	data->env = env;
	data->jobj = (*env)->NewGlobalRef(env, callback);
	(*env)->DeleteLocalRef(env, callback);
	((tox_jni_globals_t *) messenger)->nc = data;
	tox_callback_namechange(((tox_jni_globals_t *) messenger)->tox,
			(void *) callback_namechange, data);
}

static void callback_namechange(Tox * tox, int friendnumber, uint8_t *newname,
		uint16_t length, void *ptr) {
	namechange_callback_t *data = ptr;
	jclass class = (*data->env)->GetObjectClass(data->env, data->jobj);
	jmethodID meth = (*data->env)->GetMethodID(data->env, class, "execute",
			"(ILjava/lang/String;)V");
	jstring _newname = (*data->env)->NewStringUTF(data->env, newname);
	(*data->env)->CallVoidMethod(data->env, data->jobj, meth, friendnumber,
			_newname);
}
