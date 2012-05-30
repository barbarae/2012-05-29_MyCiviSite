<div class="description">
  <?php print check_plain($element['#description']); ?>
</div>
<table id="pollpanel">
  <tr>
    <th></th>
    <?php foreach ($options as $o): ?>
    <th colspan="<?php print count($o['options']); ?>" class="bleft"><?php print $o['day']; ?></th>
    <?php endforeach; ?>
  </tr>
  <tr>
    <td></td>
    <?php foreach ($options as $o): ?>
    <?php foreach ($o['options'] as $id => $data): ?>
      <th class="option ct"><?php print $data; ?></th>
      <?php endforeach; ?>
    <?php endforeach; ?>
  </tr>

  <?php if ($element['#attributes']['secure'] == 0): ?>
  <?php foreach ($votes as $username => $user_vote): ?>
    <tr>
      <td class="ct"><strong><?php print $username ?></strong></td>
      <?php foreach ($user_vote as $id => $value): ?>
      <td class="ct voted_<?php print $value; ?>"><?php print $choices[$value]; ?></td>
      <?php endforeach; ?>
    </tr>
    <?php endforeach; ?>
  <?php else: ?>
  <tr>
    <td colspan="<?php print $total_count + 1; ?>">
      <?php print t("You're not allowed to see the previous votes."); ?>
    </td>
  </tr>
  <?php endif; ?>

  <tr>
    <td>
      <?php print t('Enter your name:'); ?><?php print $input ?>
    </td>
    <?php foreach ($options as $o): ?>
    <?php foreach ($o['options'] as $id => $data): ?>
      <?php if ($element['#attributes']['maybe_option'] == 1): ?>
        <td>
          <?php foreach ($choices as $cid => $choice): ?>
          <input type="radio" name="option_<?php print $id; ?>"
                 value="<?php print $cid; ?>"><?php print $choice; ?><br/>
          <?php endforeach; ?>
        </td>
        <?php else: ?>
        <?php if ($element['#attributes']['maybe_option'] == 1): ?>
          <td class="ct"><input type="radio" name="option_<?php print $day_id; ?> value="<?php print $id; ?>"></td>
          <?php else: ?>
          <td class="ct"><input type="checkbox" name="option_<?php print $id; ?>" value="1"></td>
          <?php endif; ?>
        <?php endif; ?>
      <?php endforeach; ?>
    <?php endforeach; ?>
  </tr>
  
  <tr>
    <?php if ($element['#attributes']['secure'] == 0): ?>
    <td class="ct">
      <strong><?php print t("Summary:"); ?></strong>
    </td>
    <?php if (count($votes_stats) > 0): ?>
      <?php foreach ($votes_stats as $id => $stats): ?>
        <td class="ct <?php print $stats['style']; ?>">
            <?php print t("Yes: @num", array("@num" => $stats['yes'])); ?><br/>
            <?php print t("No: @num", array("@num" => $stats['no'])); ?><br/>
          <?php if ($element['#attributes']['maybe_option'] == 1): ?>
            <?php print t("Maybe: @num", array("@num" => $stats['maybe'])); ?><br/>
          <?php endif; ?>
        </td>
        <?php endforeach; ?>
      <?php else: ?>
      <td colspan="<?php print $total_count; ?>"><?php print t("No votes yet."); ?></td>
      <?php endif; ?>
    <?php endif; ?>
  </tr>
</table>