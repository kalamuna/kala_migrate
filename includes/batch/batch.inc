<?php
/**
 * @file
 * Batch function
 */


/*
 * Batch for ALL CMS function.
 */
function kala_migrate_batch($name, $blob, &$context) {
  // Update our progress information.
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
  }
  $context['sandbox']['progress']++;

  $url = 'http://www.drupal.org/project/' . $name;
  $checks = _kala_migrate_curl($url);
  list($check, $info) = $checks;

  // Is not a bad page
  if ($info['http_code']) {
    // check is all the happiness.
    // Add in info here and pass into CONTEXT
    // <div class="help">This module has been included with Drupal 8 core. Refer to this <a href="/node/2578705">issue</a> for more information.</div>
    if (stripos($check, 'view-project-release-download-table') !== FALSE) {
      // Grab View.
      $view = explode('<div class="view view-project-release-download-table', $check);

      // Start checking things n stuff.
      if (isset($view[1])) {
        // Check if in core.
        if (stripos($view[1], 'included with Drupal 8 core') !== FALSE) {
          $blob['drupal8status'] = 'IN CORE';
          $link = _kala_migrate_get_status_link($view[1]);
          $blob['drupal8issue'] = $link;
        }
      }
    }


    $context['results'][$name] = $blob;
    $context['results']['kala-count'][] = $url;
  }

  $context['message'] = t('Now assimilating %url', array('%url' => $url));
}

/*
 * Batch finished
 */
function kala_migrate_batch_finished($success, $results, $operations) {
  // The 'success' parameter means no fatal PHP errors were detected. All
  // other error management should be handled using 'results'.
  $items = array();
  if ($success) {
    $message = format_plural(count($results['kala-count']), 'One module assimilated.', '@count modules assimilated.');
  }
  else {
    $message = t('Assimilation was futile!');
  }
  drupal_set_message($message);

  foreach ($results as $key => $result) {
    if ($key != 'kala-count') {
      $info = serialize($result);
      $data = array(
        'name' => $key,
        'info' => $info,
      );
      // Is there a way to write this outsideof the foreach?
      drupal_write_record('system', $data, 'name');
    }
  }

  // Providing data for the redirected page is done through $_SESSION.
  drupal_goto('admin/config/development/kala_migrate/module');
}

/**
 * Curl function for all magix happen.
 *
 * @param string $url
 *   The url in question
 *
 * @return object
 *   curl response + info
 */
function _kala_migrate_curl($url) {
  // Many tries and headaches but this curl gets em all
  $agent = 'Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.0.3705; .NET CLR 1.1.4322)';
  $ch = curl_init();
  curl_setopt($ch, CURLOPT_URL, $url);
  curl_setopt($ch, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
  curl_setopt($ch, CURLOPT_ENCODING,  '');
  curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 2);
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
  curl_setopt($ch, CURLOPT_BINARYTRANSFER, true);
  curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
  curl_setopt($ch, CURLOPT_TIMEOUT_MS, 5000);
  curl_setopt($ch, CURLOPT_USERAGENT, $agent);
  $check = curl_exec($ch);
  $info = curl_getinfo($ch);
  curl_close($ch);

  $result = array($check, $info);
  return $result;
}

/**
 * String manipulator to return D8 progress link.
 *
 * @param string $link
 *   The string to manipulate.
 *
 * @return string
 *   The url of the issue.
 */
function _kala_migrate_get_status_link($link) {
  $output = stristr($link, ' for more information.', TRUE);
  $output = stristr($output, 'a href="');
  $href = explode('"', $output);
  if (isset($href[1])) {
    return 'http://www.drupal.org' . $href[1];
  }
}
