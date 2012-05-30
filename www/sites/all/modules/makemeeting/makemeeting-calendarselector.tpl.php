<fieldset>
  <legend><?php echo t('Select date'); ?></legend>
  <table width="100%">
    <tr>
      <td>
    <span class="jcalendar">
      <div class="jcalendar-selects">
        <select name="day" id="day" class="jcalendar-select-day">
          <option value="0"></option>
          <?php for ($i = 1; $i <= 31; $i++): ?>
          <option value="<?php print $i ?>"><?php print $i ?></option>
          <?php endfor; ?>
        </select>
        <select name="month" id="month" class="jcalendar-select-month">
          <option value="0"></option>
          <?php foreach ($element['#attributes']['months'] as $i => $month): ?>
          <option value="<?php print $i + 1 ?>"><?php print $month ?></option>
          <?php endforeach; ?>
        </select>
        <select name="year" id="year" class="jcalendar-select-year">
          <option value="0"></option>
          <?php for ($i = date("Y"); $i <= date("Y") + 10; $i++): ?>
          <option value="<?php print $i ?>"><?php print $i ?></option>
          <?php endfor; ?>
        </select>
      </div>
    </span>
      </td>
    </tr>
    <tr>
      <td>
        <?php echo t('Selected date(s):'); ?>
        <div class="description"><?php echo $element['#description']; ?></div>
        <div id="selected_dates">
          <table>
            <?php
                      if (is_array($element["#attributes"]['selected_dates_and_options'])) {
            $arr = $element["#attributes"]['selected_dates_and_options'];
            $rowCount = 1;
            $rows_num = sizeof($arr) / 2;
            $days = array();
            for ($i = 0; $i < $rows_num; $i++) {
              $collCount = 1;
              echo "<tr id=\"jcalendar_row_" . $rowCount . "\">
        <td>[<a href=\"javascript:jcalendar_remove_row('" . $rowCount . "')\">X</a>]&nbsp;<span id=\"daystr_" . $rowCount . "\">" . $arr['date_' . $rowCount] . "</span><input name=\"day_" . $rowCount . "\" value=\"" . $arr['date_' . $rowCount] . "\" type=\"hidden\">:</td> \n";

              foreach ($arr['option_' . $rowCount] as $option) {
                $suffix_str = $rowCount . "_" . $collCount++;
                echo "\n<td id=\"jcell_" . $suffix_str . "\">
            <input name=\"day_option_" . $suffix_str . "\" id=\"jtfield_" . $suffix_str . "\" value=\"" . $option . "\" type=\"text\">
          </td> \n";
              }

              $days[] = $arr['date_' . $rowCount];

              echo "</tr> \n";
              $rowCount++;
            }

            // set javascript variables
            echo "
    <script>
      counter = " . $rowCount . ";
      rowCounter = " . ($rowCount - 1) . ";
      collCounter = " . ($collCount - 1) . ";";
            foreach ($days as $day) {
              echo "addedDates['" . $day . "'] = 1;";
            }
            echo "
    </script>";
          }
            ?>
          </table>
        </div>
        <div id="selected_navlinks">
          <ul>
            <li><a href="javascript:jcalendar_add_column()"><?php echo t('Add column'); ?></a></li>
            <li><a href="javascript:jcalendar_remove_column()"><?php echo t('Remove last column'); ?></a></li>
            <li><a href="javascript:jcalendar_copy_first()"><?php echo t('Copy first row'); ?></a></li>
          </ul>
        </div>
      </td>
    </tr>
  </table>
</fieldset>

