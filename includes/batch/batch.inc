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

  // Do current core files different then contrib.
  if ($name == 'Core') {
    // Grab module machine name special like since it isn't there.
    if (isset($blob['files']) && count($blob['files']) > 0) {
      foreach ($blob['files'] as $file) {
        if (stripos($file, '/') !== FALSE) {
          $file = trim(stristr($file, '/'), '/');
        }
        $file = stristr($file, '.', TRUE);
        break;
      }
    }
    else {
      // Ditch effort to try and grab the module name.
      $file = strtolower($blob['name']);
    }

    $contrib_now = array('blog','dashboard','openid','poll','trigger');
    if (in_array($file, $contrib_now)) {
      $blob['drupal8status'] = 'MOVED TO CONTRIB';
      $blob['drupal8issue'] = 'http://www.drupal.org/project/' . $file;
    }

    $removed = array('overlay', 'php');
    if (in_array($file, $removed)) {
      $blob['drupal8status'] = 'REMOVED FOR GOOD';
      $blob['drupal8issue'] = 'REMOVED FOR GOOD';
    }

    $special = array('profile', 'tensile');
    if (in_array($file, $special)) {
      switch ($file) {
        case 'profile':
          $blob['drupal8status'] = 'MIGRATED TO PROFILE2';
          $blob['drupal8issue'] = 'http://www.drupal.org/project/profile2';
          break;

        case 'tensile':
          $blob['drupal8status'] = 'MIGRATED TO GITHUB';
          $blob['drupal8issue'] = 'http://github.com/lewisnyman/tensile';
          break;
      }
    }

    // Set this to be won later.
    $msg = 'CORE MODULE - ' . $file;
    $context['results'][$file] = $blob;
    $context['results']['kala-count'][] = $url;
  }
  else {
    // Check contrib modules.
    $msg = 'CONTRIB MODULE - ' . $name;
    $url = 'http://www.drupal.org/project/' . $blob['project'];
    $checks = _kala_migrate_curl($url);
    list($check, $info) = $checks;

    // Is not a bad page
    if ($info['http_code'] && !isset($blob['submodule'])) {
      // Go through the release table info.
      if (stripos($check, 'view-project-release-download-table') !== FALSE) {
        // Grab Help Text
        $help = _kala_migrate_get_between($check, '<div class="help">', '</div>');

        // Start checking things n stuff.
        if (!empty($help)) {
          // Check if in core.
          if (stripos($help, 'included with Drupal 8 core') !== FALSE) {
            $blob['drupal8status'] = 'IN CORE';
            $blob['drupal8ver'] = 'N/A';
            $link = _kala_migrate_get_status_link($help);
            $blob['drupal8issue'] = !empty($link) ? $link : '';
          }
          // Renamed or depreciated.
          if (stripos($help, 'module has been renamed or deprecated') !== FALSE) {
            $blob['drupal8status'] = 'RENAMED OR DEPRECIATED';
            $blob['drupal8ver'] = 'N/A';
            $link = _kala_migrate_get_status_link($help);
            $blob['drupal8issue'] = !empty($link) ? $link : '';
          }
          // Not been ported.
          if (stripos($help, 'module has not been ported') !== FALSE) {
            $version = _kala_migrate_grab_version($check);
            $blob['drupal8status'] = empty($version) ? 'NOT PORTED YET' : 'IN PROGRESS';
            $blob['drupal8ver'] = !empty($version) ? $version : 'NONE';
            $link = _kala_migrate_get_status_link($help);
            $blob['drupal8issue'] = !empty($link) ? $link : '';
          }
          // Porting now.
          if (stripos($help, 'currently being ported') !== FALSE) {
            $blob['drupal8status'] = 'IN PROGRESS';
            $version = _kala_migrate_grab_version($check);
            $blob['drupal8ver'] = !empty($version) ? $version : 'NONE';
            $link = _kala_migrate_get_status_link($help);
            $blob['drupal8issue'] = !empty($link) ? $link : '';
          }
        }
      }

      // Set this to be won later.
      $context['results'][$name] = $blob;
      $context['results']['kala-count'][] = $url;
    }
  }

  // Message of winning.
  $context['message'] = t('Now assimilating module %m', array('%m' => $msg));
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
 * Helper function for getting text between 2 identified strings.
 * Can be easier then using regex and preg type functions.
 *
 * @param  string $content
 *   String we are checking.
 *
 * @param  string $start
 *   Starting Position of check.
 *
 * @param  string $end
 *   End Position of check
 *
 * @return string
 *   What was in between the 2 or empty.
 */
function _kala_migrate_get_between($content, $start, $end) {
  // Explode this and let's find our stuff.
  $r = explode($start, $content);
  if (isset($r[1])){
    $r = explode($end, $r[1]);
    return $r[0];
  }
  // Return empty of nothing can be found.
  return '';
}

/**
 * String manipulator to return D8 progress link.
 *
 * @param string $help
 *   The string to manipulate.
 *
 * @return string
 *   The url of the issue.
 */
function _kala_migrate_get_status_link($help) {
  // Run href and return it properly formatted.
  $link = _kala_migrate_get_between($help, '<a href="', '">');
  return !empty($link) ? 'http://www.drupal.org' . $link : '';
}

/**
 * Try and Grab D8 Verision.
 *
 * @param string $check
 *   The string to manipulate.
 *
 * @return string
 *   The version number or empty if none found.
 */
function _kala_migrate_grab_version($check) {
  // Grab the downloads section
  $dloads = stristr($check, '<h3>Downloads</h3>');
  // Look through the table for the version.
  $tds = explode('<td', $dloads);
  if (isset($tds[1])) {
    foreach ($tds as $td) {
      // Grab all releases.
      $ver_grabs[] = _kala_migrate_get_between($td, 'releases/', '">');
    }
    $ver_grabs = array_filter($ver_grabs);
    // Filter out D8 versions.
    foreach ($ver_grabs as $ver) {
      if (stripos($ver, '8.x-') !== FALSE) {
        if (stripos($ver, '-dev') !== FALSE) {
          $vers['dev'] = $ver;
        }
        else {
          $vers['stable'] = $ver;
        }
      }
    }
    // Send this out the door.
    if (isset($vers)) {
      return array_key_exists('stable', $vers) ? $vers['stable'] : $vers['dev'];
    }
  }

  return '';
}
