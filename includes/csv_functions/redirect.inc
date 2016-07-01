<?php
/**
 * Redirect Functions.
 */

/**
 * Redirect Processing function.
 *
 * @param  string $filename
 *   The file name for this function.
 *
 * @return string
 *   The file name for this function.
 */
function _kala_migrate_redirects($filename) {
  // Open The CSV
  $f = fopen($filename, 'w');
  // Clear out vars.
  $headers = $body = array();

  $headers[0] = 'Source';
  $headers[1] = 'Redirect';
  $headers[2] = 'Status Code';

  // print headers to CSV
  fputcsv($f, $headers);

  // Grab the redirects from the DB.
  $rids = db_query("SELECT rid from {redirect}")->fetchCol();
  // Load up all the redirect entities
  $redirects = redirect_load_multiple($rids);
  foreach ($redirects as $rid => $redirect) {
    // @see redirect_list_form in redirect.admin.inc
    $source_url = redirect_url($redirect->source, $redirect->source_options);
    $redirect_url = redirect_url($redirect->redirect, array_merge(
      $redirect->redirect_options, array('alias' => TRUE)
    ));

    $body['Source'] = $source_url;
    $body['Redirect'] = $redirect_url;
    $body['Status Code'] = $redirect->status_code ? $redirect->status_code : '301';

    // throw it in the file meow.
    fputcsv($f, $body);

  }
  // Close this and trill this out.
  fclose($f);

  // Return File Name
  return $filename;
}
