
OpenInviter Engine
==================

The OpenInviter Engine provides integration with Contact Importer for the OpenInviter retrieval
library.


INSTALLATION
============

1) You can obtain the OpenInviter library from (Registration required):
    http://openinviter.com/download.php

   Use the "general" version, NOT the Drupal specific version (The Drupal specific version is
   useless).

2) Unpack the library into the sites/all/libraries folder. Your directory structure
   will look like the following:

    sites
    | + all
    | | + libraries
    | | | + OpenInviter
    | | | | + plugins
    | | | | | + [several php files]
    | | | | + openinviter.php
    | | | | + postinstall.php
    | | | | + [several other files]

3) The OpenInviter library works best if the webserver is able to write to all files and directories
   in the library.  To do this on *nix (Linux, OS X, Solaris, BSD, Unix, etc.):
    cd sites/all/libraries/OpenInviter
    chmod -R 777 .
   DO NOT do this to your entire Drupal install!

4) Run the postinstall.php script to verify that everything is setup properly. This sets up
   the OpenInviter/config.php file.  You can either run this script from the command line
    cd sites/all/libraries/OpenInviter
    php postinstall.php
   or from your browser
    http://example.com/sites/all/libraries/OpenInviter/postinstall.php

5) Remove the file postinstall.php
    rm postinstall.php


TROUBLESHOOTING
===============

Please review the installation instructions for the library:
  sites/all/libraries/OpenInviter/install.txt

For OpenInviter to function properly you will need to have PHP5 installed and either the PHP cURL
extension or the WGET command line utility.

If OpenInviter does not show up in the list at
Administration > Site Configuration > Contact Importer
Please be sure that you do not have the DCL Importer module installed.  Due to a naming conflict
you must remove all the files belonging to DCL Importer before the Contact Importer Open Inviter
Engine can be enabled.

Please note that currently social networks are not supported with the
OpenInviter library. See http://drupal.org/node/654638 for reasons why.
