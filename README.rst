============
vmod_rfc6052
============

----------------------
RFC6052 Varnish Module
----------------------

:Date: 2015-10-19
:Version: 0.1
:Manual section: 3

SYNOPSIS
========

import rfc6052;

DESCRIPTION
===========

Implements a set of functions for dealing with RFC6052 IPv4-embedded IPv6
addresses in VCL.

FUNCTIONS
=========

extract
-------

Prototype
        ::

                extract(IP ip)
Return value
        The IPv4 address embedded in "ip", or NULL.
Description
        Extracts the embedded IPv4 address from "ip" and returns it.
Example
        ::

                set req.http.X-Forwarded-For = rfc6052.extract(client.ip);

is_v4embedded
-------------

Prototype
        ::

                is_v4embedded(IP ip)
Return value
        Boolean.
Description
        Used to check whether or not "ip" is an IPv4-embedded IPv6 or not.
Example
        ::

                if(rfc6052.is_v4embedded(client.ip)) {
                        /* client.ip contains an IPv4-embedded IPv6 address */
                }

prefix
------

Prototype
        ::

                prefix(STRING prefix)
Return value
        None.
Description
        Sets the RFC6052 translation prefix used. Default: 64:ff9b::/96.
        Note: Only /96 supported. Do _not_ include the netmask.
Example
        ::

                # Set the RFC6052 translation prefix to 2001:db8::/96
                prefix("2001:db8:64::");

replace
-------

Prototype
        ::

                replace(IP ip)
Return value
        None.
Description
        Extracts the embedded IPv4 address from "ip", and uses it to perform an
        in-place replacement of "ip". No replacement is performed if "ip"
        didn't contain an IPv4-embedded IPv6 address to begin with.
Example
        ::

                rfc6052.replace(client.ip);
                # client.ip now contains the embedded IPv4 address

INSTALLATION
============

The source tree is based on autotools to configure the building, and
does also have the necessary bits in place to do functional unit tests
using the ``varnishtest`` tool.

Building requires the Varnish header files and uses pkg-config to find
the necessary paths.

Usage::

 ./autogen.sh
 ./configure

If you have installed Varnish to a non-standard directory, call
``autogen.sh`` and ``configure`` with ``PKG_CONFIG_PATH`` pointing to
the appropriate path. For rfc6052, when varnishd configure was called
with ``--prefix=$PREFIX``, use

 PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig
 export PKG_CONFIG_PATH

Make targets:

* make - builds the vmod.
* make install - installs your vmod.
* make check - runs the unit tests in ``src/tests/*.vtc``
* make distcheck - run check and prepare a tarball of the vmod.

Installation directories
------------------------

By default, the vmod ``configure`` script installs the built vmod in
the same directory as Varnish, determined via ``pkg-config(1)``. The
vmod installation directory can be overridden by passing the
``VMOD_DIR`` variable to ``configure``.

Other files like man-pages and documentation are installed in the
locations determined by ``configure``, which inherits its default
``--prefix`` setting from Varnish.

USAGE EXAMPLE
=============

In your VCL you could then use this vmod along the following lines::

        import rfc6052;

        sub vcl_init {
            # Set a custom RFC6052 translation prefix
            rfc6052.prefix("2001:db8:64::");
        }

        # Method A: Leave client.ip intact
        sub vcl_recv {
            if(rfc6052.is_v4embedded(client.ip)) {
                set req.http.X-Forwarded-For = rfc6052.extract(client.ip);
            } else {
                set req.http.X-Forwarded-For = client.ip;
            }
        }

        # Method B: Replace contents of client.ip
        sub vcl_recv {
            rfc6052.replace(client.ip);
            set req.http.X-Forwarded-For = client.ip;
        }

COMMON PROBLEMS
===============

* configure: error: Need varnish.m4 -- see README.rst

  Check if ``PKG_CONFIG_PATH`` has been set correctly before calling
  ``autogen.sh`` and ``configure``

* Incompatibilities with different Varnish Cache versions

  This VMOD is only tested to work with Varnish Cache 4.1.
