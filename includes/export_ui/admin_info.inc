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
  $html .= '<li>' . t('<b>EXPORT CSV:</b> Export all structural and architectural items for the site into multiple csv\'s contained in ZIP file.') . '</li>';
  $html .= '<li>' . t('<b>MODULES STATUS:</b> Module list for exporting to D8.') . '</li>';
  $html .= '<li>' . t('<b>FILE LIST:</b> Generate a CSV of files by last access date. (STILL UNDER DEV, WILL BE UNSTABLE)') . '</li>';
  $html .= '</ol>';

  return array('#markup' => $html);
}
