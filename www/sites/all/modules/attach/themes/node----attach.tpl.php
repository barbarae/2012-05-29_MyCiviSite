<h3><?php print l($node->attach['default_title'], 'node/'. $node->nid) ?></h3>
<div class="attach-content">
<?php hide($content['links']);print render($content) ?>
</div>
