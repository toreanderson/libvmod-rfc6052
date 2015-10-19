AM_CPPFLAGS = @VMOD_INCLUDES@ -Wall -Werror

vmoddir = @VMOD_DIR@
vmod_LTLIBRARIES = libvmod_rfc6052.la

libvmod_rfc6052_la_LDFLAGS = -module -export-dynamic -avoid-version -shared

libvmod_rfc6052_la_SOURCES = \
	vmod_rfc6052.c

nodist_libvmod_rfc6052_la_SOURCES = \
	vcc_if.c \
	vcc_if.h

vmod_rfc6052.lo: vcc_if.c vcc_if.h

vcc_if.c: vcc_if.h

vcc_if.h: @VMODTOOL@ $(top_srcdir)/src/vmod_rfc6052.vcc
	@VMODTOOL@ $(top_srcdir)/src/vmod_rfc6052.vcc

VMOD_TESTS = $(top_srcdir)/src/tests/*.vtc
.PHONY: $(VMOD_TESTS)

$(top_srcdir)/src/tests/*.vtc: libvmod_rfc6052.la
	@VARNISHTEST@ -Dvarnishd=@VARNISHD@ -Dvmod_topbuild=$(abs_top_builddir) $@

check: $(VMOD_TESTS)

EXTRA_DIST = \
	vmod_rfc6052.vcc \
	$(VMOD_TESTS)

CLEANFILES = \
	$(builddir)/vcc_if.c \
	$(builddir)/vcc_if.h \
	$(builddir)/vmod_rfc6052.rst \
	$(builddir)/vmod_rfc6052.man.rst
