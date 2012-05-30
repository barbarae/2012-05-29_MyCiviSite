Git Hook
--------

DEVELOPING for Git Hook
-----------------------

Githook event plugins are currently limited to the githook/plugins directory.
Any *.inc file in that directory is loaded, as is any *.<current event>.inc file 
for events matching the one githook-event was invoked with.

Loading these files provides them the option to implement one of the githook
hooks (see githook.api.php), which in turn allows them to use drush_set_error()
to declare an error state.

If githook declares an error state for *any reason*, git will pick up on that
and act accordingly if the current event has validation/action denied powers.

FAQ
---

How do I skip the validation and just commit my work?

  If you need to commit your work, and do not care to have it checked for errors,
  commit your code with:

  $> git commit --no-verify

How do I debug my githook action implementation?

  Set all githook events into debug mode by running

  $> drush githook --install --set-debug


