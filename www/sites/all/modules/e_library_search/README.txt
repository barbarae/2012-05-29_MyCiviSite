Readme file for the e-Library search module for Drupal
------------------------------------------------------

e-Library search provides libraries with an easy way to create blocks that 
provide a search form for thier users to search the online catalog.  

Many options are available to customize on  a per block basis.

Installation:
  Installation is like with all normal drupal modules:
  extract the 'e_library_search' folder from the tar ball to the
  modules directory from your website (typically sites/all/modules).

Dependencies:
  The basic e-Library search module has no dependencies, nothing special is 
  required however the PHP CURL library is STRONGLY recommended.

Supported Modules:
  There is support for i18n available in this module t allow for translation
  of user entered strings.

Conflicts/known issues:
  None at the time of initial release.

Main Configuration:
  The main configuration page is lcoated at admin/settings/elibrary/search
  where you must configure the e-Library search module.
  
  You must specify the base URL to your installation of e-Library, as well as
  the library Policy name.  The settings for these options can be found by 
  examining the URL used in an e-Library search.
  
  Other defaults provided will work for most libraries.

Block Configuration:
  Additional configuration options are available on a per block basis using
  the standard block configuration menu.  Defaults provided should work for 
  most libraries.