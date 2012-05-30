Datasources
-----------

Datasources provides a framework to describe datasource to import in Drupal.


Usage
-----
IT IS NOT A STANDALONE MODULE.

You have to own module to add new datasources.
Once you have defined your datasources, the module provides an UI to map datasource fields against entities fields and define import policies.

Features
--------
 - find/update/insert managment with UDID (Unique (by) datasource identifier)
 - sharable preprocessors to do some field level transformation
 - basic support for list field
 - on top of Entity API
 

Quickstart
----------

You can use contrib/datasources_dummy as a template for your own datasource modules.

Use hook_udid_datasources() to define a functional key (see api.php).

Use hook_datasource_inputs() to register new datasources (fields and key, see api.php).

Install you module and go to datasources settings to map your datasourcewith your objects.

Use the drush command dimp to import your data.
