# Tcl package index file, version 1.1
# This file is generated by the "pkg_mkIndex" command
# and sourced either when an application starts up or
# by a "package unknown" script.  It invokes the
# "package ifneeded" command to set up package-related
# information so that packages will be loaded automatically
# in response to "package require" commands.  When this
# script is sourced, the variable $dir must contain the
# full path name of this file's directory.

package ifneeded cluster 1.1 "set env(CLUSTERPLUGINDIR) {$dir}; [list source [file join $dir cluster.tcl]]"
# $Id: pkgIndex.tcl,v 1.2.6.1 2005/05/09 21:12:04 erobert3 Exp $
