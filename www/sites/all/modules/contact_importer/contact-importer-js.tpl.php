<?php

/**
 * @file
 * Default theme implementation to display a Contact Importer form in an full HTML document.
 *
 * @see contact_importer_preprocess_contact_importer_js()
 * @see contact_importer_process_contact_importer_js()
 */
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">

<head>
  <title><?php print $head_title; ?></title>
  <?php print $head; ?>
  <?php print $styles; ?>
  <?php print $scripts; ?>
</head>
<body class="<?php print $classes; ?>">
  <div id="contact-importer-js">
    <?php if (!empty($title)): ?><h1 class="title" id="page-title"><?php print $title; ?></h1><?php endif; ?>
    <?php if (!empty($messages)): print $messages; endif; ?>
    <div id="contact-importer-js-content" class="clearfix">
      <?php print $content; ?>
    </div> <!-- /content-content -->
  </div> <!-- /content -->

<?php print $footer; ?>

</body>
</html>
