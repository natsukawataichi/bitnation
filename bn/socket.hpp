#pragma once

#include <bn/header.hpp>

struct NcSock;
typedef struct NcSock NcSock;

/**
 * dump parameters of socket to stream
 *
 * @param[in] self
 */
void
NcSock_Dump(const NcSock *self, FILE *fout);

/**
 * close socket
 * if self is NULL then don't anything
 *
 * @param[in] self
 *
 * @return success to number of zero
 * @return failed to under number of zero
 */
int32_t
NcSock_Close(NcSock *self);

/**
 * open socket by source and mode like a fopen(3)
 * the open modes are:
 *
 * 		tcp-client
 * 		tcp-server
 *
 * @param[in] src  format of "host:port" of C string
 * @param[in] mode open mode
 *
 * @return pointer to dynamic allocate memory of struct cap_socket
 */
NcSock *
NcSock_Open(const char *src, const char *mode);

/**
 * get host of C string by socket
 *
 * @param[in] self
 *
 * @return pointer to memory of C string
 */
const char *
NcSock_GetcHost(const NcSock *self);

/**
 * get port of C string by socket
 *
 * @param[in] self
 *
 * @return pointer to memory of C string
 */
const char *
NcSock_GetcPort(const NcSock *self);

/**
 * wrapper of accept(2)
 * get new socket by self socket
 *
 * @param[in] self
 *
 * @return pointer to dynamic allocate memory of struct cap_socket of client
 */
NcSock *
NcSock_Accept(NcSock *self);

/**
 * wrapper of recv(2)
 * recv string from socket
 *
 * @param[in] self
 * @param[in] dst pointer to memory of destination buffer
 * @param[in] dstsz number of size of destination buffer
 *
 * @return success to number of recv size
 * @return failed to number of under of zero
 */
int32_t
NcSock_RecvStr(NcSock *self, char *dst, int32_t dstsz);

int32_t
NcSock_Recv(NcSock *self, void *dst, int32_t dstsz);

/**
 * wrapper of send(2)
 * send string to socket
 *
 * @param[in] self
 * @param[in] str send C string
 *
 * @return success to number of send size
 * @return failed to number of under of zero
 */
int32_t
NcSock_SendStr(NcSock *self, const char *str);

/**
 * wrapper of send(2)
 * send bytes to socket
 *
 * @param[in] self
 * @param[in] bytes pointer to memory of bytes
 * @param[in] size number of size of bytes
 *
 * @return success to number of send size
 * @return failed to number of under of zero
 */
int32_t
NcSock_Send(NcSock *self, void *data, int32_t datasz);

/**
 * set error
 *
 * @param[in] *self
 * @param[in] *fmt
 */
void
NcSock_SetErr(NcSock *self, const char *fmt, ...);

/**
 * get error message
 *
 * @param[in] *self
 *
 * @return poitner to C strings
 */
const char *
NcSock_GetcErr(const NcSock *self);

/**
 * if NcSock has error then return true else return false
 *
 * @param[in] *self
 *
 * @return true or false
 */
bool
NcSock_HasErr(const NcSock *self);
