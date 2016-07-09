<?php
/**
 * @file
 * Batch functions.
 */

/**
 * Batch function for processing contrib and core modules.
 */
function kala_migrate_batch_files($path, $time, &$context) {
  // Update our progress information.
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
  }
  $context['sandbox']['progress']++;

  $atime = fileatime($path);
  if ($atime < $time) {
    $context['results'][$path] = $atime;
    $context['results']['kala-count'][] = $path;
  }

  // Message of winning.
  $context['message'] = t('Now assimilating %m', array('%m' => $path));
}

/**
 * Batch finished function.
 */
function kala_migrate_batch_files_finished($success, $results, $operations) {
  // go through success and grab what we need.
  $items = array();
  if ($success) {
    $message = format_plural(count($results['kala-count']), 'One module assimilated.', '@count modules assimilated.');
  }
  else {
    $message = t('Assimilation was futile!');
  }
  drupal_set_message($message);

  $filename = 'files.csv';
  header('Content-Type: application/csv');
  header('Content-Disposition: attachment; filename="' . $filename . '";');

  $f = fopen('php://output', 'w');

  $headers[0] = 'File Path';
  $headers[1] = 'File Access Time';

  // print headers to CSV
  fputcsv($f, $headers);

  foreach ($results as $key => $result) {
    if ($key != 'kala-count') {
      $body['File Path'] = $key;
      $body['File Access Time'] = $result;

      // throw it in the file meow.
      fputcsv($f, $body);
    }
  }

  // Close this and trill this out.
  fclose($f);
  // I can;t redirect here for some reason, header is already set.
  // Tried to do the zip, but we lose the file name.
  exit;
}
