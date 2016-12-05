<?php
/**
 * @file
 * Kala Migrate CSV Export Tab UI & Functions.
 */

/**
 * File List Form / Page Callback.
 */
function kala_migrate_file_page_form() {

  $form['ef'] = array(
    '#type' => 'fieldset',
    '#title' => t('Export Files By Access Time'),
  );

  $form['hey'] = array(
    '#markup' => '<h2>I am still working on this part, it may not work properly in its current condition</h2>',
  );

  $desc = t("Describe a time by reference to the current day, like '-90 days' (90 days ago).  See !strtotime for more details.", array('!strtotime' => l(t('strtotime'), 'http://www.php.net/manual/en/function.strtotime.php')));

  $form['ef']['access'] = array(
    '#type' => 'textfield',
    '#title' => t('File Last Access Time.'),
    '#description' => $desc,
    '#default_value' => '-1 Year',
  );

  $form['#submit'][] = 'kala_migrate_file_page_form_submit';

  return system_settings_form($form);
}

/**
 * Submit handler for csv exports form.
 */
function kala_migrate_file_page_form_submit($form, &$form_state) {

  $value = $form_state['values']['access'];

  $path = variable_get('file_public_path', 'sites/default/files');
  $dir = new RecursiveDirectoryIterator($path, RecursiveDirectoryIterator::SKIP_DOTS);
  $iterator = new RecursiveIteratorIterator($dir);
  foreach ($iterator as $file) {
    $operations[] = array('kala_migrate_batch_files', array($file->getPathname(), $value));
  }

  // Set Batch fun stuff.
  $batch = array(
    'title' => t('Assimilating...'),
    'operations' => $operations,
    'finished' => 'kala_migrate_batch_files_finished',
  );

  // Engage.
  batch_set($batch);
}
