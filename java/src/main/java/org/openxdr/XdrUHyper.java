/**
 * Copyright 2010 Mark Aylett <mark.aylett@gmail.com>
 * 
 * The contents of this file are subject to the Common Development and
 * Distribution License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.sun.com/cddl/
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for
 * the specific language governing rights and limitations under the License.
 */
package org.openxdr;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public final class XdrUHyper {

    private XdrUHyper() {
    }

    public static void encode(ByteBuffer buf, long val) {
        assert ByteOrder.BIG_ENDIAN == buf.order();
        if (val < 0L)
            throw new IllegalArgumentException();
        buf.putLong(val);
    }

    public static long decode(ByteBuffer buf) {
        assert ByteOrder.BIG_ENDIAN == buf.order();
        final long val = buf.getLong();
        if (val < 0L)
            throw new IllegalArgumentException();
        return val;
    }

    public static final int SIZE = 8;

    public static final Codec<Long> CODEC = new Codec<Long>() {
        public final void encode(ByteBuffer buf, Long val) {
            XdrUHyper.encode(buf, val);
        }

        public final Long decode(ByteBuffer buf) {
            return XdrUHyper.decode(buf);
        }

        public final int size(Long val) {
            return SIZE;
        }
    };
}
