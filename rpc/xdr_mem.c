/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/*
 * Copyright 2005 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */
/* Copyright (c) 1983, 1984, 1985, 1986, 1987, 1988, 1989 AT&T */
/* All Rights Reserved */
/*
 * Portions of this source code were derived from Berkeley
 * 4.3 BSD under license from the Regents of the University of
 * California.
 */

/*
 * Portions Copyright 2010 Mark Aylett <mark.aylett@gmail.com>
 */

/*
 * xdr_mem.h, XDR implementation using memory buffers.
 *
 * If you have some data to be interpreted as external data representation
 * or to be converted to external data representation in a memory buffer,
 * then this is the package for you.
 */
#define XDR_BUILD
#include "rpc/xdr.h"

#include <memory.h>

static struct xdr_ops *xdrmem_ops(const caddr_t);

/*
 * Meaning of the private areas of the xdr struct for xdr_mem
 * 	x_base : Base from where the xdr stream starts
 * 	x_private : The current position of the stream.
 * 	x_handy : The size of the stream buffer.
 */

/*
 * The procedure xdrmem_create initializes a stream descriptor for a
 * memory buffer.
 */
XDR_API void
xdrmem_create(XDR *xdrs, const caddr_t addr, const u_int size,
							const enum xdr_op op)
{
	caddr_t eaddr = addr;

	xdrs->x_op = op;
	xdrs->x_ops = xdrmem_ops(addr);
	xdrs->x_private = xdrs->x_base = 0;
	/*
	 * We check here that the size is with in the range of the
	 * address space. If not we set x_handy to zero. This will cause
	 * all xdrmem entry points to fail.
	 */
	eaddr = addr + size;

	if (eaddr < addr)
		xdrs->x_handy = 0;
	else {
		xdrs->x_handy = size;
		xdrs->x_private = xdrs->x_base = addr;
	}
}

/* ARGSUSED */
static void
xdrmem_destroy(XDR *xdrs)
{
}

static bool_t
xdrmem_getlong(XDR *xdrs, long *lp)
{
	if (sizeof (int32_t) > (uint32_t)xdrs->x_handy) {
		xdrs->x_private += (u_int)xdrs->x_handy;
		xdrs->x_handy = 0;
		return (FALSE);
	}
	xdrs->x_handy -= sizeof (int32_t);
	/* LINTED pointer cast */
	*lp = (int32_t)xdr_ntoh32((uint32_t)(*((int32_t *)(xdrs->x_private))));
	xdrs->x_private += sizeof (int32_t);
	return (TRUE);
}

static bool_t
xdrmem_getlong_unaligned(XDR *xdrs, long *lp)
{
	int32_t i;

	if (sizeof (int32_t) > (uint32_t)xdrs->x_handy) {
		xdrs->x_private += (u_int)xdrs->x_handy;
		xdrs->x_handy = 0;
		return (FALSE);
	}
	xdrs->x_handy -= sizeof (int32_t);
	memcpy(&i, xdrs->x_private, sizeof(int32_t));
	*lp = (int32_t)xdr_ntoh32((uint32_t)i);
	xdrs->x_private += sizeof (int32_t);
	return (TRUE);
}

static bool_t
xdrmem_putlong(XDR *xdrs, long *lp)
{
	if ((*lp > INT32_MAX) || (*lp < INT32_MIN))
		return (FALSE);

	if ((sizeof (int32_t) > (uint32_t)xdrs->x_handy)) {
		xdrs->x_private += (u_int)xdrs->x_handy;
		xdrs->x_handy = 0;
		return (FALSE);
	}
	xdrs->x_handy -= sizeof (int32_t);
	/* LINTED pointer cast */
	*(int32_t *)xdrs->x_private = (int32_t)xdr_hton32((uint32_t)(*lp));
	xdrs->x_private += sizeof (int32_t);
	return (TRUE);
}

static bool_t
xdrmem_putlong_unaligned(XDR *xdrs, long *lp)
{
	int32_t i;

	if ((*lp > INT32_MAX) || (*lp < INT32_MIN))
		return (FALSE);

	if ((sizeof (int32_t) > (uint32_t)xdrs->x_handy)) {
		xdrs->x_private += (u_int)xdrs->x_handy;
		xdrs->x_handy = 0;
		return (FALSE);
	}
	xdrs->x_handy -= sizeof (int32_t);
	i = (int32_t)xdr_hton32((uint32_t)(*lp));
	memcpy(xdrs->x_private, &i, sizeof(int32_t));
	xdrs->x_private += sizeof (int32_t);
	return (TRUE);
}

static bool_t
xdrmem_getint32(XDR *xdrs, int32_t *ip)
{
	if (sizeof (int32_t) > (u_int)xdrs->x_handy) {
		xdrs->x_private += (u_int)xdrs->x_handy;
		xdrs->x_handy = 0;
		return (FALSE);
	}
	xdrs->x_handy -= sizeof (int32_t);
	/* LINTED pointer cast */
	*ip = (int32_t)xdr_ntoh32((uint32_t)(*((int32_t *)(xdrs->x_private))));
	xdrs->x_private += sizeof (int32_t);
	return (TRUE);
}

static bool_t
xdrmem_getint32_unaligned(XDR *xdrs, int32_t *ip)
{
    int32_t i;

	if (sizeof (int32_t) > (u_int)xdrs->x_handy) {
		xdrs->x_private += (u_int)xdrs->x_handy;
		xdrs->x_handy = 0;
		return (FALSE);
	}
	xdrs->x_handy -= sizeof (int32_t);
	memcpy(&i, xdrs->x_private, sizeof(int32_t));
	*ip = (int32_t)xdr_ntoh32((uint32_t)i);
	xdrs->x_private += sizeof (int32_t);
	return (TRUE);
}

static bool_t
xdrmem_putint32(XDR *xdrs, int32_t *ip)
{
	if (sizeof (int32_t) > (uint32_t)xdrs->x_handy) {
		xdrs->x_private += (u_int)xdrs->x_handy;
		xdrs->x_handy = 0;
		return (FALSE);
	}
	xdrs->x_handy -= sizeof (int32_t);
	/* LINTED pointer cast */
	*(int32_t *)xdrs->x_private = (int32_t)xdr_hton32((uint32_t)(*ip));
	xdrs->x_private += sizeof (int32_t);
	return (TRUE);
}

static bool_t
xdrmem_putint32_unaligned(XDR *xdrs, int32_t *ip)
{
    int32_t i;

	if (sizeof (int32_t) > (uint32_t)xdrs->x_handy) {
		xdrs->x_private += (u_int)xdrs->x_handy;
		xdrs->x_handy = 0;
		return (FALSE);
	}
	xdrs->x_handy -= sizeof (int32_t);
	i = (int32_t)xdr_hton32((uint32_t)(*ip));
	memcpy(xdrs->x_private, &i, sizeof(int32_t));
	xdrs->x_private += sizeof (int32_t);
	return (TRUE);
}

static bool_t
xdrmem_getbytes(XDR *xdrs, caddr_t addr, int len)
{
	if ((uint32_t)len > (uint32_t)xdrs->x_handy) {
		xdrs->x_private += (u_int)xdrs->x_handy;
		xdrs->x_handy = 0;
		return (FALSE);
	}
	xdrs->x_handy -= len;
	(void) memcpy(addr, xdrs->x_private, (u_int)len);
	xdrs->x_private += (u_int)len;
	return (TRUE);
}

static bool_t
xdrmem_putbytes(XDR *xdrs, caddr_t addr, int len)
{
	if ((uint32_t)len > (uint32_t)xdrs->x_handy) {
		xdrs->x_private += (u_int)xdrs->x_handy;
		xdrs->x_handy = 0;
		return (FALSE);
	}
	xdrs->x_handy -= len;
	(void) memcpy(xdrs->x_private, addr, (u_int)len);
	xdrs->x_private += (u_int)len;
	return (TRUE);
}

static u_int
xdrmem_getpos(XDR *xdrs)
{
	return (u_int)((uintptr_t)xdrs->x_private - (uintptr_t)xdrs->x_base);
}

static bool_t
xdrmem_setpos(XDR *xdrs, u_int pos)
{
	caddr_t newaddr = xdrs->x_base + pos;
	caddr_t lastaddr = xdrs->x_private + (u_int)xdrs->x_handy;

	if ((long)newaddr > (long)lastaddr)
		return (FALSE);
	xdrs->x_private = newaddr;
	xdrs->x_handy = (int)((uintptr_t)lastaddr - (uintptr_t)newaddr);
	return (TRUE);
}

static rpc_inline_t *
xdrmem_inline(XDR *xdrs, int len)
{
	rpc_inline_t *buf = 0;

	if ((uint32_t)xdrs->x_handy >= (uint32_t)len) {
		xdrs->x_handy -= len;
		/* LINTED pointer cast */
		buf = (rpc_inline_t *)xdrs->x_private;
		xdrs->x_private += (u_int)len;
	}
	return (buf);
}

static rpc_inline_t *
xdrmem_inline_unaligned(XDR *xdrs, int len)
{
    /*
     * Inline operations not available on unaligned memory.
     */
	return NULL;
}

static bool_t
xdrmem_control(XDR *xdrs, int request, void *info)
{
	xdr_bytesrec *xptr;

	switch (request) {
	case XDR_GET_BYTES_AVAIL:
		xptr = (xdr_bytesrec *) info;
		xptr->xc_is_last_record = TRUE;
		xptr->xc_num_avail = xdrs->x_handy;
		return (TRUE);
	default:
		return (FALSE);

	}

}

static struct xdr_ops ops_aligned = {
    xdrmem_getlong,
    xdrmem_putlong,
    xdrmem_getbytes,
    xdrmem_putbytes,
    xdrmem_getpos,
    xdrmem_setpos,
    xdrmem_inline,
    xdrmem_destroy,
    xdrmem_control,
    xdrmem_getint32,
    xdrmem_putint32
};

static struct xdr_ops ops_unaligned = {
    xdrmem_getlong_unaligned,
    xdrmem_putlong_unaligned,
    xdrmem_getbytes,
    xdrmem_putbytes,
    xdrmem_getpos,
    xdrmem_setpos,
    xdrmem_inline_unaligned,
    xdrmem_destroy,
    xdrmem_control,
    xdrmem_getint32_unaligned,
    xdrmem_putint32_unaligned
};

static struct xdr_ops *
xdrmem_ops(const caddr_t addr)
{
	return (size_t)addr & (ALIGNOF_INT32_T - 1)
        ? &ops_unaligned : &ops_aligned;
}
