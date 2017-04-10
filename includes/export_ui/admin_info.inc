<?php
/**
 * @file
 * Kala Migrate Info Tab UI.
 */

/**
 * Info Page Callback.
 */
function kala_migrate_info() {
  $html  = '<h2>' . t('Kala Migrate Options') . '</h2>';
  $html .= '<ol>';
  $html .= '<li>' . t('<b>MODULES STATUS:</b> Module list for exporting to D8.') . '</li>';
  $html .= '</ol>';

  return $html;
}
