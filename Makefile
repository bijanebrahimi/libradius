# Copyright 2021 Bijan Ebrahimi <bijanebrahimi@riseup.net>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.


WARNS?=		3

LIB = 		radius.so
SRCS =		radlib.c
OBJS =		radlib.o
PREFIX = 	/usr

CC ?=		cc
CFLAGS +=	-Wall -I/usr/include/openssl
LDFLAGS +=	-lcrypto -shared

.PHONY: all

all: $(LIB)

$(LIB): $(OBJS) radlib_private.h radlib.h radlib_vs.h
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: install
install:
	install -o root -g root -m 0655 $(LIB) $(PREFIX)/lib/libradius.so
	install -o root -g root -m 0655 radlib.h $(PREFIX)/include/

.PHONTY: clean
clean:
	rm -rf $(LIB) $(OBJS) example
